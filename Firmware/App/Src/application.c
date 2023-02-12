#include <stdio.h>
#include <string.h>
#include "ahrs.h"
#include "application.h"
#include "bmi088.h"
#include "calib.h"
#include "command.h"
#include "config.h"
#include "esc.h"
#include "fc_protocol.h"
#include "led.h"
#include "lis2mdl.h"
#include "lpf.h"
#include "main.h"
#include "matrix.h"
#include "sbus.h"
#include "tim_wrapper.h"
#include "uart_wrapper.h"
#include "usb_wrapper.h"

typedef struct {
    bmi088_t bmi088;
    lis2mdl_t lis2mdl;

    float acc[3], ang[3], mag[3];
    float acc_raw[3], ang_raw[3], mag_raw[3];
} ahrs_sensor_t;

static uint8_t fc_mode = FC_MODE_INIT;

static ahrs_sensor_t ahrs_sensor;
static ahrs_t ahrs;

static lpf3_t lpf_acc, lpf_ang, lpf_mag;

static fc_protocol_channel_t fc_prot_ch;
static fc_packet_t fc_packet_rx, fc_packet_tx;

static sbus_channel_t sbus_ch;
static bool sbus_received;
static sbus_packet_t sbus_packet;

static void standby(void);
static void flight(void);
static void motor_test(void);
static void emer(void);

static void streaming_send(void);
static void ahrs_measure(void *sensor, float acc[3], float ang[3], float mag[3]);

void setup(void) {
    const pwm_channel_t motor_pwm_mapping[4] = {
        PWM_CHANNEL_1,
        PWM_CHANNEL_2,
        PWM_CHANNEL_3,
        PWM_CHANNEL_4,
    };

    control_timer_start();

    /* Initialize the AHRS. */
    bmi088_init(&ahrs_sensor.bmi088, &hspi1, ACC_NSS_GPIO_Port, ACC_NSS_Pin, GYRO_NSS_GPIO_Port, GYRO_NSS_Pin);
    bmi088_set_range(&ahrs_sensor.bmi088, BMI088_ACC_RANGE_24G, BMI088_GYRO_RANGE_2000DPS);
    bmi088_set_odr_bwp(&ahrs_sensor.bmi088, BMI088_ACC_ODR_1600HZ, BMI088_ACC_BWP_NORMAL, BMI088_GYRO_ODR_1000HZ_BWP_116HZ);

    lis2mdl_init(&ahrs_sensor.lis2mdl, &hspi1, MAG_NSS_GPIO_Port, MAG_NSS_Pin);
    lis2mdl_set_odr(&ahrs_sensor.lis2mdl, LIS2MDL_ODR_100Hz);

    ahrs_init(&ahrs, 0.001f, &ahrs_sensor, ahrs_measure);
    ahrs_set_stddev(&ahrs, 0.01f, 0.5f, 0.3f, 0.8f);
    ahrs_init_attitude(&ahrs, 1000U);

    lpf3_init(&lpf_acc, 3U, 0.001f, 50.f);
    lpf3_init(&lpf_ang, 3U, 0.001f, 50.f);
    lpf3_init(&lpf_mag, 3U, 0.001f, 50.f);

    esc_set_motor_pwm_mapping(motor_pwm_mapping);

    /* Initialize the communication channels. */
    fc_protocol_channel_init(&fc_prot_ch, usb_send, usb_receive);
    sbus_channel_init(&sbus_ch, uart1_receive);

    uart1_init();

    /* Initialization end. */
    fc_mode = FC_MODE_STANDBY;
}

void loop(void) {
    uint8_t err;

    if (control_timer_get_flag()) {
        switch (fc_mode) {
        case FC_MODE_STANDBY:
            standby();
            break;
        case FC_MODE_FLIGHT:
            flight();
            break;
        case FC_MODE_MOTOR_TEST:
            motor_test();
            break;
        case FC_MODE_EMER:
            emer();
            break;
        default:
            break;
        }

        streaming_send();

        control_timer_clear_flag();
    }

    if (fc_protocol_channel_receive(&fc_prot_ch, &fc_packet_rx)) {
        /* Validate and execute the command. */
        err = fc_packet_validate(&fc_packet_rx);
        if (err == FC_PACKET_ERR_OK) {
            err = command_execute(fc_packet_rx.dat[0], &fc_packet_rx.dat[1], fc_packet_rx.len - 1U);
        }
        /* Send the response packet. */
        fc_packet_create_response(&fc_packet_tx, err);
        fc_protocol_channel_send(&fc_prot_ch, &fc_packet_tx);
    }

    sbus_received = sbus_channel_receive(&sbus_ch, &sbus_packet);
}

static void standby(void) {
    ahrs_update(&ahrs);

    /* Check whether the PWM is running. */
    if (pwm_is_running()) {
        led_green_write(LED_STATE_ON);
        fc_mode = FC_MODE_MOTOR_TEST;
        return;
    }

    /* Check the arming condition. */
    if (sbus_packet_is_arming_pos(&sbus_packet)) {
        pwm_start();
        led_green_write(LED_STATE_ON);
        fc_mode = FC_MODE_FLIGHT;
        return;
    }
}

static void motor_test(void) {
    ahrs_update(&ahrs);

    /* Check the emergency condition. */
    if (sbus_channel_is_timeout(&sbus_ch) || sbus_packet_is_emergency_switch_on(&sbus_packet)) {
        fc_mode = FC_MODE_EMER;
        return;
    }

    /* Check whether the PWM is not running. */
    if (!pwm_is_running()) {
        led_green_write(LED_STATE_OFF);
        fc_mode = FC_MODE_STANDBY;
        return;
    }
}

static void flight(void) {
    float roll, pitch, yaw_rate;
    float throt_trg, roll_trg, pitch_trg, yaw_rate_trg;
    float roll_err, pitch_err, yaw_rate_err;
    static float roll_prv, pitch_prv, yaw_rate_prv;
    static float roll_err_int, pitch_err_int, yaw_rate_err_int;
    float roll_err_drv, pitch_err_drv, yaw_rate_err_drv;
    float roll_out, pitch_out, yaw_rate_out;
    float throttle[4];

    ahrs_update(&ahrs);

    /* Check the emergency condition. */
    if (sbus_channel_is_timeout(&sbus_ch) || sbus_packet_is_emergency_switch_on(&sbus_packet)) {
        fc_mode = FC_MODE_EMER;
        return;
    }

    /* Check the disarming condition. */
    if (sbus_packet_is_disarming_pos(&sbus_packet)) {
        pwm_stop();
        led_green_write(LED_STATE_OFF);
        fc_mode = FC_MODE_STANDBY;
        return;
    }

    roll = ahrs.rpy[0];
    pitch = ahrs.rpy[1];
    yaw_rate = ahrs_sensor.ang[2];

    if (sbus_received) {
        /* Get the control target. */
        throt_trg = sbus_packet_map_range(sbus_packet.ch[SBUS_THROTTLE_CH], 0.f, 1.f);
        roll_trg = sbus_packet_map_range(sbus_packet.ch[SBUS_AILERON_CH], -MAX_ROLL_TARGET, MAX_ROLL_TARGET);
        pitch_trg = sbus_packet_map_range(sbus_packet.ch[SBUS_ELEVATOR_CH], -MAX_PITCH_TARGET, MAX_PITCH_TARGET);
        yaw_rate_trg = sbus_packet_map_range(sbus_packet.ch[SBUS_RUDDER_CH], -MAX_YAW_RATE_TARGET, MAX_YAW_RATE_TARGET);

        /* Calculate the error. */
        roll_err = roll_trg - roll;
        pitch_err = pitch_trg - pitch;
        yaw_rate_err = yaw_rate_trg - yaw_rate;

        /* Calculate the integral of the error. */
        roll_err_int += roll_err * DT;
        pitch_err_int += pitch_err * DT;
        yaw_rate_err_int += yaw_rate_err * DT;

        /* Calculate the derivative of the error. (Derviation kick reduction applied.) */
        roll_err_drv = (roll - roll_prv) / DT;
        pitch_err_drv = (pitch - pitch_prv) / DT;
        yaw_rate_err_drv = (yaw_rate - yaw_rate_prv) / DT;

        /* Calculate the control output. */
        roll_out = roll_err * ROLL_PITCH_P_GAIN + roll_err_int * ROLL_PITCH_I_GAIN - roll_err_drv * ROLL_PITCH_D_GAIN;
        pitch_out = pitch_err * ROLL_PITCH_P_GAIN + pitch_err_int * ROLL_PITCH_I_GAIN - pitch_err_drv * ROLL_PITCH_D_GAIN;
        yaw_rate_out = yaw_rate_err * YAW_RATE_P_GAIN + yaw_rate_err_int * YAW_RATE_I_GAIN;

        /* Set the throttle. */
        throttle[0] = throt_trg + roll_out + pitch_out;
        throttle[1] = throt_trg - roll_out + pitch_out;
        throttle[2] = throt_trg + roll_out - pitch_out;
        throttle[3] = throt_trg - roll_out - pitch_out;
        if (!esc_set_throttle(throttle)) {
            led_red_write(LED_STATE_ON);
            fc_mode = FC_MODE_EMER;
            return;
        }

        /* Update previous values. */
        roll_prv = roll;
        pitch_prv = pitch;
        yaw_rate_prv = yaw_rate;
    }
}

static void emer(void) {
    pwm_stop();
}

static void streaming_send(void) {
    static uint32_t tick;
    static float cov_state_upper_tri[10], cov_proc_upper_tri[10];
    static const fc_protocol_streaming_field_t fields[15] = {
        { FC_PACKET_DAT_MODE,       1U, &fc_mode            },
        { FC_PACKET_DAT_TICK,       4U, &tick               },
        { FC_PACKET_DAT_ACC,       12U, ahrs_sensor.acc     },
        { FC_PACKET_DAT_ANG,       12U, ahrs_sensor.ang     },
        { FC_PACKET_DAT_MAG,       12U, ahrs_sensor.mag     },
        { FC_PACKET_DAT_RAW_ACC,   12U, ahrs_sensor.acc_raw },
        { FC_PACKET_DAT_RAW_ANG,   12U, ahrs_sensor.ang_raw },
        { FC_PACKET_DAT_RAW_MAG,   12U, ahrs_sensor.mag_raw },
        { FC_PACKET_DAT_KF_QUAT,   16U, ahrs.q              },
        { FC_PACKET_DAT_KF_RPY,    12U, ahrs.rpy            },
        { FC_PACKET_DAT_MEAS_QUAT, 16U, ahrs.q_meas         },
        { FC_PACKET_DAT_MEAS_RPY,  12U, ahrs.rpy_meas       },
        { FC_PACKET_DAT_COV_STATE, 40U, cov_state_upper_tri },
        { FC_PACKET_DAT_COV_PROC,  40U, cov_proc_upper_tri  },
        { FC_PACKET_DAT_RF,        32U, sbus_packet.ch      }
    };

    tick = control_timer_get_tick();
    matrix_get_upper_triangular(ahrs.P, 4, 4, cov_state_upper_tri);
    matrix_get_upper_triangular(ahrs.Q, 4, 4, cov_proc_upper_tri);

    fc_packet_create_streaming(&fc_packet_tx, fields, 15U);
    fc_protocol_channel_send(&fc_prot_ch, &fc_packet_tx);
}

static void ahrs_measure(void *sensor, float acc[3], float ang[3], float mag[3]) {
    ahrs_sensor_t *s;
    float acc_ss_frm[3], ang_ss_frm[3], mag_ss_frm[3];

    s = (ahrs_sensor_t *)sensor;
    bmi088_read_acc(&s->bmi088, s->acc_raw);
    bmi088_read_gyro(&s->bmi088, s->ang_raw);
    lis2mdl_read_mag(&s->lis2mdl, s->mag_raw);

    calib_acc(s->acc_raw, acc_ss_frm);
    calib_gyro(s->ang_raw, ang_ss_frm);
    calib_mag(s->mag_raw, mag_ss_frm);

    s->acc[0] = -acc_ss_frm[0];
    s->acc[1] = acc_ss_frm[1];
    s->acc[2] = -acc_ss_frm[2];
    s->mag[0] = -mag_ss_frm[1];
    s->mag[1] = mag_ss_frm[0];
    s->mag[2] = -mag_ss_frm[2];

    lpf3_update(&lpf_acc, s->acc, s->acc);
    lpf3_update(&lpf_ang, s->ang, s->ang);
    lpf3_update(&lpf_mag, s->mag, s->mag);

    memcpy(acc, s->acc, sizeof(s->acc));
    memcpy(ang, s->ang, sizeof(s->ang));
    memcpy(mag, s->mag, sizeof(s->mag));
}

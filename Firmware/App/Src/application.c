#include <stdio.h>
#include "application.h"
#include "attitude.h"
#include "bmi088.h"
#include "calib.h"
#include "command.h"
#include "config.h"
#include "esc.h"
#include "led.h"
#include "lis2mdl.h"
#include "main.h"
#include "protocol.h"
#include "sbus.h"
#include "streaming.h"
#include "tim_wrapper.h"

typedef enum {
    FC_MODE_INIT,
    FC_MODE_STANDBY,
    FC_MODE_MOTOR_TEST,
    FC_MODE_FLIGHT,
    FC_MODE_EMER,
} fc_mode_t;

static fc_mode_t mode = FC_MODE_INIT;
static sbus_packet_t sbus_packet;

static void standby(void);
static void motor_test(void);
static void flight(void);
static void emer(void);

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

void setup(void) {
    const pwm_channel_t motor_pwm_mapping[4] = {
        PWM_CHANNEL_1,
        PWM_CHANNEL_2,
        PWM_CHANNEL_3,
        PWM_CHANNEL_4,
    };

    control_timer_start();

    bmi088_init();
    bmi088_set_range(BMI088_ACC_RANGE_24G, BMI088_GYRO_RANGE_2000DPS);
    bmi088_set_odr_bwp(BMI088_ACC_ODR_1600HZ, BMI088_ACC_BWP_NORMAL, BMI088_GYRO_ODR_1000HZ_BWP_116HZ);

    lis2mdl_init();
    lis2mdl_set_odr(LIS2MDL_ODR_100Hz);

    attitude_init(1000U);

    esc_set_motor_pwm_mapping(motor_pwm_mapping);

    packet_parser_init();
    sbus_init();

    /* Initialization end. */
    mode = FC_MODE_STANDBY;
}

void loop(void) {
    packet_t packet;
    uint8_t err;

    if (control_timer_get_flag()) {
        switch (mode) {
        case FC_MODE_STANDBY:
            standby();
            break;
        case FC_MODE_MOTOR_TEST:
            motor_test();
            break;
        case FC_MODE_FLIGHT:
            flight();
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

    if ((mode == FC_MODE_STANDBY) && packet_receive(&packet)) {
        err = packet_validate(&packet);
        if (err == ERR_OK) {
            err = command_execute(packet.dat[0], &packet.dat[1], packet.len - 1U);
        }
        response_send(err);
    }

    if (sbus_packet_receive(&sbus_packet)) {
        for (uint16_t i = 0U; i < SBUS_CH_NUM; i++) {
            rf_ch[i] = (float)sbus_packet.ch[i];
        }
    }
}

static void standby(void) {
    attitude_update();

    /* Check whether the PWM is running. */
    if (pwm_is_running()) {
        led_green_write(LED_STATE_ON);
        mode = FC_MODE_MOTOR_TEST;
        return;
    }

    /* Check the arming condition. */
    if (sbus_packet_is_arming_pos(&sbus_packet)) {
        pwm_start();
        led_green_write(LED_STATE_ON);
        mode = FC_MODE_FLIGHT;
        return;
    }
}

static void motor_test(void) {
    attitude_update();

    /* Check the emergency condition. */
    if (sbus_is_timeout() || sbus_packet_is_emergency_switch_on(&sbus_packet)) {
        mode = FC_MODE_EMER;
        return;
    }

    /* Check whether the PWM is not running. */
    if (!pwm_is_running()) {
        led_green_write(LED_STATE_OFF);
        mode = FC_MODE_STANDBY;
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

    attitude_update();

    /* Check the emergency condition. */
    if (sbus_is_timeout() || sbus_packet_is_emergency_switch_on(&sbus_packet)) {
        mode = FC_MODE_EMER;
        return;
    }

    /* Check the disarming condition. */
    if (sbus_packet_is_disarming_pos(&sbus_packet)) {
        pwm_stop();
        led_green_write(LED_STATE_OFF);
        mode = FC_MODE_STANDBY;
        return;
    }

    roll = rpy[0];
    pitch = rpy[1];
    yaw_rate = ang[2];

    /* Get the control target. */
    throt_trg = sbus_ch_map_range(sbus_packet.ch[SBUS_THROTTLE_CH], 0.f, 1.f);
    roll_trg = sbus_ch_map_range(sbus_packet.ch[SBUS_AILERON_CH], -MAX_ROLL_TARGET, MAX_ROLL_TARGET);
    pitch_trg = sbus_ch_map_range(sbus_packet.ch[SBUS_ELEVATOR_CH], -MAX_PITCH_TARGET, MAX_PITCH_TARGET);
    yaw_rate_trg = sbus_ch_map_range(sbus_packet.ch[SBUS_RUDDER_CH], -MAX_YAW_RATE_TARGET, MAX_YAW_RATE_TARGET);

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
        mode = FC_MODE_EMER;
        return;
    }

    /* Update previous values. */
    roll_prv = roll;
    pitch_prv = pitch;
    yaw_rate_prv = yaw_rate;
}

static void emer(void) {
    pwm_stop();
}

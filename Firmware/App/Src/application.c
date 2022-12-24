#include "application.h"
#include "bmi088.h"
#include "calib.h"
#include "command.h"
#include "esc.h"
#include "lis2mdl.h"
#include "main.h"
#include "protocol.h"
#include "streaming.h"
#include "tim_wrapper.h"

typedef enum {
    MODE_INIT    = 0x00,
    MODE_STANDBY = 0x01,
    MODE_FLIGHT  = 0x02,
    MODE_EMER    = 0x03,
} mode_t;

static mode_t mode = MODE_INIT;

static void standby(void);
static void flight(void);
static void emer(void);

void setup(void) {
    control_timer_start();

    packet_parser_init();

    bmi088_init();
    bmi088_set_range(BMI088_ACC_RANGE_24G, BMI088_GYRO_RANGE_2000DPS);
    bmi088_set_odr_bwp(BMI088_ACC_ODR_1600HZ, BMI088_ACC_BWP_NORMAL, BMI088_GYRO_ODR_1000HZ_BWP_116HZ);

    lis2mdl_init();
    lis2mdl_set_odr(LIS2MDL_ODR_100Hz);

    esc_set_protocol(ESC_PRTCL_ONESHOT125);

    /* Initialization end. */
    mode = MODE_STANDBY;
}

void loop(void) {
    packet_t packet;
    uint8_t err;

    if (control_timer_get_flag()) {
        switch (mode) {
        case MODE_STANDBY:
            standby();
            break;
        case MODE_FLIGHT:
            flight();
            break;
        case MODE_EMER:
            emer();
            break;
        default:
            break;
        }

        control_timer_clear_flag();
    }

    if ((mode == MODE_STANDBY) && packet_receive(&packet)) {
        err = packet_validate(&packet);
        if (err == ERR_OK) {
            err = command_execute(packet.dat[0], &packet.dat[1], packet.len - 1U);
        }
        response_send(err);
    }
}

static void standby(void) {
    float acc_ss_frm[3], ang_ss_frm[3], mag_ss_frm[3];

    bmi088_read_acc(acc_raw);
    bmi088_read_gyro(ang_raw);
    lis2mdl_read_mag(mag_raw);

    /* Calibrate. */
    calib_acc(acc_raw, acc_ss_frm);
    calib_gyro(ang_raw, ang_ss_frm);
    calib_mag(mag_raw, mag_ss_frm);

    /* Change axes from the sensor frame to the FC frame. */
    acc[0] = -acc_ss_frm[0];
    acc[1] = acc_ss_frm[1];
    acc[2] = -acc_ss_frm[2];
    ang[0] = -ang_ss_frm[0];
    ang[1] = ang_ss_frm[1];
    ang[2] = -ang_ss_frm[2];
    mag[0] = -mag_ss_frm[1];
    mag[1] = mag_ss_frm[0];
    mag[2] = -mag_ss_frm[2];

    esc_send_throttle();

    streaming_send();
}

static void flight(void) {

}

static void emer(void) {
    esc_set_throttle(MOTOR_POSITION_FRONT_LEFT, 0.f);
    esc_set_throttle(MOTOR_POSITION_FRONT_RIGHT, 0.f);
    esc_set_throttle(MOTOR_POSITION_REAR_LEFT, 0.f);
    esc_set_throttle(MOTOR_POSITION_REAR_RIGHT, 0.f);

    esc_send_throttle();
}

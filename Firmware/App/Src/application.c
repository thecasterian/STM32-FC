#include <stdio.h>
#include "application.h"
#include "attitude.h"
#include "bmi088.h"
#include "calib.h"
#include "command.h"
#include "esc.h"
#include "lis2mdl.h"
#include "main.h"
#include "protocol.h"
#include "sbus.h"
#include "streaming.h"
#include "tim_wrapper.h"

typedef enum {
    FC_MODE_INIT    = 0x00,
    FC_MODE_STANDBY = 0x01,
    FC_MODE_FLIGHT  = 0x02,
    FC_MODE_EMER    = 0x03,
} fc_mode_t;

static fc_mode_t mode = FC_MODE_INIT;
static sbus_packet_t sbus_packet;

static void standby(void);
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

    /* Wait RF. */
    while (!sbus_packet_receive(&sbus_packet)) {}

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
        case FC_MODE_FLIGHT:
            flight();
            break;
        case FC_MODE_EMER:
            emer();
            break;
        default:
            break;
        }

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

    /* Check the emergency condition. */
    if (sbus_is_timeout() || sbus_packet_is_emergency_switch_on(&sbus_packet)) {
        mode = FC_MODE_EMER;
        return;
    }

    /* Check the arming condition. */
    if (sbus_packet_is_arming_pos(&sbus_packet)) {
        pwm_start();
        mode = FC_MODE_FLIGHT;
        return;
    }

    streaming_send();
}

static void flight(void) {
    static float throttle[4];

    attitude_update();

    /* Check the emergency condition. */
    if (sbus_is_timeout() || sbus_packet_is_emergency_switch_on(&sbus_packet)) {
        mode = FC_MODE_EMER;
        return;
    }

    /* Check the disarming condition. */
    if (sbus_packet_is_disarming_pos(&sbus_packet)) {
        pwm_stop();
        mode = FC_MODE_STANDBY;
        return;
    }

    /* Set throttle. */
    for (int16_t i = 0; i < 4; i++) {
        throttle[i] = sbus_ch_map_range(sbus_packet.ch[SBUS_THROTTLE_CH], 0.f, 0.1f);
    }
    esc_set_throttle(throttle);
}

static void emer(void) {
    pwm_stop();
}

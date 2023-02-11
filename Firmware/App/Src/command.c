#include <stddef.h>
#include "command.h"
#include "esc.h"
#include "fc_protocol.h"
#include "util.h"

#define COMMAND_HANDLER_LIST_INIT(ID, HDL) [(ID) - FC_PACKET_CMD_START] = (HDL)

static uint8_t handler_cmd_throt(const uint8_t arg[], uint8_t arglen);
static uint8_t handler_cmd_pwm(const uint8_t arg[], uint8_t arglen);

typedef uint8_t (*command_handler_t)(const uint8_t arg[], uint8_t arglen);
static const command_handler_t command_handler_list[FC_PACKET_CMD_END - FC_PACKET_CMD_START] = {
    COMMAND_HANDLER_LIST_INIT( FC_PACKET_CMD_PWM,   handler_cmd_pwm   ),
    COMMAND_HANDLER_LIST_INIT( FC_PACKET_CMD_THROT, handler_cmd_throt ),
};

uint8_t command_execute(uint8_t cmd, const uint8_t arg[], uint8_t arglen) {
    uint8_t err;

    if ((cmd < FC_PACKET_CMD_START) || (cmd >= FC_PACKET_CMD_END) ||
        (command_handler_list[cmd - FC_PACKET_CMD_START] == NULL)) {
        err = FC_PACKET_ERR_CMD_INVAL;
    } else {
        err = command_handler_list[cmd - FC_PACKET_CMD_START](arg, arglen);
    }

    return err;
}

static uint8_t handler_cmd_pwm(const uint8_t arg[], uint8_t arglen) {
    uint8_t err;

    if (arglen != 1U) {
        err = FC_PACKET_ERR_LEN_INVAL;
    } else if (!is_boolean_u8(arg[0])) {
        err = FC_PACKET_ERR_ARG_INVAL;
    } else {
        if (arg[0] == 1U) {
            pwm_start();
        } else {
            pwm_stop();
        }
        err = FC_PACKET_ERR_OK;
    }

    return err;
}

static uint8_t handler_cmd_throt(const uint8_t arg[], uint8_t arglen) {
    uint8_t err;
    float throttle[4];

    err = FC_PACKET_ERR_OK;
    if (arglen != 4U) {
        err = FC_PACKET_ERR_LEN_INVAL;
    } else {
        for (int16_t i = 0; i < 4; i++) {
            throttle[i] = (float)arg[i] / 100.f;

            if ((throttle[i] < 0.f) || (throttle[i] > 1.f)) {
                err = FC_PACKET_ERR_ARG_INVAL;
                break;
            }
        }

        if (err == FC_PACKET_ERR_OK) {
            esc_set_throttle(throttle);
        }
    }

    return err;
}

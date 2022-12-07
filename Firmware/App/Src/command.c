#include <stddef.h>
#include "command.h"
#include "led.h"
#include "streaming.h"
#include "util.h"

#define CMD_HDL_LIST_INIT(ID, HDL) [ID - CMD_START] = HDL

static uint8_t cmd_hdl_led_red(const uint8_t *arg, uint8_t arglen);
static uint8_t cmd_hdl_led_green(const uint8_t *arg, uint8_t arglen);
static uint8_t cmd_hdl_led_blue(const uint8_t *arg, uint8_t arglen);
static uint8_t cmd_hdl_strm_dat(const uint8_t *arg, uint8_t arglen);
static uint8_t cmd_hdl_strm(const uint8_t *arg, uint8_t arglen);

typedef uint8_t (*cmd_hdl_t)(const uint8_t *arg, uint8_t arglen);
static const cmd_hdl_t cmd_hdl_list[CMD_END - CMD_START] = {
    CMD_HDL_LIST_INIT( CMD_LED_RED,   cmd_hdl_led_red   ),
    CMD_HDL_LIST_INIT( CMD_LED_GREEN, cmd_hdl_led_green ),
    CMD_HDL_LIST_INIT( CMD_LED_BLUE,  cmd_hdl_led_blue  ),
    CMD_HDL_LIST_INIT( CMD_STRM_DAT,  cmd_hdl_strm_dat  ),
    CMD_HDL_LIST_INIT( CMD_STRM,      cmd_hdl_strm      ),
};

uint8_t command_execute(uint8_t cmd, const uint8_t *arg, uint8_t arglen) {
    uint8_t err;

    if ((cmd < CMD_START) || (cmd >= CMD_END) || (cmd_hdl_list[cmd - CMD_START] == NULL)) {
        err = ERR_CMD_INVAL;
    } else {
        err = cmd_hdl_list[cmd - CMD_START](arg, arglen);
    }

    return err;
}

static uint8_t cmd_hdl_led_red(const uint8_t *arg, uint8_t arglen) {
    uint8_t err;

    if (arglen != 1U) {
        err = ERR_LEN_INVAL;
    } else if (!is_boolean_u8(arg[0])) {
        err = ERR_ARG_INVAL;
    } else {
        led_red_write(arg[0]);
        err = ERR_OK;
    }

    return err;
}

static uint8_t cmd_hdl_led_green(const uint8_t *arg, uint8_t arglen) {
    uint8_t err;

    if (arglen != 1U) {
        err = ERR_LEN_INVAL;
    } else if (!is_boolean_u8(arg[0])) {
        err = ERR_ARG_INVAL;
    } else {
        led_green_write(arg[0]);
        err = ERR_OK;
    }

    return err;
}

static uint8_t cmd_hdl_led_blue(const uint8_t *arg, uint8_t arglen) {
    uint8_t err;

    if (arglen != 2U) {
        err = ERR_LEN_INVAL;
    } else if (!is_boolean_u8(arg[0])) {
        err = ERR_ARG_INVAL;
    } else {
        led_blue_write(arg[0]);
        err = ERR_OK;
    }

    return err;
}

static uint8_t cmd_hdl_strm_dat(const uint8_t *arg, uint8_t arglen) {
    uint8_t err;

    err = ERR_OK;
    for (uint16_t i = 0U; i < arglen; i++) {
        if ((arg[i] < DAT_START) || (arg[i] >= DAT_END) || (strm_dat_list[arg[i] - DAT_START].dat == NULL)) {
            err = ERR_ARG_INVAL;
            break;
        }
    }

    if (err == ERR_OK) {
        for (uint16_t i = 0U; i < DAT_END - DAT_START; i++) {
            strm_dat_sel[i] = false;
        }

        for (uint16_t i = 0U; i < arglen; i++) {
            strm_dat_sel[arg[i] - DAT_START] = true;
        }
    }

    return err;
}

static uint8_t cmd_hdl_strm(const uint8_t *arg, uint8_t arglen) {
    uint8_t err;

    if (arglen != 1U) {
        err = ERR_LEN_INVAL;
    } else if (!is_boolean_u8(arg[0])) {
        err = ERR_ARG_INVAL;
    } else {
        strm_en = arg[0];
        err = ERR_OK;
    }

    return err;
}

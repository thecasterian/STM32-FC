#include <string.h>
#include "application.h"
#include "led.h"
#include "protocol.h"
#include "ring_buffer.h"
#include "streaming_data.h"
#include "usb_wrapper.h"
#include "util.h"

#define CMD_HDL_LIST_INIT(ID, HDL) [ID - CMD_START] = HDL
#define STRM_DAT_LIST_INIT(ID, DAT, SIZE) [ID - DAT_START] = { .dat = DAT, .size = SIZE }

static struct {
    packet_t packet;                                        /* Received packet. */

    bool stx;                                               /* Is STX found? */
    uint16_t cnt;                                           /* Number of received bytes. */
} packet_receiver;

static uint8_t cmd_hdl_led_red(const uint8_t *dat, uint8_t len);
static uint8_t cmd_hdl_led_green(const uint8_t *dat, uint8_t len);
static uint8_t cmd_hdl_led_blue(const uint8_t *dat, uint8_t len);
static uint8_t cmd_hdl_strm_dat(const uint8_t *dat, uint8_t len);
static uint8_t cmd_hdl_strm(const uint8_t *dat, uint8_t len);

typedef uint8_t (*cmd_hdl_t)(const uint8_t *dat, uint8_t len);
static const cmd_hdl_t cmd_hdl_list[CMD_END - CMD_START] = {
    CMD_HDL_LIST_INIT( CMD_LED_RED,   cmd_hdl_led_red   ),
    CMD_HDL_LIST_INIT( CMD_LED_GREEN, cmd_hdl_led_green ),
    CMD_HDL_LIST_INIT( CMD_LED_BLUE,  cmd_hdl_led_blue  ),
    CMD_HDL_LIST_INIT( CMD_STRM_DAT,  cmd_hdl_strm_dat  ),
    CMD_HDL_LIST_INIT( CMD_STRM,      cmd_hdl_strm      ),
};

/* Is streaming enabled? */
static bool strm_en;
/* Is streaming data selected? */
static bool strm_dat_sel[DAT_END - DAT_START];

/* List of streaming data. */
static const struct {
    float *dat;
    uint8_t size;
} strm_dat_list[DAT_END - DAT_START] = {
    STRM_DAT_LIST_INIT( DAT_ACC,          acc,          12U ),
    STRM_DAT_LIST_INIT( DAT_ANG,          ang,          12U ),
    STRM_DAT_LIST_INIT( DAT_MAG,          mag,          12U ),
    STRM_DAT_LIST_INIT( DAT_PRES,         &pres,         4U ),
    STRM_DAT_LIST_INIT( DAT_TEMP,         &temp,         4U ),
    STRM_DAT_LIST_INIT( DAT_RAW_ACC,      acc_raw,      12U ),
    STRM_DAT_LIST_INIT( DAT_RAW_GYRO,     ang_raw,      12U ),
    STRM_DAT_LIST_INIT( DAT_RAW_MAG,      mag_raw,      12U ),
    STRM_DAT_LIST_INIT( DAT_KF_QUAT,      &q.w,         16U ),
    STRM_DAT_LIST_INIT( DAT_KF_RPY,       rpy,          12U ),
    STRM_DAT_LIST_INIT( DAT_KF_VEL,       vel,          12U ),
    STRM_DAT_LIST_INIT( DAT_KF_POS,       pos,          12U ),
    STRM_DAT_LIST_INIT( DAT_EXT_ACC,      acc_ext,      12U ),
    STRM_DAT_LIST_INIT( DAT_ACC_MAG_QUAT, &q_acc_mag.w, 16U ),
    STRM_DAT_LIST_INIT( DAT_ACC_MAG_RPY,  rpy_acc_mag,  12U ),
    STRM_DAT_LIST_INIT( DAT_BARO_HEIGHT,  &baro_height,  4U ),
};

static uint8_t calc_checksum(const uint8_t *buf, uint8_t size);

bool packet_receive(packet_t *packet) {
    uint8_t c;
    bool res;
    bool success;

    res = false;

    if (!packet_receiver.stx) {
        /* Find STX. */
        while (true) {
            success = usb_ringbuf_pop(&c, 1U);
            if (!success || (c == STX)) {
                break;
            }
        }

        if (success && (c == STX)) {
            /* STX is found. */
            packet_receiver.stx = true;
            packet_receiver.cnt++;
        } else {
            packet_receiver.stx = false;
            packet_receiver.cnt = 0U;
        }
    }

    if (packet_receiver.stx) {
        while (true) {
            success = usb_ringbuf_pop(&c, 1U);
            if (!success) {
                break;
            }

            if (packet_receiver.cnt == 1U) {
                /* Byte 1 is TYP. */
                packet_receiver.packet.typ = c;
            } else if (packet_receiver.cnt == 2U) {
                /* Byte 2 is LEN. */
                packet_receiver.packet.len = c;
            } else if (packet_receiver.cnt < (packet_receiver.packet.len + 3U)) {
                /* Byte 3 .. (LEN + 2) are in DAT. */
                packet_receiver.packet.dat[packet_receiver.cnt - 3U] = c;
            } else if (packet_receiver.cnt == (packet_receiver.packet.len + 3U)) {
                /* Byte (LEN + 3) is CKS. */
                packet_receiver.packet.cks = c;
            } else {
                /* Last byte is ETX. */
                packet_receiver.packet.etx = c;

                res = true;
                packet_receiver.stx = false;
                packet_receiver.cnt = 0U;

                /* Copy packet. */
                memcpy(packet, &packet_receiver.packet, sizeof(packet_t));

                break;
            }

            packet_receiver.cnt++;
        }
    }

    return res;
}

uint8_t command_execute(packet_t *packet) {
    uint8_t err;

    /* Is the packet valid? */
    if (packet->etx != ETX) {
        err = ERR_ETX_MIS;
    } else if (calc_checksum(packet->dat, packet->len) != packet->cks) {
        err = ERR_CKS_MIS;
    } else if (packet->typ != TYP_CMD) {
        err = ERR_TYP_INVAL;
    } else if ((packet->dat[0] < CMD_START) || (packet->dat[0] >= CMD_END)
               || (cmd_hdl_list[packet->dat[0] - CMD_START] == NULL)) {
        err = ERR_CMD_INVAL;
    } else {
        /* Execute the command. */
        err = cmd_hdl_list[packet->dat[0] - CMD_START](packet->dat, packet->len);
    }

    return err;
}

void response_send(uint8_t err) {
    uint8_t buf[7];

    buf[0] = STX;
    buf[1] = TYP_RESP;
    buf[2] = 2U;
    buf[3] = err == ERR_OK ? ACK : NACK;
    buf[4] = err;
    buf[5] = calc_checksum(&buf[3], 2U);
    buf[6] = ETX;

    usb_transmit(buf, sizeof(buf));
}

void stream_send(void) {
    uint8_t buf[PROTOCOL_LEN_MAX + 5];

    if (strm_en) {
        buf[0] = STX;
        buf[1] = TYP_STRM;

        buf[2] = 0U;
        for (int16_t i = 0; i < DAT_END - DAT_START; i++) {
            if (strm_dat_sel[i]) {
                memcpy(&buf[buf[2] + 3U], strm_dat_list[i].dat, strm_dat_list[i].size);
                buf[2] += strm_dat_list[i].size;
            }
        }

        buf[buf[2] + 3U] = calc_checksum(&buf[3], buf[2]);
        buf[buf[2] + 4U] = ETX;

        usb_transmit(buf, buf[2] + 5U);
    }
}

static uint8_t cmd_hdl_led_red(const uint8_t *dat, uint8_t len) {
    uint8_t err;

    if (len != 2U) {
        err = ERR_LEN_INVAL;
    } else if (!is_boolean_u8(dat[1])) {
        err = ERR_ARG_INVAL;
    } else {
        led_red_write(dat[1]);
        err = ERR_OK;
    }

    return err;
}

static uint8_t cmd_hdl_led_green(const uint8_t *dat, uint8_t len) {
    uint8_t err;

    if (len != 2U) {
        err = ERR_LEN_INVAL;
    } else if (!is_boolean_u8(dat[1])) {
        err = ERR_ARG_INVAL;
    } else {
        led_green_write(dat[1]);
        err = ERR_OK;
    }

    return err;
}

static uint8_t cmd_hdl_led_blue(const uint8_t *dat, uint8_t len) {
    uint8_t err;

    if (len != 2U) {
        err = ERR_LEN_INVAL;
    } else if (!is_boolean_u8(dat[1])) {
        err = ERR_ARG_INVAL;
    } else {
        led_blue_write(dat[1]);
        err = ERR_OK;
    }

    return err;
}

static uint8_t cmd_hdl_strm_dat(const uint8_t *dat, uint8_t len) {
    uint8_t err;

    err = ERR_OK;
    for (uint16_t i = 1U; i < len; i++) {
        if ((dat[i] < DAT_START) || (dat[i] >= DAT_END) || (strm_dat_list[dat[i] - DAT_START].dat == NULL)) {
            err = ERR_ARG_INVAL;
            break;
        }
    }

    if (err == ERR_OK) {
        for (uint16_t i = 0U; i < DAT_END - DAT_START; i++) {
            strm_dat_sel[i] = false;
        }

        for (uint16_t i = 1U; i < len; i++) {
            strm_dat_sel[dat[i] - DAT_START] = true;
        }
    }

    return err;
}

static uint8_t cmd_hdl_strm(const uint8_t *dat, uint8_t len) {
    uint8_t err;

    if (len != 2U) {
        err = ERR_LEN_INVAL;
    } else if (!is_boolean_u8(dat[1])) {
        err = ERR_ARG_INVAL;
    } else {
        strm_en = dat[1];
        err = ERR_OK;
    }

    return err;
}

static uint8_t calc_checksum(const uint8_t *buf, uint8_t size) {
    uint8_t checksum;

    checksum = 0U;
    for (uint16_t i = 0U; i < size; i++) {
        checksum += buf[i];
    }

    return checksum;
}

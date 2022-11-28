#include <string.h>
#include "application.h"
#include "gpio.h"
#include "protocol.h"
#include "streaming_data.h"
#include "usb_queue.h"
#include "usbd_cdc_if.h"

/* Is streaming enabled? */
static bool strm_en;
/* Is streaming data selected? */
static bool strm_dat_sel[DAT_NUM];

/* List of streaming data. */
static const struct {
    float *dat;
    uint8_t size;
} strm_dat_list[DAT_NUM] = {
    [DAT_ACC]          = { .dat = acc,          .size = 12U },
    [DAT_ANG]          = { .dat = ang,          .size = 12U },
    [DAT_MAG]          = { .dat = mag,          .size = 12U },
    [DAT_PRES]         = { .dat = &pres,        .size = 4U  },
    [DAT_TEMP]         = { .dat = &temp,        .size = 4U  },
    [DAT_RAW_ACC]      = { .dat = acc_raw,      .size = 12U },
    [DAT_RAW_GYRO]     = { .dat = ang_raw,      .size = 12U },
    [DAT_RAW_MAG]      = { .dat = mag_raw,      .size = 12U },
    [DAT_KF_QUAT]      = { .dat = &q.w,         .size = 16U },
    [DAT_KF_RPY]       = { .dat = rpy,          .size = 12U },
    [DAT_KF_VEL]       = { .dat = vel,          .size = 12U },
    [DAT_KF_POS]       = { .dat = pos,          .size = 12U },
    [DAT_EXT_ACC]      = { .dat = acc_ext,      .size = 12U },
    [DAT_ACC_MAG_QUAT] = { .dat = &q_acc_mag.w, .size = 16U },
    [DAT_ACC_MAG_RPY]  = { .dat = rpy_acc_mag,  .size = 12U },
    [DAT_BARO_HEIGHT]  = { .dat = &baro_height, .size = 4U  },
};

static void calc_checksum(const uint8_t *buf, uint8_t size, uint8_t *checksum);
static void validate_data_id(uint8_t id, bool *valid);

void packet_recver_init(packet_recver_t *packet_recver) {
    packet_recver->recved = false;
    memset(&packet_recver->packet, 0, sizeof(packet_recver->packet));

    packet_recver->stx = false;
    packet_recver->cnt = 0U;
}

void packet_recver_recv(packet_recver_t *packet_recver) {
    uint8_t c;
    bool success;

    packet_recver->recved = false;

    if (!packet_recver->stx) {
        /* Find STX. */
        while (true) {
            usb_queue_pop(&usb_queue, &c, 1U, &success);
            if (!success || (c == STX)) {
                break;
            }
        }

        if (success && (c == STX)) {
            /* STX is found. */
            packet_recver->stx = true;
            packet_recver->cnt++;
        } else {
            packet_recver->stx = false;
            packet_recver->cnt = 0U;
        }
    }

    if (packet_recver->stx) {
        while (true) {
            usb_queue_pop(&usb_queue, &c, 1U, &success);
            if (!success) {
                break;
            }

            if (packet_recver->cnt == 1U) {
                /* Byte 1 is TYP. */
                packet_recver->packet.typ = c;
            } else if (packet_recver->cnt == 2U) {
                /* Byte 2 is LEN. */
                packet_recver->packet.len = c;
            } else if (packet_recver->cnt < (packet_recver->packet.len + 3U)) {
                /* Byte 3 .. (LEN + 2) are in DAT. */
                packet_recver->packet.dat[packet_recver->cnt - 3U] = c;
            } else if (packet_recver->cnt == (packet_recver->packet.len + 3U)) {
                /* Byte (LEN + 3) is CKS. */
                packet_recver->packet.cks = c;
            } else {
                /* Last byte is ETX. */
                packet_recver->packet.etx = c;

                packet_recver->recved = true;
                packet_recver->stx = false;
                packet_recver->cnt = 0U;

                break;
            }

            packet_recver->cnt++;
        }
    }
}

void packet_validate(packet_t *packet, uint8_t *err) {
    uint8_t cks;
    bool valid;
    uint16_t strm_len;

    *err = ERR_OK;
    calc_checksum(packet->dat, packet->len, &cks);

    if (packet->etx != ETX) {
        *err = ERR_ETX_MIS;
    } else if (cks != packet->cks) {
        *err = ERR_CKS_MIS;
    } else if (packet->typ != TYP_CMD) {
        *err = ERR_TYP_INVAL;
    } else {
        switch (packet->dat[0]) {
        case CMD_LED_RED:
        case CMD_LED_GREEN:
        case CMD_LED_BLUE:
        case CMD_STRM:
            if (packet->len != 2U) {
                /* Length of ARG is not 1 byte. */
                *err = ERR_LEN_INVAL;
            } else if (packet->dat[1] > 1U) {
                /* Argument is not 0 nor 1. */
                *err = ERR_ARG_INVAL;
            }
            break;
        case CMD_STRM_DAT:
            /* Variable length; no need to validate length. */
            strm_len = 0U;
            for (uint8_t i = 1U; i < packet->len; i++) {
                validate_data_id(packet->dat[i], &valid);
                if (!valid) {
                    /* Streaming data ID is invalid. */
                    *err = ERR_ARG_INVAL;
                    break;
                }
                strm_len += strm_dat_list[packet->dat[i]].size;
            }
            if (strm_len > PROTOCOL_LEN_MAX) {
                /* Streaming data is too large. */
                *err = ERR_ARG_INVAL;
            }
            break;
        default:
            *err = ERR_CMD_INVAL;
            break;
        }
    }
}

void command_execute(packet_t *packet) {
    switch (packet->dat[0]) {
    case CMD_LED_RED:
        HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, packet->dat[1] == 1U ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    case CMD_LED_GREEN:
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, packet->dat[1] == 1U ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    case CMD_LED_BLUE:
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, packet->dat[1] == 1U ? GPIO_PIN_SET : GPIO_PIN_RESET);
        break;
    case CMD_STRM_DAT:
        memset(strm_dat_sel, 0, sizeof(strm_dat_sel));
        for (uint8_t i = 1U; i < packet->len; i++) {
            strm_dat_sel[packet->dat[i]] = true;
        }
        break;
    case CMD_STRM:
        strm_en = packet->dat[1] == 1U ? true : false;
        break;
    default:
        /* Do nothing. */
        break;
    }
}

void response_send(uint8_t err) {
    uint8_t buf[7];

    buf[0] = STX;
    buf[1] = TYP_RESP;
    buf[2] = 2U;
    buf[3] = err == ERR_OK ? ACK : NACK;
    buf[4] = err;
    calc_checksum(&buf[3], 2U, &buf[5]);
    buf[6] = ETX;

    CDC_Transmit_FS(buf, sizeof(buf));
}

void stream_send(void) {
    uint8_t buf[PROTOCOL_LEN_MAX + 5];

    if (strm_en) {
        buf[0] = STX;
        buf[1] = TYP_STRM;

        buf[2] = 0U;
        for (int16_t i = 0; i < DAT_NUM; i++) {
            if (strm_dat_sel[i]) {
                memcpy(&buf[buf[2] + 3U], strm_dat_list[i].dat, strm_dat_list[i].size);
                buf[2] += strm_dat_list[i].size;
            }
        }

        calc_checksum(&buf[3], buf[2], &buf[buf[2] + 3U]);
        buf[buf[2] + 4U] = ETX;

        CDC_Transmit_FS(buf, buf[2] + 5U);
    }
}

static void calc_checksum(const uint8_t *buf, uint8_t size, uint8_t *checksum) {
    *checksum = 0U;
    for (uint16_t i = 0U; i < size; i++) {
        *checksum += buf[i];
    }
}

static void validate_data_id(uint8_t id, bool *valid) {
    switch (id) {
    case DAT_ACC:
    case DAT_ANG:
    case DAT_MAG:
    case DAT_PRES:
    case DAT_TEMP:
    case DAT_RAW_ACC:
    case DAT_RAW_GYRO:
    case DAT_RAW_MAG:
    case DAT_KF_QUAT:
    case DAT_KF_RPY:
    case DAT_KF_VEL:
    case DAT_KF_POS:
    case DAT_EXT_ACC:
    case DAT_ACC_MAG_QUAT:
    case DAT_ACC_MAG_RPY:
    case DAT_BARO_HEIGHT:
        *valid = true;
        break;
    default:
        *valid = false;
        break;
    }
}

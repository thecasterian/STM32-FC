#include <string.h>
#include "application.h"
#include "gpio.h"
#include "protocol.h"
#include "ring_buffer.h"
#include "streaming_data.h"
#include "usbd_cdc_if.h"

static struct {
    packet_t packet;                                        /* Received packet. */

    bool stx;                                               /* Is STX found? */
    uint16_t cnt;                                           /* Number of received bytes. */
} packet_receiver;

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
    } else {
        // TODO: execute command.
        err = ERR_OK;
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

        buf[buf[2] + 3U] = calc_checksum(&buf[3], buf[2]);
        buf[buf[2] + 4U] = ETX;

        CDC_Transmit_FS(buf, buf[2] + 5U);
    }
}

static uint8_t calc_checksum(const uint8_t *buf, uint8_t size) {
    uint8_t checksum;

    checksum = 0U;
    for (uint16_t i = 0U; i < size; i++) {
        checksum += buf[i];
    }

    return checksum;
}

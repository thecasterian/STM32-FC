#include <string.h>
#include "protocol.h"
#include "ring_buffer.h"
#include "usb_wrapper.h"
#include "util.h"

static struct {
    packet_t packet;                                        /* Received packet. */

    bool stx;                                               /* Is STX found? */
    uint16_t cnt;                                           /* Number of received bytes. */
} parser;

void packet_parser_init(void) {
    parser.stx = false;
    parser.cnt = 0U;
}

bool packet_receive(packet_t *packet) {
    uint8_t c;
    bool res;
    bool success;

    res = false;

    if (!parser.stx) {
        /* Find STX. */
        while (true) {
            success = usb_ringbuf_pop(&c, 1U);
            if (!success || (c == STX)) {
                break;
            }
        }

        if (success && (c == STX)) {
            /* STX is found. */
            parser.stx = true;
            parser.cnt++;
        } else {
            parser.stx = false;
            parser.cnt = 0U;
        }
    }

    if (parser.stx) {
        while (true) {
            success = usb_ringbuf_pop(&c, 1U);
            if (!success) {
                break;
            }

            if (parser.cnt == 1U) {
                /* Byte 1 is TYP. */
                parser.packet.typ = c;
            } else if (parser.cnt == 2U) {
                /* Byte 2 is LEN. */
                parser.packet.len = c;
            } else if (parser.cnt < (parser.packet.len + 3U)) {
                /* Byte 3 .. (LEN + 2) are in DAT. */
                parser.packet.dat[parser.cnt - 3U] = c;
            } else if (parser.cnt == (parser.packet.len + 3U)) {
                /* Byte (LEN + 3) is CKS. */
                parser.packet.cks = c;
            } else {
                /* Last byte is ETX. */
                parser.packet.etx = c;

                res = true;
                parser.stx = false;
                parser.cnt = 0U;

                /* Copy packet. */
                memcpy(packet, &parser.packet, sizeof(packet_t));

                break;
            }

            parser.cnt++;
        }
    }

    return res;
}

uint8_t packet_validate(packet_t *packet) {
    uint8_t err;

    if (packet->etx != ETX) {
        /* ETX is missing. */
        err = ERR_ETX_MIS;
    } else if (packet_checksum_calc(packet->dat, packet->len) != packet->cks) {
        /* Checksum mismatch. */
        err = ERR_CKS_MIS;
    } else if (packet->typ != TYP_CMD) {
        /* Received packet must be a command packet. */
        err = ERR_TYP_INVAL;
    } else if (packet->len == 0U) {
        /* No command. */
        err = ERR_CMD_INVAL;
    } else {
        err = ERR_OK;
    }

    return err;
}

uint8_t packet_checksum_calc(const uint8_t buf[], uint8_t size) {
    uint8_t checksum;

    checksum = 0U;
    for (uint16_t i = 0U; i < size; i++) {
        checksum += buf[i];
    }

    return checksum;
}

void response_send(uint8_t err) {
    uint8_t buf[7];

    buf[0] = STX;
    buf[1] = TYP_RESP;
    buf[2] = 2U;
    buf[3] = err == ERR_OK ? ACK : NACK;
    buf[4] = err;
    buf[5] = packet_checksum_calc(&buf[3], 2U);
    buf[6] = ETX;

    usb_transmit(buf, sizeof(buf));
}

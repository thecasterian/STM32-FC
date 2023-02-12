#include <string.h>
#include "fc_protocol.h"

static const uint16_t crc_lut[256] = {
    0x0000U, 0xC0C1U, 0xC181U, 0x0140U, 0xC301U, 0x03C0U, 0x0280U, 0xC241U,
    0xC601U, 0x06C0U, 0x0780U, 0xC741U, 0x0500U, 0xC5C1U, 0xC481U, 0x0440U,
    0xCC01U, 0x0CC0U, 0x0D80U, 0xCD41U, 0x0F00U, 0xCFC1U, 0xCE81U, 0x0E40U,
    0x0A00U, 0xCAC1U, 0xCB81U, 0x0B40U, 0xC901U, 0x09C0U, 0x0880U, 0xC841U,
    0xD801U, 0x18C0U, 0x1980U, 0xD941U, 0x1B00U, 0xDBC1U, 0xDA81U, 0x1A40U,
    0x1E00U, 0xDEC1U, 0xDF81U, 0x1F40U, 0xDD01U, 0x1DC0U, 0x1C80U, 0xDC41U,
    0x1400U, 0xD4C1U, 0xD581U, 0x1540U, 0xD701U, 0x17C0U, 0x1680U, 0xD641U,
    0xD201U, 0x12C0U, 0x1380U, 0xD341U, 0x1100U, 0xD1C1U, 0xD081U, 0x1040U,
    0xF001U, 0x30C0U, 0x3180U, 0xF141U, 0x3300U, 0xF3C1U, 0xF281U, 0x3240U,
    0x3600U, 0xF6C1U, 0xF781U, 0x3740U, 0xF501U, 0x35C0U, 0x3480U, 0xF441U,
    0x3C00U, 0xFCC1U, 0xFD81U, 0x3D40U, 0xFF01U, 0x3FC0U, 0x3E80U, 0xFE41U,
    0xFA01U, 0x3AC0U, 0x3B80U, 0xFB41U, 0x3900U, 0xF9C1U, 0xF881U, 0x3840U,
    0x2800U, 0xE8C1U, 0xE981U, 0x2940U, 0xEB01U, 0x2BC0U, 0x2A80U, 0xEA41U,
    0xEE01U, 0x2EC0U, 0x2F80U, 0xEF41U, 0x2D00U, 0xEDC1U, 0xEC81U, 0x2C40U,
    0xE401U, 0x24C0U, 0x2580U, 0xE541U, 0x2700U, 0xE7C1U, 0xE681U, 0x2640U,
    0x2200U, 0xE2C1U, 0xE381U, 0x2340U, 0xE101U, 0x21C0U, 0x2080U, 0xE041U,
    0xA001U, 0x60C0U, 0x6180U, 0xA141U, 0x6300U, 0xA3C1U, 0xA281U, 0x6240U,
    0x6600U, 0xA6C1U, 0xA781U, 0x6740U, 0xA501U, 0x65C0U, 0x6480U, 0xA441U,
    0x6C00U, 0xACC1U, 0xAD81U, 0x6D40U, 0xAF01U, 0x6FC0U, 0x6E80U, 0xAE41U,
    0xAA01U, 0x6AC0U, 0x6B80U, 0xAB41U, 0x6900U, 0xA9C1U, 0xA881U, 0x6840U,
    0x7800U, 0xB8C1U, 0xB981U, 0x7940U, 0xBB01U, 0x7BC0U, 0x7A80U, 0xBA41U,
    0xBE01U, 0x7EC0U, 0x7F80U, 0xBF41U, 0x7D00U, 0xBDC1U, 0xBC81U, 0x7C40U,
    0xB401U, 0x74C0U, 0x7580U, 0xB541U, 0x7700U, 0xB7C1U, 0xB681U, 0x7640U,
    0x7200U, 0xB2C1U, 0xB381U, 0x7340U, 0xB101U, 0x71C0U, 0x7080U, 0xB041U,
    0x5000U, 0x90C1U, 0x9181U, 0x5140U, 0x9301U, 0x53C0U, 0x5280U, 0x9241U,
    0x9601U, 0x56C0U, 0x5780U, 0x9741U, 0x5500U, 0x95C1U, 0x9481U, 0x5440U,
    0x9C01U, 0x5CC0U, 0x5D80U, 0x9D41U, 0x5F00U, 0x9FC1U, 0x9E81U, 0x5E40U,
    0x5A00U, 0x9AC1U, 0x9B81U, 0x5B40U, 0x9901U, 0x59C0U, 0x5880U, 0x9841U,
    0x8801U, 0x48C0U, 0x4980U, 0x8941U, 0x4B00U, 0x8BC1U, 0x8A81U, 0x4A40U,
    0x4E00U, 0x8EC1U, 0x8F81U, 0x4F40U, 0x8D01U, 0x4DC0U, 0x4C80U, 0x8C41U,
    0x4400U, 0x84C1U, 0x8581U, 0x4540U, 0x8701U, 0x47C0U, 0x4680U, 0x8641U,
    0x8201U, 0x42C0U, 0x4380U, 0x8341U, 0x4100U, 0x81C1U, 0x8081U, 0x4040U,
};

static uint16_t fc_packet_crc(uint16_t len, const uint8_t *dat);

void fc_protocol_channel_init(fc_protocol_channel_t *prot_ch, channel_send_t send,
                              channel_receive_t receive) {
    prot_ch->send = send;
    prot_ch->receive = receive;
    prot_ch->stx = false;
    prot_ch->cnt = 0U;
}

void fc_protocol_channel_send(fc_protocol_channel_t *prot_ch, const fc_packet_t *packet) {
    uint8_t buf[FC_PACKET_LEN_MAX + 7];
    uint8_t crc;

    crc = fc_packet_crc(packet->len, packet->dat);

    buf[0] = FC_PACKET_STX;
    buf[1] = packet->typ;
    buf[2] = byte0(packet->len);
    buf[3] = byte1(packet->len);
    memcpy(&buf[4], packet->dat, packet->len);
    buf[packet->len + 4U] = byte0(crc);
    buf[packet->len + 5U] = byte1(crc);
    buf[packet->len + 6U] = FC_PACKET_ETX;

    prot_ch->send(buf, packet->len + 6U);
}

bool fc_protocol_channel_receive(fc_protocol_channel_t *prot_ch, fc_packet_t *packet) {
    uint8_t c;
    bool res;
    bool success;

    res = false;

    if (!prot_ch->stx) {
        /* Find STX. */
        while (true) {
            success = prot_ch->receive(&c, 1U);
            if (!success || (c == FC_PACKET_STX)) {
                break;
            }
        }

        if (success && (c == FC_PACKET_STX)) {
            /* STX is found. */
            prot_ch->stx = true;
            prot_ch->cnt++;
        } else {
            prot_ch->stx = false;
            prot_ch->cnt = 0U;
        }
    }

    if (prot_ch->stx) {
        while (true) {
            success = prot_ch->receive(&c, 1U);
            if (!success) {
                break;
            }

            if (prot_ch->cnt == 1U) {
                /* Byte 1 is TYP. */
                prot_ch->packet.typ = c;
            } else if (prot_ch->cnt == 2U) {
                /* Byte 2 is the lower byte of LEN. */
                prot_ch->packet.len = c;
            } else if (prot_ch->cnt == 3U) {
                /* Byte 3 is the upper byte of LEN. */
                prot_ch->packet.len |= (c << 8U);
            } else if (prot_ch->cnt < (prot_ch->packet.len + 4U)) {
                /* Bytes 4 .. (LEN + 3) are in DAT. */
                prot_ch->packet.dat[prot_ch->cnt - 4U] = c;
            } else if (prot_ch->cnt == (prot_ch->packet.len + 4U)) {
                /* Byte (LEN + 4) is the lower byte of CRC. */
                prot_ch->packet.crc = c;
            } else if (prot_ch->cnt == (prot_ch->packet.len + 5U)) {
                /* Byte (LEN + 5) is the upper byte of CRC. */
                prot_ch->packet.crc |= (c << 8U);
            } else {
                /* Last byte is ETX. */
                prot_ch->packet.etx = c;

                res = true;
                prot_ch->stx = false;
                prot_ch->cnt = 0U;

                /* Copy packet. */
                memcpy(packet, &prot_ch->packet, sizeof(*packet));

                break;
            }

            prot_ch->cnt++;
        }
    }

    return res;
}

uint8_t fc_packet_validate(fc_packet_t *packet) {
    uint8_t err;

    if (packet->etx != FC_PACKET_ETX) {
        /* ETX is missing. */
        err = FC_PACKET_ERR_ETX_MIS;
    } else if (fc_packet_crc(packet->len, packet->dat) != packet->crc) {
        /* CRC mismatch. */
        err = FC_PACKET_ERR_CRC_MIS;
    } else if (packet->typ != FC_PACKET_TYP_CMD) {
        /* Received packet must be a command packet. */
        err = FC_PACKET_ERR_TYP_INVAL;
    } else if (packet->len == 0U) {
        /* No command. */
        err = FC_PACKET_ERR_CMD_INVAL;
    } else {
        err = FC_PACKET_ERR_OK;
    }

    return err;
}

void fc_packet_create_response(fc_packet_t *packet, uint8_t err) {
    packet->typ = FC_PACKET_TYP_RESP;
    packet->len = 1U;
    packet->dat[0] = err;
}

void fc_packet_create_streaming(fc_packet_t *packet, const fc_protocol_streaming_field_t *fields, uint16_t nfields) {
    packet->typ = FC_PACKET_TYP_STRM;
    packet->len = nfields + 1U;
    packet->dat[0] = nfields;
    for (uint16_t i = 0U; i < nfields; i++) {
        packet->dat[i + 1U] = fields[i].id;
    }

    for (uint16_t i = 0U; i < nfields; i++) {
        memcpy(&packet->dat[packet->len], fields[i].data, fields[i].size);
        packet->len += fields[i].size;
    }
}

static uint16_t fc_packet_crc(uint16_t len, const uint8_t *dat) {
    uint16_t crc;

    /* Calculate CRC-16/MODBUS. */
    crc = 0xFFFFU;
    for (uint16_t i = 0U; i < len; i++) {
        crc = (crc >> 8U) ^ crc_lut[(crc ^ dat[i]) & 0xFFU];
    }

    return crc;
}

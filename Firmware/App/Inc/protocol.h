#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

#define STX 0x02
#define ETX 0x03

#define ACK  0x06
#define NACK 0x15

#define TYP_CMD  0x90
#define TYP_RESP 0x91
#define TYP_STRM 0x92

#define CMD_START 0xA0
#define CMD_END   0xA5

#define CMD_LED_RED   0xA0
#define CMD_LED_GREEN 0xA1
#define CMD_LED_BLUE  0xA2
#define CMD_STRM_DAT  0xA3
#define CMD_STRM      0xA4

#define ERR_OK        0xE0
#define ERR_ETX_MIS   0xE1
#define ERR_CKS_MIS   0xE2
#define ERR_TYP_INVAL 0xE3
#define ERR_CMD_INVAL 0xE4
#define ERR_LEN_INVAL 0xE5
#define ERR_ARG_INVAL 0xE6

#define DAT_START 0x00
#define DAT_END   0x1A

#define DAT_ACC          0x00
#define DAT_ANG          0x01
#define DAT_MAG          0x02
#define DAT_RAW_ACC      0x08
#define DAT_RAW_GYRO     0x09
#define DAT_RAW_MAG      0x0A
#define DAT_KF_QUAT      0x10
#define DAT_KF_RPY       0x11
#define DAT_ACC_MAG_QUAT 0x18
#define DAT_ACC_MAG_RPY  0x19

#define LEN_MAX 255

typedef struct {
    uint8_t typ;                                /* Packet type. */
    uint8_t len;                                /* Data length. */
    uint8_t dat[LEN_MAX];              /* Data. */
    uint8_t cks;                                /* Data checksum. */
    uint8_t etx;                                /* ETX. */
} packet_t;

void packet_parser_init(void);
bool packet_receive(packet_t *packet);
uint8_t packet_validate(packet_t *packet);

uint8_t packet_checksum_calc(const uint8_t *dat, uint8_t len);

void response_send(uint8_t err);

#endif

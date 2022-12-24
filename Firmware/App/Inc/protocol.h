/**
 * @file protocol.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Protocol definition and packet handlers.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

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
#define CMD_END   0xD3

#define CMD_STRM_DAT  0xA0
#define CMD_STRM      0xA1
#define CMD_ESC_PRTCL 0xA2
#define CMD_PWM       0xA3
#define CMD_THROT     0xA4
#define CMD_LED_RED   0xD0
#define CMD_LED_GREEN 0xD1
#define CMD_LED_BLUE  0xD2

#define ERR_OK        0xE0
#define ERR_ETX_MIS   0xE1
#define ERR_CKS_MIS   0xE2
#define ERR_TYP_INVAL 0xE3
#define ERR_CMD_INVAL 0xE4
#define ERR_LEN_INVAL 0xE5
#define ERR_ARG_INVAL 0xE6

#define DAT_START 0x10
#define DAT_END   0x2A

#define DAT_ACC          0x10
#define DAT_ANG          0x11
#define DAT_MAG          0x12
#define DAT_RAW_ACC      0x18
#define DAT_RAW_GYRO     0x19
#define DAT_RAW_MAG      0x1A
#define DAT_KF_QUAT      0x20
#define DAT_KF_RPY       0x21
#define DAT_ACC_MAG_QUAT 0x28
#define DAT_ACC_MAG_RPY  0x29

#define LEN_MAX 255

typedef struct {
    uint8_t typ;                                /* Packet type. */
    uint8_t len;                                /* Data length. */
    uint8_t dat[LEN_MAX];                       /* Data. */
    uint8_t cks;                                /* Data checksum. */
    uint8_t etx;                                /* ETX. */
} packet_t;

/**
 * @brief Initializes the packet parser.
 */
void packet_parser_init(void);

/**
 * @brief Receives a packet.
 *
 * @param packet Pointer to the packet to be received.
 * @return true if a packet was received, false otherwise.
 */
bool packet_receive(packet_t *packet);

/**
 * @brief Validates a packet.
 *
 * @param packet Pointer to the packet to validate.
 * @return Error code.
 */
uint8_t packet_validate(packet_t *packet);

/**
 * @brief Calculates the checksum defined in the packet.
 *
 * @param dat DAT field of the packet.
 * @param len Length of the DAT field.
 * @return Checksum.
 */
uint8_t packet_checksum_calc(const uint8_t dat[], uint8_t len);

/**
 * @brief Sends a response packet.
 *
 * @param err Error code returned from the latest received command packet.
 */
void response_send(uint8_t err);

#endif

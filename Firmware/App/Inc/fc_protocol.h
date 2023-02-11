/**
 * @file fc_protocol.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief STM32-FC protocol handler.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef FC_PROTOCOL_H
#define FC_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

#define FC_PACKET_STX 0x02
#define FC_PACKET_ETX 0x03

#define FC_PACKET_TYP_CMD  0x90
#define FC_PACKET_TYP_RESP 0x91
#define FC_PACKET_TYP_STRM 0x92

#define FC_PACKET_CMD_START 0xA0
#define FC_PACKET_CMD_END   0xA2

#define FC_PACKET_CMD_PWM       0xA0
#define FC_PACKET_CMD_THROT     0xA1

#define FC_PACKET_ERR_OK        0xE0
#define FC_PACKET_ERR_ETX_MIS   0xE1
#define FC_PACKET_ERR_CRC_MIS   0xE2
#define FC_PACKET_ERR_TYP_INVAL 0xE3
#define FC_PACKET_ERR_CMD_INVAL 0xE4
#define FC_PACKET_ERR_LEN_INVAL 0xE5
#define FC_PACKET_ERR_ARG_INVAL 0xE6

#define FC_PACKET_DAT_START 0x00
#define FC_PACKET_DAT_END   0x41

#define FC_PACKET_DAT_MODE      0x00
#define FC_PACKET_DAT_ACC       0x10
#define FC_PACKET_DAT_ANG       0x11
#define FC_PACKET_DAT_MAG       0x12
#define FC_PACKET_DAT_RAW_ACC   0x18
#define FC_PACKET_DAT_RAW_ANG   0x19
#define FC_PACKET_DAT_RAW_MAG   0x1A
#define FC_PACKET_DAT_KF_QUAT   0x20
#define FC_PACKET_DAT_KF_RPY    0x21
#define FC_PACKET_DAT_MEAS_QUAT 0x28
#define FC_PACKET_DAT_MEAS_RPY  0x29
#define FC_PACKET_DAT_COV_STATE 0x30
#define FC_PACKET_DAT_COV_PROC  0x31
#define FC_PACKET_DAT_RF_CH     0x40

#define FC_MODE_INIT       0x00
#define FC_MODE_STANDBY    0x01
#define FC_MODE_FLIGHT     0x02
#define FC_MODE_MOTOR_TEST 0x10
#define FC_MODE_EMER       0xFF

#define FC_PACKET_LEN_MAX 511

/**
 * @brief STM32-FC protocol packet.
 */
typedef struct {
    /** Packet type. */
    uint8_t typ;
    /** Data length. */
    uint16_t len;
    /** Data. */
    uint8_t dat[FC_PACKET_LEN_MAX];
    /** Data CRC. */
    uint16_t crc;
    /** ETX. */
    uint8_t etx;
} fc_packet_t;

/**
 * @brief Function to send bytes to the STM32-FC protocol channel.
 *
 * @param buf Buffer for bytes to send.
 * @param size Required size to send.
 */
typedef void (*fc_protocol_channel_send_t)(uint8_t *buf, uint16_t size);

/**
 * @brief Function to receive bytes from the STM32-FC protocol channel.
 *
 * @param buf Buffer for received bytes.
 * @param size Required size to receive.
 *
 * @return true if the required size of bytes are received, false otherwise.
 */
typedef bool (*fc_protocol_channel_receive_t)(uint8_t *buf, uint16_t size);

/**
 * @brief Communication channel for the STM32-FC protocol.
 */
typedef struct {
    /** Function to send bytes. */
    fc_protocol_channel_send_t send;
    /** Function to receive bytes. */
    fc_protocol_channel_receive_t receive;

    /** Is STX found during parsing? */
    bool stx;
    /** Number of received bytes during parsing. */
    uint16_t cnt;
    /** Buffer for the receiving packet. */
    fc_packet_t packet;
} fc_protocol_channel_t;

/**
 * @brief Field of a streaming packet.
 */
typedef struct {
    /** Field ID. */
    uint8_t id;
    /** Field size. */
    uint16_t size;
    /** Pointer to data. */
    void *data;
} fc_protocol_streaming_field_t;

/**
 * @brief Initializes the channel.
 *
 * @param prot_ch Parser for the STM32-FC protocol packet.
 * @param send Function to send bytes.
 * @param receive Function to receive bytes.
 */
void fc_protocol_channel_init(fc_protocol_channel_t *prot_ch, fc_protocol_channel_send_t send,
                              fc_protocol_channel_receive_t receive);

/**
 * @brief Sends a packet to the channel.
 * @details Only TYP, LEN, and DAT fields are used in this function.
 *
 * @param prot_ch STM32-FC protocol channel.
 * @param packet Pointer to the packet to send.
 */
void fc_protocol_channel_send(fc_protocol_channel_t *prot_ch, const fc_packet_t *packet);

/**
 * @brief Receives a packet from the channel.
 *
 * @param prot_ch STM32-FC protocol channel.
 * @param packet Pointer to the packet to be received.
 * @return true if a packet was received, false otherwise.
 */
bool fc_protocol_channel_receive(fc_protocol_channel_t *prot_ch, fc_packet_t *packet);

/**
 * @brief Validates a packet.
 *
 * @param packet Pointer to the packet to validate.
 * @return Error code.
 */
uint8_t fc_packet_validate(fc_packet_t *packet);

/**
 * @brief Creates a response packet.
 *
 * @param packet Pointer to the response packet to create.
 * @param err Error code returned from the latest received packet.
 */
void fc_packet_create_response(fc_packet_t *packet, uint8_t err);

/**
 * @brief Creates a streaming packet.
 *
 * @param packet Pointer to the streaming packet to create.
 * @param fields Pointer to the array of fields.
 * @param nfields Number of fields.
 */
void fc_packet_create_streaming(fc_packet_t *packet, fc_protocol_streaming_field_t *fields, uint16_t nfields);

#endif

/**
 * @file sbus.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief SBUS protocol handler.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef SBUS_H
#define SBUS_H

#include "util.h"

#define SBUS_CH_NUM 16U

#define SBUS_THROTTLE_CH 0
#define SBUS_AILERON_CH  1
#define SBUS_ELEVATOR_CH 2
#define SBUS_RUDDER_CH   3
#define SBUS_S1_CH       4
#define SBUS_S2_CH       5
#define SBUS_SA_CH       6
#define SBUS_SB_CH       7
#define SBUS_SC_CH       8
#define SBUS_SD_CH       9
#define SBUS_SF_CH       10
#define SBUS_SH_CH       11

#define SBUS_CH_DAT_LEN 22U

typedef struct {
    uint16_t ch[SBUS_CH_NUM];
    uint8_t flg;
} sbus_packet_t;

typedef struct {
    /** Function to receive bytes. */
    channel_receive_t receive;

    /** Is STX found during parsing? */
    bool stx;
    /* Number of received bytes during parsing. */
    uint16_t cnt;
    /* Received channel data. */
    uint8_t ch_dat[SBUS_CH_DAT_LEN];
    /* Buffer for the receiving packet. */
    sbus_packet_t packet;

    /* Tick when the last packet is received. */
    uint32_t tick;
} sbus_channel_t;

/**
 * @brief Initializes the SBUS channel.
 *
 * @param sbus_ch SBUS channel.
 * @param receive Function to receive bytes.
 */
void sbus_channel_init(sbus_channel_t *sbus_ch, channel_receive_t receive);

/**
 * @brief Receives a SBUS packet.
 *
 * @param sbus_ch SBUS channel.
 * @param packet SBUS packet.
 * @return true if a packet is received, false otherwise.
 */
bool sbus_channel_receive(sbus_channel_t *sbus_ch, sbus_packet_t *packet);

/**
 * @brief Checks if the channel is timed out (no packet comes in for 100 ticks).
 *
 * @param sbus_ch SBUS channel.
 * @return true if the channel is timed out, false otherwise.
 */
bool sbus_channel_is_timeout(sbus_channel_t *sbus_ch);

/**
 * @brief Checks if the packet is in arming position.
 *
 * @param packet SBUS packet.
 * @return true if the packet is in arming position, false otherwise.
 */
bool sbus_packet_is_arming_pos(const sbus_packet_t *packet);

/**
 * @brief Checks if the packet is in disarming position.
 *
 * @param packet SBUS packet.
 * @return true if the packet is in disarming position, false otherwise.
 */
bool sbus_packet_is_disarming_pos(const sbus_packet_t *packet);

/**
 * @brief Checks if the packet is in emergency switch on.
 *
 * @param packet SBUS packet.
 * @return true if the packet is in emergency switch on, false otherwise.
 */
bool sbus_packet_is_emergency_switch_on(const sbus_packet_t *packet);

/**
 * @brief Maps the SBUS channel value to the range [min, max].
 *
 * @param ch_val SBUS channel value.
 * @param min Minimum value of the range.
 * @param max Maximum value of the range.
 * @return Mapped value.
 */
float sbus_packet_map_range(uint16_t ch_val, float min, float max);

#endif

#ifndef SBUS_H
#define SBUS_H

#include <stdbool.h>
#include <stdint.h>

#define SBUS_CH_NUM 16U

#define SBUS_MIN 172
#define SBUS_MAX 1811

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

#define SBUS_TIMEOUT_TICK   100U
#define SBUS_ARMING_TICK    2000U
#define SBUS_DISARMING_TICK 2000U

typedef struct {
    uint16_t ch[SBUS_CH_NUM];
    uint8_t flg;
} sbus_packet_t;

void sbus_init(void);

bool sbus_packet_receive(sbus_packet_t *packet);

bool sbus_is_timeout(void);
bool sbus_packet_is_arming_pos(const sbus_packet_t *packet);
bool sbus_packet_is_disarming_pos(const sbus_packet_t *packet);
bool sbus_packet_is_emergency_switch_on(const sbus_packet_t *packet);

float sbus_ch_map_range(uint16_t ch_val, float min, float max);

#endif

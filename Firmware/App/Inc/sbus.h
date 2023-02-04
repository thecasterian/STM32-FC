#ifndef SBUS_H
#define SBUS_H

#include <inttypes.h>
#include <stdbool.h>

#define SBUS_CH_NUM 16U

#define SBUS_MIN 172.f
#define SBUS_MAX 1811.f

typedef struct {
    uint16_t ch[SBUS_CH_NUM];
    uint8_t flg;
} sbus_packet_t;

void sbus_init(void);

bool sbus_packet_receive(sbus_packet_t *packet);

#endif

/**
 * @file streaming.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Data streaming.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef STREAMING_DATA_H
#define STREAMING_DATA_H

#include <stdbool.h>
#include "protocol.h"
#include "quaternion.h"

typedef struct {
    float *dat;
    uint8_t size;
} strm_dat_t;

extern float acc[3], ang[3], mag[3];
extern float acc_raw[3], ang_raw[3], mag_raw[3];
extern quaternion_t q;
extern float rpy[3];
extern quaternion_t q_meas;
extern float rpy_meas[3];

/* Is streaming enabled? */
extern bool strm_en;
/* Is streaming data selected? */
extern bool strm_dat_sel[DAT_END - DAT_START];

extern const strm_dat_t strm_dat_list[DAT_END - DAT_START];

void streaming_send(void);

#endif

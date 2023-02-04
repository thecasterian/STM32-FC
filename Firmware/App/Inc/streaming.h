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
extern float q[4], rpy[3];
extern float q_meas[4], rpy_meas[3];
extern float P[16], Q[16], R[36];
extern float acc_hpf[3];
extern float rf_ch[16];

extern const strm_dat_t strm_dat_list[DAT_END - DAT_START];

/**
 * @brief Starts the streaming.
 */
void streaming_start(void);

/**
 * @brief Stops the streaming.
 */
void streaming_stop(void);

/**
 * @brief Selects the streaming data.
 *
 * @param[in] data_id Array of data IDs.
 * @param num_data Number of data.
 */
void streaming_select_data(const uint8_t data_id[], uint16_t num_data);

/**
 * @brief Sends the streaming data if the streaming is enabled.
 */
void streaming_send(void);

#endif

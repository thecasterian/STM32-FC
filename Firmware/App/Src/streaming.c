#include <string.h>
#include "streaming.h"
#include "usb_wrapper.h"

#define STRM_DAT_LIST_INIT(ID, DAT, SIZE) [ID - DAT_START] = { .dat = DAT, .size = SIZE }

float acc[3], ang[3], mag[3];
float acc_raw[3], ang_raw[3], mag_raw[3];
float q[4], rpy[3];
float q_meas[4], rpy_meas[3];
float P[16], Q[16], R[36];
float acc_hpf[3];
float rf_ch[16];

/* List of streaming data. */
const strm_dat_t strm_dat_list[DAT_END - DAT_START] = {
    STRM_DAT_LIST_INIT( DAT_ACC,       acc,       12U ),
    STRM_DAT_LIST_INIT( DAT_ANG,       ang,       12U ),
    STRM_DAT_LIST_INIT( DAT_MAG,       mag,       12U ),
    STRM_DAT_LIST_INIT( DAT_RAW_ACC,   acc_raw,   12U ),
    STRM_DAT_LIST_INIT( DAT_RAW_GYRO,  ang_raw,   12U ),
    STRM_DAT_LIST_INIT( DAT_RAW_MAG,   mag_raw,   12U ),
    STRM_DAT_LIST_INIT( DAT_KF_QUAT,   q,         16U ),
    STRM_DAT_LIST_INIT( DAT_KF_RPY,    rpy,       12U ),
    STRM_DAT_LIST_INIT( DAT_MEAS_QUAT, q_meas,    16U ),
    STRM_DAT_LIST_INIT( DAT_MEAS_RPY,  rpy_meas,  12U ),
    STRM_DAT_LIST_INIT( DAT_COV_STATE, P,         64U ),
    STRM_DAT_LIST_INIT( DAT_COV_PROC,  Q,         64U ),
    STRM_DAT_LIST_INIT( DAT_COV_MEAS,  R,        144U ),
    STRM_DAT_LIST_INIT( DAT_HPF_ACC,   acc_hpf,   12U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH1,    &rf_ch[0],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH2,    &rf_ch[1],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH3,    &rf_ch[2],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH4,    &rf_ch[3],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH5,    &rf_ch[4],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH6,    &rf_ch[5],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH7,    &rf_ch[6],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH8,    &rf_ch[7],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH9,    &rf_ch[8],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH10,   &rf_ch[9],  4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH11,   &rf_ch[10], 4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH12,   &rf_ch[11], 4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH13,   &rf_ch[12], 4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH14,   &rf_ch[13], 4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH15,   &rf_ch[14], 4U ),
    STRM_DAT_LIST_INIT( DAT_RF_CH16,   &rf_ch[15], 4U ),
};

/* Is streaming enabled? */
static bool strm_en;
/* Is streaming data selected? */
static bool strm_dat_sel[DAT_END - DAT_START];

void streaming_start(void) {
    strm_en = true;
}

void streaming_stop(void) {
    strm_en = false;
}

void streaming_select_data(const uint8_t data_id[], uint16_t num_data) {
    for (uint16_t i = 0U; i < DAT_END - DAT_START; i++) {
        strm_dat_sel[i] = false;
    }

    for (uint16_t i = 0U; i < num_data; i++) {
        strm_dat_sel[data_id[i] - DAT_START] = true;
    }
}

void streaming_send(void) {
    uint8_t buf[LEN_MAX + 5];

    if (strm_en) {
        buf[0] = STX;
        buf[1] = TYP_STRM;

        buf[2] = 0U;
        for (int16_t i = 0; i < DAT_END - DAT_START; i++) {
            if (strm_dat_sel[i]) {
                memcpy(&buf[buf[2] + 3U], strm_dat_list[i].dat, strm_dat_list[i].size);
                buf[2] += strm_dat_list[i].size;
            }
        }

        buf[buf[2] + 3U] = packet_checksum_calc(&buf[3], buf[2]);
        buf[buf[2] + 4U] = ETX;

        usb_transmit(buf, buf[2] + 5U);
    }
}

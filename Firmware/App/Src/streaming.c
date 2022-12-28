#include <string.h>
#include "streaming.h"
#include "usb_wrapper.h"

#define STRM_DAT_LIST_INIT(ID, DAT, SIZE) [ID - DAT_START] = { .dat = DAT, .size = SIZE }

float acc[3], ang[3], mag[3];
float acc_raw[3], ang_raw[3], mag_raw[3];
quaternion_t q;
float rpy[3];
quaternion_t q_meas;
float rpy_meas[3];

/* Is streaming enabled? */
bool strm_en;
/* Is streaming data selected? */
bool strm_dat_sel[DAT_END - DAT_START];

/* List of streaming data. */
const strm_dat_t strm_dat_list[DAT_END - DAT_START] = {
    STRM_DAT_LIST_INIT( DAT_ACC,       acc,       12U ),
    STRM_DAT_LIST_INIT( DAT_ANG,       ang,       12U ),
    STRM_DAT_LIST_INIT( DAT_MAG,       mag,       12U ),
    STRM_DAT_LIST_INIT( DAT_RAW_ACC,   acc_raw,   12U ),
    STRM_DAT_LIST_INIT( DAT_RAW_GYRO,  ang_raw,   12U ),
    STRM_DAT_LIST_INIT( DAT_RAW_MAG,   mag_raw,   12U ),
    STRM_DAT_LIST_INIT( DAT_KF_QUAT,   &q.w,      16U ),
    STRM_DAT_LIST_INIT( DAT_KF_RPY,    rpy,       12U ),
    STRM_DAT_LIST_INIT( DAT_MEAS_QUAT, &q_meas.w, 16U ),
    STRM_DAT_LIST_INIT( DAT_MEAS_RPY,  rpy_meas,  12U ),
};

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

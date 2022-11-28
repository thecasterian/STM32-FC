#ifndef STREAMING_DATA_H
#define STREAMING_DATA_H

#include "quaternion.h"

extern float acc[3], ang[3], mag[3], pres, temp;
extern float acc_raw[3], ang_raw[3], mag_raw[3];
extern quaternion_t q;
extern float rpy[3], vel[3], pos[3], acc_ext[3];
extern quaternion_t q_acc_mag;
extern float rpy_acc_mag[3], baro_height;

#endif

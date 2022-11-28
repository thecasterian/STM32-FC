#include "streaming_data.h"

float acc[3], ang[3], mag[3], pres, temp;
float acc_raw[3], ang_raw[3], mag_raw[3];
quaternion_t q;
float rpy[3], vel[3], pos[3], acc_ext[3];
quaternion_t q_acc_mag;
float rpy_acc_mag[3], baro_height;

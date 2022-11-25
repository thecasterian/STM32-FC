#ifndef CALIB_H
#define CALIB_H

void calib_acc(const float *raw_acc, float *acc);
void calib_gyro(const float *raw_gyro, float *gyro);
void calib_mag(const float *raw_mag, float *mag);

#endif

#include <math.h>
#include <stdint.h>
#include "calib.h"
#include "config.h"

static const float magcal_a[3][3] = {
    {MAG_SOFT_IRON_XX, MAG_SOFT_IRON_XY, MAG_SOFT_IRON_XZ},
    {MAG_SOFT_IRON_XY, MAG_SOFT_IRON_YY, MAG_SOFT_IRON_YZ},
    {MAG_SOFT_IRON_XZ, MAG_SOFT_IRON_YZ, MAG_SOFT_IRON_ZZ},
};
static const float magcal_b[3] = {
    MAG_HARD_IRON_X,
    MAG_HARD_IRON_Y,
    MAG_HARD_IRON_Z,
};

void calib_acc(const float acc_raw[3], float acc[3]) {
    acc[0] = ACC_SCALE * (acc_raw[0] - ACC_BIAS_X);
    acc[1] = ACC_SCALE * (acc_raw[1] - ACC_BIAS_Y);
    acc[2] = ACC_SCALE * (acc_raw[2] - ACC_BIAS_Z);
}

void calib_gyro(const float ang_raw[3], float ang[3]) {
    ang[0] = ang_raw[0] - GYRO_BIAS_X;
    ang[1] = ang_raw[1] - GYRO_BIAS_Y;
    ang[2] = ang_raw[2] - GYRO_BIAS_Z;
}

void calib_mag(const float mag_raw[3], float mag[3]) {
    for (int16_t i = 0; i < 3; i++) {
        mag[i] = 0.0f;
        for (int16_t j = 0; j < 3; j++) {
            mag[i] += magcal_a[i][j] * (mag_raw[j] - magcal_b[j]);
        }
    }
}

#include <math.h>
#include <stdint.h>
#include "calib.h"
#include "config.h"

static const float gyro_bias[3] = {
    GYRO_BIAS_X,
    GYRO_BIAS_Y,
    GYRO_BIAS_Z,
};
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

void calib_acc(const float *raw_acc, float *acc) {
    /* No calibration is done yet. */
    acc[0] = raw_acc[0];
    acc[1] = raw_acc[1];
    acc[2] = raw_acc[2];
}

void calib_gyro(const float *raw_gyro, float *gyro) {
    static float gyro_prev[3], integrator[3];

    /* Heuristic drift reduction. */
    for (int16_t i = 0; i < 3; i++) {
        if (gyro_prev[i] > 0.f) {
            integrator[i] -= GYRO_HDR_INC;
        } else if (gyro_prev[i] < 0.f) {
            integrator[i] += GYRO_HDR_INC;
        } else {}

        gyro[i] = raw_gyro[i] - gyro_bias[i] + integrator[i];
        gyro_prev[i] = gyro[i];
    }
}

void calib_mag(const float *raw_mag, float *mag) {
    for (int16_t i = 0; i < 3; i++) {
        mag[i] = 0.0f;
        for (int16_t j = 0; j < 3; j++) {
            mag[i] += magcal_a[i][j] * (raw_mag[j] - magcal_b[j]);
        }
    }
}

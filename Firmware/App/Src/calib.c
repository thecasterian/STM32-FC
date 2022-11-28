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

void calib_acc(const float acc_raw[3], float acc[3]) {
    /* No calibration is done yet. */
    acc[0] = acc_raw[0];
    acc[1] = acc_raw[1];
    acc[2] = acc_raw[2];
}

void calib_gyro(const float ang_raw[3], float ang[3]) {
    static float ang_prev[3], integrator[3];

    /* Heuristic drift reduction. */
    for (int16_t i = 0; i < 3; i++) {
        if (ang_prev[i] > 0.f) {
            integrator[i] -= GYRO_HDR_INC;
        } else if (ang_prev[i] < 0.f) {
            integrator[i] += GYRO_HDR_INC;
        } else {}

        ang[i] = ang_raw[i] - gyro_bias[i] + integrator[i];
        ang_prev[i] = ang[i];
    }
}

void calib_mag(const float mag_raw[3], float mag[3]) {
    for (int16_t i = 0; i < 3; i++) {
        mag[i] = 0.0f;
        for (int16_t j = 0; j < 3; j++) {
            mag[i] += magcal_a[i][j] * (mag_raw[j] - magcal_b[j]);
        }
    }
}

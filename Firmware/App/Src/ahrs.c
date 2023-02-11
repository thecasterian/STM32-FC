#include <math.h>
#include <string.h>
#include "ahrs.h"
#include "const.h"
#include "matrix.h"
#include "tim_wrapper.h"
#include "quaternion.h"
#include "util.h"

#define qw (q_est[0])
#define qx (q_est[1])
#define qy (q_est[2])
#define qz (q_est[3])

/* Inclination angle of Earth's magnetic field. */
static float alpha;
/* Sine and cosine of the inclination angle. */
static float sa, ca;

static void ahrs_run_kalman_filter(ahrs_t *ahrs, const float acc[3], const float ang[3], const float mag[3]);

static void measurement_to_attitude(const float acc[3], const float mag[3], float rpy[3]);
static void attitude_to_measurement(const float q_est[4], float z[6]);
static void attitude_to_measurement_jacobian(const float q_est[4], float H[24]);

void ahrs_init(ahrs_t *ahrs, float dt, void *sensor, ahrs_measure_t measure) {
    ahrs->dt = dt;
    ahrs->sensor = sensor;
    ahrs->measure = measure;
}

void ahrs_set_stddev(ahrs_t *ahrs, float init_att_stddev, float norm_acc_stddev, float ang_stddev,
                     float norm_mag_stddev) {
    matrix_zero(ahrs->P, 4U, 4U);
    matrix_zero(ahrs->Q, 4U, 4U);
    matrix_zero(ahrs->R, 6U, 6U);

    /* Initial state covariance matrix. */
    ahrs->P[0] = init_att_stddev * init_att_stddev;
    ahrs->P[5] = init_att_stddev * init_att_stddev;
    ahrs->P[10] = init_att_stddev * init_att_stddev;
    ahrs->P[15] = init_att_stddev * init_att_stddev;

    /* Measurement noise covariance matrix. */
    ahrs->R[0] = norm_acc_stddev * norm_acc_stddev;
    ahrs->R[7] = norm_acc_stddev * norm_acc_stddev;
    ahrs->R[14] = norm_acc_stddev * norm_acc_stddev;
    ahrs->R[21] = norm_mag_stddev * norm_mag_stddev;
    ahrs->R[28] = norm_mag_stddev * norm_mag_stddev;
    ahrs->R[35] = norm_mag_stddev * norm_mag_stddev;

    ahrs->ang_stddev = ang_stddev;
}

void ahrs_init_attitude(ahrs_t *ahrs, uint32_t num_sample) {
    float acc[3], ang[3], mag[3];
    float acc_sum[3] = { 0.f, 0.f, 0.f }, mag_sum[3] = { 0.f, 0.f, 0.f };
    uint32_t cnt;

    cnt = 0U;
    while (cnt < num_sample) {
        if (control_timer_get_flag()) {
            ahrs->measure(ahrs->sensor, acc, ang, mag);

            for (int16_t i = 0; i < 3; i++) {
                acc_sum[i] += acc[i];
                mag_sum[i] += mag[i];
            }

            cnt++;
        }

        control_timer_clear_flag();
    }

    /* Initial values of acceleration and magnetic field. */
    for (int16_t i = 0; i < 3; i++) {
        acc[i] = acc_sum[i] / num_sample;
        mag[i] = mag_sum[i] / num_sample;
    }

    /* Initialize the quaternion. */
    measurement_to_attitude(acc, mag, ahrs->rpy);

    /* Initialize the quaternion and RPY. */
    quaternion_from_euler(ahrs->q, ahrs->rpy);
    memcpy(ahrs->q_meas, ahrs->q, sizeof(ahrs->q_meas));
    memcpy(ahrs->rpy_meas, ahrs->rpy, sizeof(ahrs->rpy_meas));
}

void ahrs_update(ahrs_t *ahrs) {
    float acc[3], ang[3], mag[3];

    /* Measure. */
    ahrs->measure(ahrs->sensor, acc, ang, mag);

    measurement_to_attitude(acc, mag, ahrs->rpy_meas);
    quaternion_from_euler(ahrs->q_meas, ahrs->rpy_meas);

    /* Kalman filter. */
    ahrs_run_kalman_filter(ahrs, acc, ang, mag);
}

static void ahrs_run_kalman_filter(ahrs_t *ahrs, const float acc[3], const float ang[3], const float mag[3]) {
    const float dt2 = ahrs->dt / 2.f;

    const float A[16] = {
                  1.f,  ang[0] * dt2,  ang[1] * dt2,  ang[2] * dt2,
        -ang[0] * dt2,           1.f,  ang[2] * dt2, -ang[1] * dt2,
        -ang[1] * dt2, -ang[2] * dt2,           1.f,  ang[0] * dt2,
        -ang[2] * dt2,  ang[1] * dt2, -ang[0] * dt2,           1.f
    };                                          /* 4 x 4. */
    float A_T[16];                              /* 4 x 4. */

    const float J_f[12] = {
         ahrs->q[1] * dt2,  ahrs->q[2] * dt2,  ahrs->q[3] * dt2,
        -ahrs->q[0] * dt2, -ahrs->q[3] * dt2,  ahrs->q[2] * dt2,
         ahrs->q[3] * dt2, -ahrs->q[0] * dt2, -ahrs->q[1] * dt2,
        -ahrs->q[2] * dt2,  ahrs->q[1] * dt2, -ahrs->q[0] * dt2
    };                                          /* 4 x 3. */
    float J_f_T[12];                            /* 3 x 4. */

    float q_est[4];                             /* 4 x 1. */
    float P_est[16];                            /* 4 x 4. */

    const float acc_norm = norm3(acc[0], acc[1], acc[2]), mag_norm = norm3(mag[0], mag[1], mag[2]);

    const float z[6] = {
        acc[0] / acc_norm,
        acc[1] / acc_norm,
        acc[2] / acc_norm,
        mag[0] / mag_norm,
        mag[1] / mag_norm,
        mag[2] / mag_norm,
    };                                          /* 6 x 1. */
    float z_est[6];                             /* 6 x 1. */

    float H[24];                                /* 6 x 4. */
    float H_T[24];                              /* 4 x 6. */

    float K[24];                                /* 4 x 6. */

    float tmp[64];

    matrix_transpose(A, 4, 4, A_T);

    /* Calculate the process noise covariance matrix. */
    matrix_transpose(J_f, 4, 3, J_f_T);
    matrix_mul(J_f, J_f_T, 4, 3, 4, ahrs->Q);
    matrix_mul_scalar(ahrs->Q, ahrs->ang_stddev * ahrs->ang_stddev, 4, 4, ahrs->Q);

    /* Estimate. */
    matrix_mul(A, ahrs->q, 4, 4, 1, q_est);
    matrix_mul(A, ahrs->P, 4, 4, 4, tmp);
    matrix_mul(tmp, A_T, 4, 4, 4, tmp);
    matrix_add(tmp, ahrs->Q, 4, 4, P_est);

    attitude_to_measurement(q_est, z_est);
    attitude_to_measurement_jacobian(q_est, H);
    matrix_transpose(H, 6, 4, H_T);

    /* Correct. */
    matrix_mul(H, P_est, 6, 4, 4, tmp);
    matrix_mul(tmp, H_T, 6, 4, 6, tmp);
    matrix_add(tmp, ahrs->R, 6, 6, tmp);
    matrix_inv_sym(tmp, 6, tmp);
    matrix_mul(H_T, tmp, 4, 6, 6, tmp);
    matrix_mul(P_est, tmp, 4, 4, 6, K);
    matrix_sub(z, z_est, 6, 1, tmp);
    matrix_mul(K, tmp, 4, 6, 1, tmp);
    matrix_add(q_est, tmp, 4, 1, ahrs->q);
    matrix_mul(K, H, 4, 6, 4, tmp);
    matrix_mul_scalar(tmp, -1.f, 4, 4, tmp);
    matrix_add_eye(tmp, 1.f, 4, tmp);
    matrix_mul(tmp, P_est, 4, 4, 4, ahrs->P);

    quaternion_normalize(ahrs->q, ahrs->q);
    quaternion_to_euler(ahrs->q, ahrs->rpy);
}

static void measurement_to_attitude(const float acc[3], const float mag[3], float rpy[3]) {
    float q_acc[4], mag_acc[3];

    rpy[0] = atan2f(-acc[1], -acc[2]);
    rpy[1] = atan2f(acc[0], norm2(acc[1], acc[2]));
    rpy[2] = 0.f;
    quaternion_from_euler(q_acc, rpy);

    quaternion_rot_vec_inv(q_acc, mag, mag_acc);
    rpy[2] = atan2f(-mag_acc[1], mag_acc[0]);

    alpha = atan2f(mag_acc[2], norm2(mag_acc[0], mag_acc[1]));
    sa = sinf(alpha);
    ca = cosf(alpha);
}

static void attitude_to_measurement(const float q_est[4], float z[6]) {
    z[0] = -2.f * (qw * qy + qx * qz);
    z[1] = 2.f * (qw * qx - qy * qz);
    z[2] = 2.f * qx * qx + 2.f * qy * qy - 1.f;
    z[3] = (2.f * qw * qw + 2.f * qx * qx - 1.f) * ca + 2.f * (qw * qy + qx * qz) * sa;
    z[4] = 2.f * (qw * qz + qx * qy) * ca + 2.f * (qy * qz - qw * qx) * sa;
    z[5] = 2.f * (qx * qz - qw * qy) * ca + (2.f * qw * qw + 2.f * qz * qz - 1.f) * sa;
}

static void attitude_to_measurement_jacobian(const float q_est[4], float H[24]) {
    H[ 0] = -2.f * qy;
    H[ 1] = -2.f * qz;
    H[ 2] = -2.f * qw;
    H[ 3] = -2.f * qx;

    H[ 4] = 2.f * qx;
    H[ 5] = 2.f * qw;
    H[ 6] = -2.f * qz;
    H[ 7] = -2.f * qy;

    H[ 8] = 0.f;
    H[ 9] = 4.f * qx;
    H[10] = 4.f * qy;
    H[11] = 0.f;

    H[12] = 4.f * qw * ca + 2.f * qy * sa;
    H[13] = 4.f * qx * ca + 2.f * qz * sa;
    H[14] = 2.f * qw * sa;
    H[15] = 2.f * qx * sa;

    H[16] = 2.f * qz * ca - 2.f * qx * sa;
    H[17] = 2.f * qy * ca - 2.f * qw * sa;
    H[18] = 2.f * qx * ca + 2.f * qz * sa;
    H[19] = 2.f * qw * ca + 2.f * qy * sa;

    H[20] = -2.f * qy * ca + 4.f * qw * sa;
    H[21] = 2.f * qz * ca;
    H[22] = -2.f * qw * ca;
    H[23] = 2.f * qx * ca + 4.f * qz * sa;
}

#include <math.h>
#include "attitude.h"
#include "bmi088.h"
#include "calib.h"
#include "lis2mdl.h"
#include "matrix.h"
#include "streaming.h"
#include "tim_wrapper.h"
#include "util.h"

#define DT  .001f
#define DT2 .0005f

#define SIGMA_OMEGA 0.3f
#define SIGMA_A 0.5f
#define SIGMA_M 0.8f

#define qw (q_est[0])
#define qx (q_est[1])
#define qy (q_est[2])
#define qz (q_est[3])

/* Inclination angle of Earth's magnetic field. */
static float alpha;
/* Sine and cosine of the inclination angle. */
static float sa, ca;

static void measure(void);
static void kalman_filter(void);
static void attitude_to_measurement(const float q_est[4], float z[6]);
static void attitude_to_measurement_jacobian(const float q_est[4], float H[24]);

void attitude_init(uint32_t num_sample) {
    uint32_t cnt;
    float acc_raw[3], acc_ss_frm[3], acc[3], acc_sum[3] = {0.f};
    float mag_raw[3], mag_ss_frm[3], mag[3], mag_sum[3] = {0.f};
    float q_acc[4], mag_acc[3];
    float roll, pitch, yaw;

    cnt = 0U;
    while (cnt < num_sample) {
        if (control_timer_get_flag()) {
            bmi088_read_acc(acc_raw);
            lis2mdl_read_mag(mag_raw);

            calib_acc(acc_raw, acc_ss_frm);
            calib_mag(mag_raw, mag_ss_frm);

            acc[0] = -acc_ss_frm[0];
            acc[1] = acc_ss_frm[1];
            acc[2] = -acc_ss_frm[2];
            mag[0] = -mag_ss_frm[1];
            mag[1] = mag_ss_frm[0];
            mag[2] = -mag_ss_frm[2];

            for (int16_t i = 0; i < 3; i++) {
                acc_sum[i] += acc[i];
                mag_sum[i] += mag[i];
            }

            cnt++;
        }

        control_timer_clear_flag();
    }

    /* Initial values of acceleration and magnetic field. */
    for (int i = 0; i < 3; i++) {
        acc[i] = acc_sum[i] / num_sample;
        mag[i] = mag_sum[i] / num_sample;
    }

    /* Initial value of quaternion. */
    roll = atan2f(-acc[1], -acc[2]);
    pitch = atan2f(acc[0], norm2(acc[1], acc[2]));
    quaternion_from_euler(q_acc, roll, pitch, 0.f);

    /* Magnetic field in inertial frame. */
    quaternion_rot_vec_inv(q_acc, mag, mag_acc);
    yaw = atan2f(-mag_acc[1], mag_acc[0]);

    /* Initial quaternion and RPY. */
    quaternion_from_euler(q, roll, pitch, yaw);
    rpy[0] = roll;
    rpy[1] = pitch;
    rpy[2] = yaw;

    /* Inclination angle. */
    alpha = atan2f(mag_acc[2], norm2(mag_acc[0], mag_acc[1]));
    sa = sinf(alpha);
    ca = cosf(alpha);
}

void attitude_update(void) {
    measure();
    kalman_filter();
}

static void measure(void) {
    float acc_ss_frm[3], ang_ss_frm[3], mag_ss_frm[3];

    bmi088_read_acc(acc_raw);
    bmi088_read_gyro(ang_raw);
    lis2mdl_read_mag(mag_raw);

    /* Calibrate. */
    calib_acc(acc_raw, acc_ss_frm);
    calib_gyro(ang_raw, ang_ss_frm);
    calib_mag(mag_raw, mag_ss_frm);

    /* Change axes from the sensor frame to the FC frame. */
    acc[0] = -acc_ss_frm[0];
    acc[1] = acc_ss_frm[1];
    acc[2] = -acc_ss_frm[2];
    ang[0] = -ang_ss_frm[0];
    ang[1] = ang_ss_frm[1];
    ang[2] = -ang_ss_frm[2];
    mag[0] = -mag_ss_frm[1];
    mag[1] = mag_ss_frm[0];
    mag[2] = -mag_ss_frm[2];
}

static void kalman_filter(void) {
    const float A[16] = {
                  1.f,  ang[0] * DT2,  ang[1] * DT2,  ang[2] * DT2,
        -ang[0] * DT2,           1.f,  ang[2] * DT2, -ang[1] * DT2,
        -ang[1] * DT2, -ang[2] * DT2,           1.f,  ang[0] * DT2,
        -ang[2] * DT2,  ang[1] * DT2, -ang[0] * DT2,           1.f
    };                                          /* 4 x 4. */
    float A_T[16];                              /* 4 x 4. */

    const float J_f[12] = {
         q[1] * DT2,  q[2] * DT2,  q[3] * DT2,
        -q[0] * DT2, -q[3] * DT2,  q[2] * DT2,
         q[3] * DT2, -q[0] * DT2, -q[1] * DT2,
        -q[2] * DT2,  q[1] * DT2, -q[0] * DT2
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
    matrix_transpose(J_f, 4, 3, J_f_T);

    matrix_mul(J_f, J_f_T, 4, 3, 4, Q);
    matrix_mul_scalar(Q, SIGMA_OMEGA * SIGMA_OMEGA, 4, 4, Q);
    for (int16_t i = 0; i < 6; i++) {
        for (int16_t j = 0; j < 6; j++) {
            R[i * 6 + j] = 0.f;
        }
    }
    R[ 0] = SIGMA_A * SIGMA_A;
    R[ 7] = SIGMA_A * SIGMA_A;
    R[14] = SIGMA_A * SIGMA_A;
    R[21] = SIGMA_M * SIGMA_M;
    R[28] = SIGMA_M * SIGMA_M;
    R[35] = SIGMA_M * SIGMA_M;

    /* Estimate. */
    matrix_mul(A, q, 4, 4, 1, q_est);
    matrix_mul(A, P, 4, 4, 4, tmp);
    matrix_mul(tmp, A_T, 4, 4, 4, tmp);
    matrix_add(tmp, Q, 4, 4, P_est);

    attitude_to_measurement(q_est, z_est);
    attitude_to_measurement_jacobian(q_est, H);
    matrix_transpose(H, 6, 4, H_T);

    /* Correct. */
    matrix_mul(H, P_est, 6, 4, 4, tmp);
    matrix_mul(tmp, H_T, 6, 4, 6, tmp);
    matrix_add(tmp, R, 6, 6, tmp);
    matrix_inv_sym(tmp, 6, tmp);
    matrix_mul(H_T, tmp, 4, 6, 6, tmp);
    matrix_mul(P_est, tmp, 4, 4, 6, K);
    matrix_sub(z, z_est, 6, 1, tmp);
    matrix_mul(K, tmp, 4, 6, 1, tmp);
    matrix_add(q_est, tmp, 4, 1, q);
    matrix_mul(K, H, 4, 6, 4, tmp);
    matrix_mul_scalar(tmp, -1.f, 4, 4, tmp);
    matrix_add_eye(tmp, 1.f, 4, tmp);
    matrix_mul(tmp, P_est, 4, 4, 4, P);

    quaternion_normalize(q, q);
    quaternion_to_euler(q, rpy);
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

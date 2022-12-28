#include <math.h>
#include "attitude.h"
#include "bmi088.h"
#include "calib.h"
#include "lis2mdl.h"
#include "streaming.h"
#include "tim_wrapper.h"
#include "util.h"

void attitude_init(uint32_t num_sample) {
    uint32_t cnt;
    float acc_raw[3], acc_ss_frm[3], acc[3], acc_sum[3] = {0.f};
    float mag_raw[3], mag_ss_frm[3], mag[3], mag_sum[3] = {0.f};
    quaternion_t q_acc;
    float mag_acc[3];
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
    quaternion_from_euler(&q_acc, roll, pitch, 0.f);

    /* Magnetic field in inertial frame. */
    quaternion_rot_vec_inv(&q_acc, mag, mag_acc);
    yaw = atan2f(-mag_acc[1], mag_acc[0]);

    quaternion_from_euler(&q, roll, pitch, yaw);
    rpy[0] = roll;
    rpy[1] = pitch;
    rpy[2] = yaw;
}

void attitude_from_measurement(const float acc[3], const float mag[3], quaternion_t *q_meas, float rpy_meas[3]) {
    float roll, pitch, yaw;
    quaternion_t q_acc;
    float mag_acc[3];

    /* Calculate roll and pitch from the acceleration. */
    roll = atan2f(-acc[1], -acc[2]);
    pitch = atan2f(acc[0], norm2(acc[1], acc[2]));
    quaternion_from_euler(&q_acc, roll, pitch, 0.f);

    /* Calculate the yaw from the magnetic field. */
    quaternion_rot_vec_inv(&q_acc, mag, mag_acc);
    yaw = atan2f(-mag_acc[1], mag_acc[0]);

    quaternion_from_euler(q_meas, roll, pitch, yaw);
    rpy_meas[0] = roll;
    rpy_meas[1] = pitch;
    rpy_meas[2] = yaw;
}

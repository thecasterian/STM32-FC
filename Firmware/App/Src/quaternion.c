#include <math.h>
#include <string.h>
#include "quaternion.h"

void quaternion_from_euler(float q[4], float roll, float pitch, float yaw)
{
    float cr, cp, cy, sr, sp, sy;

    cr = cosf(roll / 2.f);
    cp = cosf(pitch / 2.f);
    cy = cosf(yaw / 2.f);

    sr = sinf(roll / 2.f);
    sp = sinf(pitch / 2.f);
    sy = sinf(yaw / 2.f);

    q[0] = cr * cp * cy + sr * sp * sy;
    q[1] = cr * sp * sy - sr * cp * cy;
    q[2] = -(cr * sp * cy + sr * cp * sy);
    q[3] = sr * sp * cy - cr * cp * sy;
}

void quaternion_scale(const float q[4], float scale, float res[4]) {
    float tmp[4];

    tmp[0] = q[0] * scale;
    tmp[1] = q[1] * scale;
    tmp[2] = q[2] * scale;
    tmp[3] = q[3] * scale;

    memcpy(res, tmp, 4U * sizeof(float));
}

void quaternion_normalize(const float q[4], float res[4]) {
    float tmp[4];
    float norm;

    norm = sqrtf((q[1] * q[1]) + (q[2] * q[2]) + (q[3] * q[3]) + (q[0] * q[0]));
    tmp[0] = q[0] / norm;
    tmp[1] = q[1] / norm;
    tmp[2] = q[2] / norm;
    tmp[3] = q[3] / norm;

    memcpy(res, tmp, 4U * sizeof(float));
}

void quaternion_add(const float q1[4], const float q2[4], float res[4]) {
    float tmp[4];

    tmp[0] = q1[0] + q2[0];
    tmp[1] = q1[1] + q2[1];
    tmp[2] = q1[2] + q2[2];
    tmp[3] = q1[3] + q2[3];

    memcpy(res, tmp, 4U * sizeof(float));
}

void quaternion_mul(const float q1[4], const float q2[4], float res[4]) {
    float tmp[4];

    tmp[0] = (q1[0] * q2[0]) - (q1[1] * q2[1]) - (q1[2] * q2[2]) - (q1[3] * q2[3]);
    tmp[1] = (q1[0] * q2[1]) + (q1[1] * q2[0]) + (q1[2] * q2[3]) - (q1[3] * q2[2]);
    tmp[2] = (q1[0] * q2[2]) + (q1[2] * q2[0]) + (q1[3] * q2[1]) - (q1[1] * q2[3]);
    tmp[3] = (q1[0] * q2[3]) + (q1[3] * q2[0]) + (q1[1] * q2[2]) - (q1[2] * q2[1]);

    memcpy(res, tmp, 4U * sizeof(float));
}

void quaternion_rot_vec(const float q[4], const float *vec, float *res) {
    float q_vec[4] = {0.f, vec[0], vec[1], vec[2]}, q_conj[4] = {q[0], -q[1], -q[2], -q[3]}, q_res[4];

    quaternion_mul(q, q_vec, q_vec);
    quaternion_mul(q_vec, q_conj, q_res);

    res[0] = q_res[1];
    res[1] = q_res[2];
    res[2] = q_res[3];
}

void quaternion_rot_vec_inv(const float q[4], const float *vec, float *res) {
    float q_vec[4] = {0.f, vec[0], vec[1], vec[2]}, q_conj[4] = {q[0], -q[1], -q[2], -q[3]}, q_res[4];

    quaternion_mul(q_conj, q_vec, q_vec);
    quaternion_mul(q_vec, q, q_res);

    res[0] = q_res[1];
    res[1] = q_res[2];
    res[2] = q_res[3];
}

void quaternion_to_euler(const float q[4], float *roll, float *pitch, float *yaw) {
    *roll = atan2f(2.f * ((q[2] * q[3]) - (q[0] * q[1])), 1.f - 2.f * ((q[1] * q[1]) + (q[2] * q[2])));
    *pitch = asinf(-2.f * ((q[0] * q[2]) + (q[1] * q[3])));
    *yaw = atan2f(2.f * ((q[1] * q[2]) - (q[0] * q[3])), 1.f - 2.f * ((q[2] * q[2]) + (q[3] * q[3])));
}

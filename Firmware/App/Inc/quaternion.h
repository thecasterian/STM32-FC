#ifndef QUATERNION_H
#define QUATERNION_H

typedef struct {
    float w;                                    /* Real part. */
    float x;                                    /* i-term. */
    float y;                                    /* j-term. */
    float z;                                    /* k-term. */
} quaternion_t;

void quaternion_init(quaternion_t *q, float w, float x, float y, float z);
void quaternion_from_euler(quaternion_t *q, float roll, float pitch, float yaw);

void quaternion_scale(const quaternion_t *q, float scale, quaternion_t *res);
void quaternion_normalize(const quaternion_t *q, quaternion_t *res);

void quaternion_add(const quaternion_t *q1, const quaternion_t *q2, quaternion_t *res);
void quaternion_mul(const quaternion_t *q1, const quaternion_t *q2, quaternion_t *res);

void quaternion_rot_vec(const quaternion_t *q, const float *vec, float *res);
void quaternion_rot_vec_inv(const quaternion_t *q, const float *vec, float *res);
void quaternion_to_euler(const quaternion_t *q, float *roll, float *pitch, float *yaw);

#endif

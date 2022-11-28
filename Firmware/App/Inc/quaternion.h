/**
 * @file quaternion.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Quaternion library.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef QUATERNION_H
#define QUATERNION_H

/**
 * @brief Quaternion: q = w + xi + yj + zk.
 */
typedef struct {
    /** Real part. */
    float w;
    /** i-term. */
    float x;
    /** j-term. */
    float y;
    /** k-term. */
    float z;
} quaternion_t;

/**
 * @brief Initializes the quaternion.
 *
 * @param[out] q Quaternion.
 * @param w Real part.
 * @param x i-term.
 * @param y j-term.
 * @param z k-term.
 */
void quaternion_init(quaternion_t *q, float w, float x, float y, float z);

/**
 * @brief Initializes the quaternion from the ZYX euler angle.
 *
 * @param[out] q Quaternion.
 * @param roll Roll angle (rad).
 * @param pitch Pitch angle (rad).
 * @param yaw Yaw angle (rad).
 */
void quaternion_from_euler(quaternion_t *q, float roll, float pitch, float yaw);

/**
 * @brief Scales the quaternion by a scalar.
 *
 * @param[in] q Quaternion.
 * @param scale Scale factor.
 * @param[out] res Scaled quaternion.
 */
void quaternion_scale(const quaternion_t *q, float scale, quaternion_t *res);

/**
 * @brief Normalizes the quaternion.
 *
 * @param[in] q Quaternion.
 * @param[out] res Normalized quaternion.
 */
void quaternion_normalize(const quaternion_t *q, quaternion_t *res);

/**
 * @brief Adds two quaternions.
 *
 * @param[in] q1 First quaternion.
 * @param[in] q2 Second quaternion.
 * @param[out] res Sum of the two quaternions.
 */
void quaternion_add(const quaternion_t *q1, const quaternion_t *q2, quaternion_t *res);

/**
 * @brief Multiplies two quaternions.
 *
 * @param[in] q1 First quaternion.
 * @param[in] q2 Second quaternion.
 * @param[out] res Product of the two quaternions.
 */
void quaternion_mul(const quaternion_t *q1, const quaternion_t *q2, quaternion_t *res);

/**
 * @brief Rotates the vector by the quaternion.
 * @details Calculates q * v * q^-1.
 *
 * @param[in] q Quaternion.
 * @param[in] vec Vector.
 * @param[out] res Rotated vector.
 */
void quaternion_rot_vec(const quaternion_t *q, const float vec[3], float res[3]);

/**
 * @brief Rotates the vector by the inverse of the quaternion.
 * @details Calculates q^-1 * v * q.
 *
 * @param[in] q Quaternion.
 * @param[in] vec Vector.
 * @param[out] res Rotated vector.
 */
void quaternion_rot_vec_inv(const quaternion_t *q, const float vec[3], float res[3]);

/**
 * @brief Converts the quaternion to the ZYX euler angle.
 *
 * @param[in] q Quaternion.
 * @param[out] roll Roll angle (rad).
 * @param[out] pitch Pitch angle (rad).
 * @param[out] yaw Yaw angle (rad).
 */
void quaternion_to_euler(const quaternion_t *q, float *roll, float *pitch, float *yaw);

#endif

/**
 * @file quaternion.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Quaternion library. A quaternion is reperesented as an array [w, x, y, z] where q = w + xi + yj + zk.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef QUATERNION_H
#define QUATERNION_H

/**
 * @brief Initializes the quaternion from the ZYX euler angle.
 *
 * @param[out] q Quaternion.
 * @param[in] roll Array of roll, pitch, and yaw angles (rad).
 */
void quaternion_from_euler(float q[4], const float rpy[3]);

/**
 * @brief Scales the quaternion by a scalar.
 *
 * @param[in] q Quaternion.
 * @param scale Scale factor.
 * @param[out] res Scaled quaternion.
 */
void quaternion_scale(const float q[4], float scale, float res[4]);

/**
 * @brief Normalizes the quaternion.
 *
 * @param[in] q Quaternion.
 * @param[out] res Normalized quaternion.
 */
void quaternion_normalize(const float q[4], float res[4]);

/**
 * @brief Adds two quaternions.
 *
 * @param[in] q1 First quaternion.
 * @param[in] q2 Second quaternion.
 * @param[out] res Sum of the two quaternions.
 */
void quaternion_add(const float q1[4], const float q2[4], float res[4]);

/**
 * @brief Multiplies two quaternions.
 *
 * @param[in] q1 First quaternion.
 * @param[in] q2 Second quaternion.
 * @param[out] res Product of the two quaternions.
 */
void quaternion_mul(const float q1[4], const float q2[4], float res[4]);

/**
 * @brief Rotates the vector by the quaternion.
 * @details Calculates q * v * q^-1.
 *
 * @param[in] q Quaternion.
 * @param[in] vec Vector.
 * @param[out] res Rotated vector.
 */
void quaternion_rot_vec(const float q[4], const float vec[3], float res[3]);

/**
 * @brief Rotates the vector by the inverse of the quaternion.
 * @details Calculates q^-1 * v * q.
 *
 * @param[in] q Quaternion.
 * @param[in] vec Vector.
 * @param[out] res Rotated vector.
 */
void quaternion_rot_vec_inv(const float q[4], const float vec[3], float res[3]);

/**
 * @brief Converts the quaternion to the ZYX euler angle.
 *
 * @param[in] q Quaternion.
 * @param[out] rpy Array of roll, pitch, and yaw angles (rad).
 */
void quaternion_to_euler(const float q[4], float rpy[3]);

#endif

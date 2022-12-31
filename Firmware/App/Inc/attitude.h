#ifndef ATTITUDE_H
#define ATTITUDE_H

#include <stdint.h>
#include "quaternion.h"

/**
 * @brief Gets the initial attitude.
 *
 * @param num_sample Number of samples from the accelerometer and magnetomerter to calculate the initial attitude.
 */
void attitude_init(uint32_t num_sample);

/**
 * @brief Gets the attitude from the accelerometer and magnetometer measurements.
 *
 * @param[in] acc Acceleration.
 * @param[in] mag Magnetic field.
 * @param[out] q_meas Quaternion from the measurements.
 * @param[out] rpy_meas Roll, pitch, yaw from the measurements.
 */
void attitude_from_measurement(const float acc[3], const float mag[3], float q_meas[4], float rpy_meas[3]);

#endif

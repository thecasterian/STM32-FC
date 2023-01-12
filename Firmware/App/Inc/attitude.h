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
 * @brief Updates the attitude from the measurements.
 */
void attitude_update(void);

#endif

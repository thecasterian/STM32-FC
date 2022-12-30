/**
 * @file esc.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief ESC (electric speed controller) driver.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef ESC_H
#define ESC_H

#include <stdbool.h>
#include "tim_wrapper.h"

typedef enum {
    MOTOR_POSITION_FRONT_LEFT  = 0,
    MOTOR_POSITION_FRONT_RIGHT = 1,
    MOTOR_POSITION_REAR_LEFT   = 2,
    MOTOR_POSITION_REAR_RIGHT  = 3,
} motor_position_t;

/**
 * @brief Set the mapping between the motor position and the PWM channel.
 *
 * @param[in] mapping Array of PWM channels of each motor.
 * @return true if the mapping is valid, false otherwise.
 */
bool esc_set_motor_pwm_mapping(const pwm_channel_t mapping[4]);

/**
 * @brief Sets the throttle of the specified motor.
 *
 * @param[in] throttle Array of throttle value of each motor. A throttle must be in range of [0, 1].
 * @return true if the throttle was set successfully, false otherwise.
 */
bool esc_set_throttle(const float throttle[4]);

#endif

#ifndef ESC_H
#define ESC_H

#include <stdbool.h>
#include "tim_wrapper.h"

typedef enum {
    ESC_PRTCL_STD_PWM    = 0x00,
    ESC_PRTCL_ONESHOT125 = 0x01,
    ESC_PRTCL_ONESHOT42  = 0x02,
    ESC_PRTCL_DSHOT150   = 0x03,
    ESC_PRTCL_DSHOT300   = 0x04,
    ESC_PRTCL_DSHOT600   = 0x05,
} esc_protocol_t;

typedef enum {
    MOTOR_POSITION_FRONT_LEFT  = 0,
    MOTOR_POSITION_FRONT_RIGHT = 1,
    MOTOR_POSITION_REAR_LEFT   = 2,
    MOTOR_POSITION_REAR_RIGHT  = 3,
} motor_position_t;

/**
 * @brief Set the mapping between the motor position and the PWM channel.
 *
 * @param mapping Array of PWM channels of each motor.
 * @return true if the mapping is valid, false otherwise.
 */
bool esc_set_motor_pwm_mapping(pwm_channel_t mapping[4]);

/**
 * @brief Sets the ESC (electric speed controller) protocol.
 *
 * @param protocol ESC protocol.
 * @return true if the protocol was set successfully, false otherwise.
 */
bool esc_set_protocol(esc_protocol_t protocol);

/**
 * @brief Sets the throttle of the specified motor.
 *
 * @param pos Motor position.
 * @param throttle Throttle value in the range [0, 1].
 * @return true if the throttle was set successfully, false otherwise.
 */
bool esc_set_throttle(motor_position_t position, float throttle);

/**
 * @brief Sends the throttle to ESCs.
 */
void esc_send_throttle(void);

#endif

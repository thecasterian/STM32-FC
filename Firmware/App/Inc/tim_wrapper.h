/**
 * @file timer.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief STM32 timer wrapper.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <stdint.h>

#define PWM_TIM_FREQ 84000000.f

typedef enum {
    PWM_CHANNEL_1 = 0,
    PWM_CHANNEL_2 = 1,
    PWM_CHANNEL_3 = 2,
    PWM_CHANNEL_4 = 3,
} pwm_channel_t;

extern bool pwm_en;

/**
 * @brief Starts the control timer.
 */
void control_timer_start(void);

/**
 * @brief Gets the period elapsed flag.
 */
bool control_timer_get_flag(void);

/**
 * @brief Clears the period elapsed flag.
 */
void control_timer_clear_flag(void);

/**
 * @brief Set the PWM frequency.
 *
 * @param freq Frequency in Hz.
 */
void pwm_set_frequency(float freq);

/**
 * @brief Sends a packet of PWM signals.
 *
 * @param ch PWM channel.
 * @param width Array of pulse widths in us.
 * @param len Length of the packet.
 */
void pwm_send_packet(pwm_channel_t ch, float width[], uint16_t len);

#endif

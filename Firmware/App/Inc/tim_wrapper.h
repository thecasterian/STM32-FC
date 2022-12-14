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
 * @brief Starts PWM output.
 */
void pwm_start(void);

/**
 * @brief Stops PWM output.
 */
void pwm_stop(void);

/**
 * @brief Sets the PWM output periods.
 *
 * @param[in] period Array of PWM output periods.
 */
void pwm_set_period(const uint32_t period[4]);

#endif

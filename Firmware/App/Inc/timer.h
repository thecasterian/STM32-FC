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
#include "tim.h"

/**
 * @brief Period timer for a periodic event.
 */
typedef struct {
    /** Timer handle. */
    TIM_HandleTypeDef *htim;
    /** Flag set when the period is elapsed. Reset manually. */
    volatile bool period_elapsed;
} period_timer_t;

/**
 * @brief Initializes the period timer.
 *
 * @param[out] timer Period timer.
 * @param[in] htim Timer handle.
 */
void period_timer_init(period_timer_t *timer, TIM_HandleTypeDef *htim);

/**
 * @brief Callback function for the period timer. Call this function in HAL_TIM_PeriodElapsedCallback().
 *
 * @param[inout] timer Period timer.
 */
void period_timer_callback(period_timer_t *timer);

#endif

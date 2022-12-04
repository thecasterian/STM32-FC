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

#endif

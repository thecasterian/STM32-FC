/**
 * @file led.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Debug LED driver.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef LED_H
#define LED_H

#include <stdbool.h>

typedef enum {
    LED_STATE_OFF = 0,
    LED_STATE_ON  = 1,
} led_state_t;

void led_red_write(led_state_t state);
void led_green_write(led_state_t state);
void led_blue_write(led_state_t state);

void led_red_toggle(void);
void led_green_toggle(void);
void led_blue_toggle(void);

#endif

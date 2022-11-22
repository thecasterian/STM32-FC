#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include "tim.h"

typedef struct {
    TIM_HandleTypeDef *htim;
    volatile bool period_elapsed;
} period_timer_t;

void period_timer_init(period_timer_t *timer, TIM_HandleTypeDef *htim);

void period_timer_callback(period_timer_t *timer);

#endif

#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include "tim.h"

typedef struct {
    TIM_HandleTypeDef *htim;
    volatile bool period_elapsed;
} Timer;

extern Timer timer_cntl, timer_pwm;

void timer_init(Timer *timer, TIM_HandleTypeDef *htim);

#endif

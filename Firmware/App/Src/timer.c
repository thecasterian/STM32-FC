#include "timer.h"

void period_timer_init(period_timer_t *timer, TIM_HandleTypeDef *htim) {
    timer->htim = htim;
    timer->period_elapsed = false;

    HAL_TIM_Base_Start_IT(timer->htim);
}

void period_timer_callback(period_timer_t *timer) {
    timer->period_elapsed = true;
}

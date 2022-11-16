#include "timer.h"

Timer timer_cntl, timer_pwm;

void timer_init(Timer *timer, TIM_HandleTypeDef *htim) {
    timer->htim = htim;
    timer->period_elapsed = false;

    HAL_TIM_Base_Start_IT(timer->htim);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == timer_cntl.htim) {
        timer_cntl.period_elapsed = true;
    } else if (htim == timer_pwm.htim) {
        timer_pwm.period_elapsed = true;
    }
}

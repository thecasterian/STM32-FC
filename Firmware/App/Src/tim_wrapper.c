#include <math.h>
#include "tim.h"
#include "tim_wrapper.h"

#define CONTROL_HTIM (&htim5)

bool pwm_en;

static volatile bool control_timer_flag;

void control_timer_start(void) {
    HAL_TIM_Base_Start_IT(CONTROL_HTIM);
}

bool control_timer_get_flag(void) {
    return control_timer_flag;
}

void control_timer_clear_flag(void) {
    control_timer_flag = false;
}

void pwm_start(void) {
    HAL_TIM_PWM_Start(CONTROL_HTIM, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(CONTROL_HTIM, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(CONTROL_HTIM, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(CONTROL_HTIM, TIM_CHANNEL_4);
}

void pwm_stop(void) {
    HAL_TIM_PWM_Stop(CONTROL_HTIM, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(CONTROL_HTIM, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(CONTROL_HTIM, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(CONTROL_HTIM, TIM_CHANNEL_4);
}

void pwm_set_period(const uint32_t period[4]) {
    CONTROL_HTIM->Instance->CCR1 = period[0];
    CONTROL_HTIM->Instance->CCR2 = period[1];
    CONTROL_HTIM->Instance->CCR3 = period[2];
    CONTROL_HTIM->Instance->CCR4 = period[3];
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == CONTROL_HTIM) {
        control_timer_flag = true;
    }
}

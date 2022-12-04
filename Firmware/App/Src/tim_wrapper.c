#include "application.h"
#include "tim.h"
#include "tim_wrapper.h"

static volatile bool control_timer_flag = false;

void control_timer_start(void) {
    HAL_TIM_Base_Start_IT(&CONTROL_HTIM);
}

bool control_timer_get_flag(void) {
    return control_timer_flag;
}

void control_timer_clear_flag(void) {
    control_timer_flag = false;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    control_timer_flag = true;
}

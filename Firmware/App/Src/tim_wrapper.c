#include <math.h>
#include "tim.h"
#include "tim_wrapper.h"

#define CONTROL_HTIM htim6

bool pwm_en;

static volatile bool control_timer_flag;

static struct {
    uint32_t arr;
    uint32_t ccr[4];
    bool on;
} pwm_config;

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

void pwm_set_frequency(float freq) {
    pwm_config.arr = lrintf(PWM_TIM_FREQ / freq) - 1U;

    __HAL_TIM_SET_PRESCALER(&htim5, 0U);
    __HAL_TIM_SET_AUTORELOAD(&htim5, pwm_config.arr);
}

void pwm_send_packet(pwm_channel_t ch, float width[], uint16_t len) {
    uint32_t ccr[128];

    if ((!pwm_config.on) && (len < 128U)) {
        for (uint16_t i = 0; i < len; i++) {
            ccr[i] = lrintf(PWM_TIM_FREQ * width[i] * 1.e-6f);
        }

        switch (ch) {
        case PWM_CHANNEL_1:
            HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_1, ccr, len);
            break;
        case PWM_CHANNEL_2:
            HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_2, ccr, len);
            break;
        case PWM_CHANNEL_3:
            HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_3, ccr, len);
            break;
        case PWM_CHANNEL_4:
            HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, ccr, len);
            break;
        default:
            break;
        }
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    /* This callback is called when the whole packet is transmitted via DMA. */
    if (htim->Instance == TIM5) {
        switch (htim->Channel) {
        case HAL_TIM_ACTIVE_CHANNEL_1:
            HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_1);
            break;
        case HAL_TIM_ACTIVE_CHANNEL_2:
            HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_2);
            break;
        case HAL_TIM_ACTIVE_CHANNEL_3:
            HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_3);
            break;
        case HAL_TIM_ACTIVE_CHANNEL_4:
            HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_4);
            break;
        default:
            break;
        }
    }
}

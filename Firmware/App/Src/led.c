#include "led.h"
#include "main.h"

void led_red_write(led_state_t state) {
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, state == LED_STATE_OFF ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void led_green_write(led_state_t state) {
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, state == LED_STATE_OFF ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void led_blue_write(led_state_t state) {
    HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, state == LED_STATE_OFF ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void led_red_toggle(void) {
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
}

void led_green_toggle(void) {
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}

void led_blue_toggle(void) {
    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
}

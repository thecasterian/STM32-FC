#include <stdio.h>
#include "application.h"
#include "main.h"
#include "timer.h"
#include "usbd_cdc_if.h"

void setup(void) {
    timer_init(&timer_cntl, &htim6);
}

void loop(void) {
    if (timer_cntl.period_elapsed) {
        HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);

        timer_cntl.period_elapsed = false;
    }
}

int _write(int file, char *ptr, int len) {
    int res;

    UNUSED(file);

    if (CDC_Transmit_FS((uint8_t*)ptr, len) != USBD_OK) {
        res = -1;
    } else {
        res = len;
    }

    return res;
}

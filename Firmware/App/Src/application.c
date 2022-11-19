#include <stdio.h>
#include "application.h"
#include "bmi088.h"
#include "main.h"
#include "timer.h"
#include "usbd_cdc_if.h"

static Bmi088 bmi088;

uint8_t dump[128];

void setup(void) {
    timer_init(&timer_cntl, &htim6);

    bmi088_init(&bmi088, &hspi1, ACC_NSS_GPIO_Port, ACC_NSS_Pin, GYRO_NSS_GPIO_Port, GYRO_NSS_Pin);
    bmi088_set_range(&bmi088, BMI088_ACC_RANGE_24G, BMI088_GYRO_RANGE_2000DPS);
    bmi088_set_odr(&bmi088, BMI088_ACC_ODR_1600HZ, BMI088_ACC_BWP_NORMAL, BMI088_GYRO_ODR_1000HZ_BWP_116HZ);
}

void loop(void) {
    float acc_raw[3], gyro_raw[3], acc[3], gyro[3];
    uint8_t buf[25];

    if (timer_cntl.period_elapsed) {
        bmi088_read_acc(&bmi088, acc_raw);
        bmi088_read_gyro(&bmi088, gyro_raw);

        /* Change axis. */
       acc[0] = -acc_raw[0];
       acc[1] = acc_raw[1];
       acc[2] = -acc_raw[2];
       gyro[0] = -gyro_raw[0];
       gyro[1] = gyro_raw[1];
       gyro[2] = -gyro_raw[2];

        buf[0] = 0x42;
        memcpy(&buf[1], acc, sizeof(acc));
        memcpy(&buf[13], gyro, sizeof(gyro));
        _write(1, (char *)buf, sizeof(buf));

        timer_cntl.period_elapsed = false;
    }
}

int _write(int file, char *ptr, int len) {
    int res;

    UNUSED(file);

    if (CDC_Transmit_FS((uint8_t *)ptr, len) != USBD_OK) {
        res = -1;
    } else {
        res = len;
    }

    return res;
}

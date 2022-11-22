#include <math.h>
#include <stdio.h>
#include "application.h"
#include "bmi088.h"
#include "bmp280.h"
#include "lis2mdl.h"
#include "main.h"
#include "timer.h"
#include "usbd_cdc_if.h"

static period_timer_t timer;
static bmi088_t bmi088;
static lis2mdl_t lis2mdl;
static bmp280_t bmp280;

void setup(void) {
    timer_init(&timer, &htim6);

    bmi088_init(&bmi088, &hspi1, ACC_NSS_GPIO_Port, ACC_NSS_Pin, GYRO_NSS_GPIO_Port, GYRO_NSS_Pin);
    bmi088_set_range(&bmi088, BMI088_ACC_RANGE_24G, BMI088_GYRO_RANGE_2000DPS);
    bmi088_set_odr(&bmi088, BMI088_ACC_ODR_1600HZ, BMI088_ACC_BWP_NORMAL, BMI088_GYRO_ODR_1000HZ_BWP_116HZ);

    lis2mdl_init(&lis2mdl, &hspi1, MAG_NSS_GPIO_Port, MAG_NSS_Pin);
    lis2mdl_set_odr(&lis2mdl, LIS2MDL_ODR_100Hz);

    bmp280_init(&bmp280, &hspi1, BARO_NSS_GPIO_Port, BARO_NSS_Pin);
    bmp280_set_param(&bmp280, BMP280_OSPL_X16, BMP280_OSPL_X2, BMP280_STBY_TIME_0_5_MS, BMP280_IIR_COEFF_16);
}

void loop(void) {
    float acc_raw[3], gyro_raw[3], mag_raw[3], acc[3], gyro[3], mag[3], pres;
    uint8_t buf[41];

    if (timer.period_elapsed) {
        bmi088_read_acc(&bmi088, acc_raw);
        bmi088_read_gyro(&bmi088, gyro_raw);
        lis2mdl_read_mag(&lis2mdl, mag_raw);
        bmp280_read_pres(&bmp280, &pres);

        /* Change axes. */
        acc[0] = -acc_raw[0];
        acc[1] = acc_raw[1];
        acc[2] = -acc_raw[2];
        gyro[0] = -gyro_raw[0];
        gyro[1] = gyro_raw[1];
        gyro[2] = -gyro_raw[2];
        mag[0] = -mag_raw[1];
        mag[1] = mag_raw[0];
        mag[2] = -mag_raw[2];

        buf[0] = 0x42;
        memcpy(&buf[1], acc, sizeof(acc));
        memcpy(&buf[13], gyro, sizeof(gyro));
        memcpy(&buf[25], mag, sizeof(mag));
        memcpy(&buf[37], &pres, sizeof(pres));
        _write(1, (char *)buf, sizeof(buf));

        timer.period_elapsed = false;
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == timer.htim) {
        period_timer_callback(&timer);
    }
}

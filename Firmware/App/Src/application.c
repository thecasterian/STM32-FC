#include <math.h>
#include <stdio.h>
#include "application.h"
#include "bmi088.h"
#include "bmp280.h"
#include "calib.h"
#include "lis2mdl.h"
#include "main.h"
#include "protocol.h"
#include "timer.h"
#include "usb_queue.h"
#include "usbd_cdc_if.h"

static period_timer_t timer;
static packet_recver_t packet_recver;

static bmi088_t bmi088;
static lis2mdl_t lis2mdl;
static bmp280_t bmp280;

float acc[3], gyro[3], mag[3], pres, temp, raw_acc_meas[3], raw_gyro_meas[3], raw_mag_meas[3];
float kf_rpy[3], kf_acc[3], kf_vel[3], kf_pos[3], acc_rpy[3], baro_height;

static float acc_ss_frm[3], gyro_ss_frm[3], mag_ss_frm[3];

void setup(void) {
    period_timer_init(&timer, &htim6);
    packet_recver_init(&packet_recver);

    bmi088_init(&bmi088, &hspi1, ACC_NSS_GPIO_Port, ACC_NSS_Pin, GYRO_NSS_GPIO_Port, GYRO_NSS_Pin);
    bmi088_set_range(&bmi088, BMI088_ACC_RANGE_24G, BMI088_GYRO_RANGE_2000DPS);
    bmi088_set_odr(&bmi088, BMI088_ACC_ODR_1600HZ, BMI088_ACC_BWP_NORMAL, BMI088_GYRO_ODR_1000HZ_BWP_116HZ);

    lis2mdl_init(&lis2mdl, &hspi1, MAG_NSS_GPIO_Port, MAG_NSS_Pin);
    lis2mdl_set_odr(&lis2mdl, LIS2MDL_ODR_100Hz);

    bmp280_init(&bmp280, &hspi1, BARO_NSS_GPIO_Port, BARO_NSS_Pin);
    bmp280_set_param(&bmp280, BMP280_OSPL_X16, BMP280_OSPL_X2, BMP280_STBY_TIME_0_5_MS, BMP280_IIR_OFF);
}

void loop(void) {
    uint8_t err;

    if (timer.period_elapsed) {
        bmi088_read_acc(&bmi088, raw_acc_meas);
        bmi088_read_gyro(&bmi088, raw_gyro_meas);
        lis2mdl_read_mag(&lis2mdl, raw_mag_meas);
        bmp280_read_pres_temp(&bmp280, &pres, &temp);

        /* Calibrate. */
        calib_acc(raw_acc_meas, acc_ss_frm);
        calib_gyro(raw_gyro_meas, gyro_ss_frm);
        calib_mag(raw_mag_meas, mag_ss_frm);

        /* Change axes from the sensor frame to the FC frame. */
        acc[0] = -acc_ss_frm[0];
        acc[1] = acc_ss_frm[1];
        acc[2] = -acc_ss_frm[2];
        gyro[0] = -gyro_ss_frm[0];
        gyro[1] = gyro_ss_frm[1];
        gyro[2] = -gyro_ss_frm[2];
        mag[0] = -mag_ss_frm[1];
        mag[1] = mag_ss_frm[0];
        mag[2] = -mag_ss_frm[2];

        stream_send();

        timer.period_elapsed = false;
    }

    packet_recver_recv(&packet_recver);
    if (packet_recver.recved) {
        packet_validate(&packet_recver.packet, &err);
        if (err == ERR_OK) {
            command_execute(&packet_recver.packet);
        }
        response_send(err);
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

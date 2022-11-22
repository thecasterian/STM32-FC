#ifndef BMI088_H
#define BMI088_H

#include "spi.h"

/* BMI088 accelerometer maximum measurement range. */
typedef enum {
    BMI088_ACC_RANGE_3G  = 0x00,
    BMI088_ACC_RANGE_6G  = 0x01,
    BMI088_ACC_RANGE_12G = 0x02,
    BMI088_ACC_RANGE_24G = 0x03,
} bmi088_acc_range_t;

/* BMI088 gyro maximum measurement range. */
typedef enum {
    BMI088_GYRO_RANGE_125DPS  = 0x04,
    BMI088_GYRO_RANGE_250DPS  = 0x03,
    BMI088_GYRO_RANGE_500DPS  = 0x02,
    BMI088_GYRO_RANGE_1000DPS = 0x01,
    BMI088_GYRO_RANGE_2000DPS = 0x00,
} bmi088_gyro_range_t;

/* BMI088 accelerometer output data rate (ODR). */
typedef enum {
    BMI088_ACC_ODR_12_5HZ = 0x05,
    BMI088_ACC_ODR_25HZ   = 0x06,
    BMI088_ACC_ODR_50HZ   = 0x07,
    BMI088_ACC_ODR_100HZ  = 0x08,
    BMI088_ACC_ODR_200HZ  = 0x09,
    BMI088_ACC_ODR_400HZ  = 0x0A,
    BMI088_ACC_ODR_800HZ  = 0x0B,
    BMI088_ACC_ODR_1600HZ = 0x0C,
} bmi088_acc_odr_t;

/* BMI088 accelerometer low-pass filter bandwidth parameter. Refer to the 3 dB cut-off frequency of the digital low-pass
   filter in section 4.4.1 of the datasheet. */
typedef enum {
    BMI088_ACC_BWP_NORMAL = 0xA0,               /* No oversampling. */
    BMI088_ACC_BWP_OSR2   = 0x90,               /* 2-fold oversampling. */
    BMI088_ACC_BWP_OSR4   = 0x80,               /* 4-fold oversampling. */
} bmi088_acc_bwp_t;

/* BMI088 gyro output data rate (ODR) and low-pass filter bandwidth parameter. Refer to the filter bandwidth of in
   section 5.5.6 of the datasheet. */
typedef enum {
    BMI088_GYRO_ODR_100HZ_BWP_12HZ   = 0x05,
    BMI088_GYRO_ODR_100HZ_BWP_32HZ   = 0x07,
    BMI088_GYRO_ODR_200HZ_BWP_23HZ   = 0x04,
    BMI088_GYRO_ODR_200HZ_BWP_64HZ   = 0x06,
    BMI088_GYRO_ODR_400HZ_BWP_47HZ   = 0x03,
    BMI088_GYRO_ODR_1000HZ_BWP_116HZ = 0x02,
    BMI088_GYRO_ODR_2000HZ_BWP_230HZ = 0x01,
    BMI088_GYRO_ODR_2000HZ_BWP_532HZ = 0x00,
} bmi088_gyro_odr_bwp;

typedef struct {
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *acc_nss_port;
    uint16_t acc_nss_pin;
    GPIO_TypeDef *gyro_nss_port;
    uint16_t gyro_nss_pin;

    uint8_t acc_chip_id;
    uint8_t gyro_chip_id;

    bmi088_acc_range_t acc_range;
    bmi088_gyro_range_t gyro_range;
} bmi088_t;

void bmi088_init(bmi088_t *bmi088, SPI_HandleTypeDef *hspi, GPIO_TypeDef *acc_nss_port, uint16_t acc_nss_pin,
                 GPIO_TypeDef *gyro_nss_port, uint16_t gyro_nss_pin);
void bmi088_set_range(bmi088_t *bmi088, bmi088_acc_range_t acc_range, bmi088_gyro_range_t gyro_range);
void bmi088_set_odr(bmi088_t *bmi088, bmi088_acc_odr_t acc_odr, bmi088_acc_bwp_t acc_bwp,
                    bmi088_gyro_odr_bwp gyro_odr_bwp);

void bmi088_read_acc(bmi088_t *bmi088, float *acc);
void bmi088_read_gyro(bmi088_t *bmi088, float *gyro);

#endif

#ifndef BMI088_H
#define BMI088_H

#include <stdbool.h>
#include "spi.h"

/* BMI088 accelerometer maximum measurement range. */
typedef enum {
    BMI088_ACC_RANGE_3G  = 0x00,
    BMI088_ACC_RANGE_6G  = 0x01,
    BMI088_ACC_RANGE_12G = 0x02,
    BMI088_ACC_RANGE_24G = 0x03,
} Bmi088AccRange;

/* BMI088 gyro maximum measurement range. */
typedef enum {
    BMI088_GYRO_RANGE_125DPS  = 0x04,
    BMI088_GYRO_RANGE_250DPS  = 0x03,
    BMI088_GYRO_RANGE_500DPS  = 0x02,
    BMI088_GYRO_RANGE_1000DPS = 0x01,
    BMI088_GYRO_RANGE_2000DPS = 0x00,
} Bmi088GyroRange;

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
} Bmi088AccOdr;

/* BMI088 accelerometer low-pass filter bandwidth parameter. Refer to the 3 dB cut-off frequency of the digital low-pass
   filter in section 4.4.1 of the datasheet. */
typedef enum {
    BMI088_ACC_BWP_NORMAL = 0x0A,               /* No oversampling. */
    BMI088_ACC_BWP_OSR2   = 0x09,               /* 2-fold oversampling. */
    BMI088_ACC_BWP_OSR4   = 0x08,               /* 4-fold oversampling. */
} Bmi088AccBwp;

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
} Bmi088GyroOdrBwp;

typedef struct {
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *acc_nss_port;
    uint16_t acc_nss_pin;
    GPIO_TypeDef *gyro_nss_port;
    uint16_t gyro_nss_pin;

    uint8_t acc_chip_id;
    uint8_t gyro_chip_id;

    Bmi088AccRange acc_range;
    Bmi088GyroRange gyro_range;
} Bmi088;

void bmi088_init(Bmi088 *bmi088, SPI_HandleTypeDef *hspi, GPIO_TypeDef *acc_nss_port, uint16_t acc_nss_pin,
                 GPIO_TypeDef *gyro_nss_port, uint16_t gyro_nss_pin);
void bmi088_set_range(Bmi088 *bmi088, Bmi088AccRange acc_range, Bmi088GyroRange gyro_range);
void bmi088_set_odr(Bmi088 *bmi088, Bmi088AccOdr acc_odr, Bmi088AccBwp acc_bwp, Bmi088GyroOdrBwp gyro_odr_bwp);

void bmi088_read_acc(Bmi088 *bmi088, float *acc);
void bmi088_read_gyro(Bmi088 *bmi088, float *gyro);

#endif

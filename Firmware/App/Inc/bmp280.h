#ifndef BMP280_H
#define BMP280_H

#include "spi.h"

/* BMP280 pressure and temperature sensor oversampling. */
typedef enum {
    BMP280_OSPL_X1  = 1,
    BMP280_OSPL_X2  = 2,
    BMP280_OSPL_X4  = 3,
    BMP280_OSPL_X8  = 4,
    BMP280_OSPL_X16 = 5,
} bmp280_ospl_t;

typedef enum {
    BMP280_STBY_TIME_0_5_MS  = 0,
    BMP280_STBY_TIME_62_5_MS = 1,
    BMP280_STBY_TIME_125_MS  = 2,
    BMP280_STBY_TIME_250_MS  = 3,
    BMP280_STBY_TIME_500_MS  = 4,
    BMP280_STBY_TIME_1000_MS = 5,
    BMP280_STBY_TIME_2000_MS = 6,
    BMP280_STBY_TIME_4000_MS = 7,
} bmp280_stby_time_t;

typedef enum {
    BMP280_IIR_OFF      = 0,
    BMP280_IIR_COEFF_2  = 1,
    BMP280_IIR_COEFF_4  = 2,
    BMP280_IIR_COEFF_8  = 3,
    BMP280_IIR_COEFF_16 = 4,
} bmp280_iir_coeff_t;

typedef struct {
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *nss_port;
    uint16_t nss_pin;

    uint8_t chip_id;

    int32_t dig_t1;
    int32_t dig_t2;
    int32_t dig_t3;
    int64_t dig_p1;
    int64_t dig_p2;
    int64_t dig_p3;
    int64_t dig_p4;
    int64_t dig_p5;
    int64_t dig_p6;
    int64_t dig_p7;
    int64_t dig_p8;
    int64_t dig_p9;
} bmp280_t;

void bmp280_init(bmp280_t *bmp280, SPI_HandleTypeDef *hspi, GPIO_TypeDef *nss_port, uint16_t nss_pin);
void bmp280_set_param(bmp280_t *bmp280, bmp280_ospl_t pres_ospl, bmp280_ospl_t temp_ospl, bmp280_stby_time_t stby_time,
                      bmp280_iir_coeff_t iir_coeff);

void bmp280_read_pres(bmp280_t *bmp280, float *pres);

#endif

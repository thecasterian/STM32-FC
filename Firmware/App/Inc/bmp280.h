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
} Bmp280Ospl;

typedef enum {
    BMP280_STBY_TIME_0_5_MS  = 0,
    BMP280_STBY_TIME_62_5_MS = 1,
    BMP280_STBY_TIME_125_MS  = 2,
    BMP280_STBY_TIME_250_MS  = 3,
    BMP280_STBY_TIME_500_MS  = 4,
    BMP280_STBY_TIME_1000_MS = 5,
    BMP280_STBY_TIME_2000_MS = 6,
    BMP280_STBY_TIME_4000_MS = 7,
} Bmp280StbyTime;

typedef enum {
    BMP280_IIR_OFF      = 0,
    BMP280_IIR_COEFF_2  = 1,
    BMP280_IIR_COEFF_4  = 2,
    BMP280_IIR_COEFF_8  = 3,
    BMP280_IIR_COEFF_16 = 4,
} Bmp280IirCoeff;

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
} Bmp280;

void bmp280_init(Bmp280 *bmp280, SPI_HandleTypeDef *hspi, GPIO_TypeDef *nss_port, uint16_t nss_pin);
void bmp280_set_param(Bmp280 *bmp280, Bmp280Ospl pres_ospl, Bmp280Ospl temp_ospl, Bmp280StbyTime stby_time,
                      Bmp280IirCoeff iir_coeff);

void bmp280_read_pres(Bmp280 *bmp280, float *pres);

#endif

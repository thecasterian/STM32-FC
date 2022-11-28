/**
 * @file bmp280.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief BMP280 barometer driver.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef BMP280_H
#define BMP280_H

#include "spi.h"

/**
 * @brief BMP280 pressure and temperature oversampling ratio.
 */
typedef enum {
    /** Oversampling x1. */
    BMP280_OSPL_X1  = 1,
    /** Oversampling x2. */
    BMP280_OSPL_X2  = 2,
    /** Oversampling x4. */
    BMP280_OSPL_X4  = 3,
    /** Oversampling x8. */
    BMP280_OSPL_X8  = 4,
    /** Oversampling x16. */
    BMP280_OSPL_X16 = 5,
} bmp280_ospl_t;

/**
 * @brief BMP280 measurement standby time (time between the end of a measurement and the start of the next measurement).
 */
typedef enum {
    /** 0.5 ms. */
    BMP280_STBY_TIME_0_5_MS  = 0,
    /** 62.5 ms. */
    BMP280_STBY_TIME_62_5_MS = 1,
    /** 125 ms. */
    BMP280_STBY_TIME_125_MS  = 2,
    /** 250 ms. */
    BMP280_STBY_TIME_250_MS  = 3,
    /** 500 ms. */
    BMP280_STBY_TIME_500_MS  = 4,
    /** 1000 ms. */
    BMP280_STBY_TIME_1000_MS = 5,
    /** 2000 ms. */
    BMP280_STBY_TIME_2000_MS = 6,
    /** 4000 ms. */
    BMP280_STBY_TIME_4000_MS = 7,
} bmp280_stby_time_t;

/**
 * @brief BMP280 pressure IIR filter coefficient.
 */
typedef enum {
    /** No IIR filter. */
    BMP280_IIR_OFF      = 0,
    /** Coefficient = 2. */
    BMP280_IIR_COEFF_2  = 1,
    /** Coefficient = 4. */
    BMP280_IIR_COEFF_4  = 2,
    /** Coefficient = 8. */
    BMP280_IIR_COEFF_8  = 3,
    /** Coefficient = 16. */
    BMP280_IIR_COEFF_16 = 4,
} bmp280_iir_coeff_t;

/**
 * @brief BMP280 barometer sensor.
 */
typedef struct {
    /** SPI handle. */
    SPI_HandleTypeDef *hspi;

    /** NSS port. */
    GPIO_TypeDef *nss_port;
    /** NSS pin. */
    uint16_t nss_pin;

    /** Chip ID. */
    uint8_t chip_id;

    ///@{
    /** Calibration data. */
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
    // @}
} bmp280_t;

/**
 * @brief Initializes the BMP280.
 *
 * @param[out] bmp280 BMP280.
 * @param[in] hspi SPI handle that the BMP280 is connected to.
 * @param[in] nss_port NSS port.
 * @param nss_pin NSS pin.
 */
void bmp280_init(bmp280_t *bmp280, SPI_HandleTypeDef *hspi, GPIO_TypeDef *nss_port, uint16_t nss_pin);

/**
 * @brief Sets the parameters.
 *
 * @param[out] bmp280 BMP280.
 * @param pres_ospl Pressure oversampling ratio.
 * @param temp_ospl Temperature oversampling ratio.
 * @param stby_time Measurement standby time.
 * @param iir_coeff Pressure IIR filter coefficient.
 */
void bmp280_set_param(bmp280_t *bmp280, bmp280_ospl_t pres_ospl, bmp280_ospl_t temp_ospl, bmp280_stby_time_t stby_time,
                      bmp280_iir_coeff_t iir_coeff);

/**
 * @brief Reads the pressure and temperature.
 *
 * @param[in] bmp280 BMP280.
 * @param[out] pres Pressure (Pa).
 * @param[out] temp Temperature (degrees Celsius).
 */
void bmp280_read_pres_temp(bmp280_t *bmp280, float *pres, float *temp);

#endif
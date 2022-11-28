/**
 * @file lis2mdl.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief LIS2MDL magnetometer driver.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef LIS2MDL_H
#define LIS2MDL_H

#include "spi.h"

/**
 * @brief LIS2MDL output data rate (ODR).
 */
typedef enum {
    /** 10 Hz. */
    LIS2MDL_ODR_10Hz  = 0x00,
    /** 20 Hz. */
    LIS2MDL_ODR_20Hz  = 0x04,
    /** 50 Hz. */
    LIS2MDL_ODR_50Hz  = 0x08,
    /** 100 Hz. */
    LIS2MDL_ODR_100Hz = 0x0C,
} lis2mdl_odr_t;

/**
 * @brief LIS2MDL magnetometer sensor.
 */
typedef struct {
    /** SPI handle. */
    SPI_HandleTypeDef *hspi;

    /** NSS port. */
    GPIO_TypeDef *nss_port;
    /** NSS pin. */
    uint16_t nss_pin;

    /** Chip ID. */
    uint8_t who_am_i;
} lis2mdl_t;

/**
 * @brief Initializes the LIS2MDL.
 *
 * @param[out] lis2mdl LIS2MDL.
 * @param[in] hspi SPI handle that the LIS2MDL is connected to.
 * @param[in] nss_port NSS port.
 * @param nss_pin NSS pin.
 */
void lis2mdl_init(lis2mdl_t *lis2mdl, SPI_HandleTypeDef *hspi, GPIO_TypeDef *nss_port, uint16_t nss_pin);

/**
 * @brief Sets the output data rate.
 *
 * @param[out] lis2mdl LIS2MDL.
 * @param odr Output data rate.
 */
void lis2mdl_set_odr(lis2mdl_t *lis2mdl, lis2mdl_odr_t odr);

/**
 * @brief Reads the magnetometer measurement.
 *
 * @param[in] lis2mdl LIS2MDL.
 * @param[out] mag Magnetic field in x, y, and z axes (mG).
 */
void lis2mdl_read_mag(lis2mdl_t *lis2mdl, float mag[3]);

#endif

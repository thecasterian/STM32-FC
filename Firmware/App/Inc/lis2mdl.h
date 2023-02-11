/**
 * @file lis2mdl.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief LIS2MDL magnetometer driver.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef LIS2MDL_H
#define LIS2MDL_H

#include "gpio.h"
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
 * @brief LIS2MDL driver structure.
 */
typedef struct {
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *nss_port;
    uint16_t nss_pin;
} lis2mdl_t;

/**
 * @brief Initializes the LIS2MDL.
 *
 * @param lis2mdl LIS2MDL driver structure.
 * @param hspi SPI handle.
 * @param nss_port NSS GPIO port.
 * @param nss_pin NSS GPIO pin.
 */
void lis2mdl_init(lis2mdl_t *lis2mdl, SPI_HandleTypeDef *hspi, GPIO_TypeDef *nss_port, uint16_t nss_pin);

/**
 * @brief Sets the output data rate.
 *
 * @param lis2mdl LIS2MDL driver structure.
 * @param odr Output data rate.
 */
void lis2mdl_set_odr(lis2mdl_t *lis2mdl, lis2mdl_odr_t odr);

/**
 * @brief Reads the magnetometer measurement.
 *
 * @param lis2mdl LIS2MDL driver structure.
 * @param[out] mag Magnetic field in x, y, and z axes (uT).
 */
void lis2mdl_read_mag(lis2mdl_t *lis2mdl, float mag[3]);

#endif

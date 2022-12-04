/**
 * @file lis2mdl.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief LIS2MDL magnetometer driver.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef LIS2MDL_H
#define LIS2MDL_H

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
 * @brief Initializes the LIS2MDL.
 */
void lis2mdl_init(void);

/**
 * @brief Sets the output data rate.
 *
 * @param odr Output data rate.
 */
void lis2mdl_set_odr(lis2mdl_odr_t odr);

/**
 * @brief Reads the magnetometer measurement.
 *
 * @param[out] mag Magnetic field in x, y, and z axes (mG).
 */
void lis2mdl_read_mag(float mag[3]);

#endif

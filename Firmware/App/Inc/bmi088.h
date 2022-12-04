/**
 * @file bmi088.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief BMI088 accelerometer and gyro driver.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef BMI088_H
#define BMI088_H

/**
 * @brief BMI088 accelerometer maximum measurement range.
 */
typedef enum {
    /** +/- 3g. */
    BMI088_ACC_RANGE_3G  = 0x00,
    /** +/- 6g. */
    BMI088_ACC_RANGE_6G  = 0x01,
    /** +/- 12g. */
    BMI088_ACC_RANGE_12G = 0x02,
    /** +/- 24g. */
    BMI088_ACC_RANGE_24G = 0x03,
} bmi088_acc_range_t;

/**
 * @brief BMI088 gyro maximum measurement range.
 */
typedef enum {
    /** +/- 125 deg/s. */
    BMI088_GYRO_RANGE_125DPS  = 0x04,
    /** +/- 250 deg/s. */
    BMI088_GYRO_RANGE_250DPS  = 0x03,
    /** +/- 500 deg/s. */
    BMI088_GYRO_RANGE_500DPS  = 0x02,
    /** +/- 1000 deg/s. */
    BMI088_GYRO_RANGE_1000DPS = 0x01,
    /** +/- 2000 deg/s. */
    BMI088_GYRO_RANGE_2000DPS = 0x00,
} bmi088_gyro_range_t;

/**
 * @brief BMI088 accelerometer output data rate (ODR).
 */
typedef enum {
    /* 12.5 Hz. */
    BMI088_ACC_ODR_12_5HZ = 0x05,
    /* 25 Hz. */
    BMI088_ACC_ODR_25HZ   = 0x06,
    /* 50 Hz. */
    BMI088_ACC_ODR_50HZ   = 0x07,
    /* 100 Hz. */
    BMI088_ACC_ODR_100HZ  = 0x08,
    /* 200 Hz. */
    BMI088_ACC_ODR_200HZ  = 0x09,
    /* 400 Hz. */
    BMI088_ACC_ODR_400HZ  = 0x0A,
    /* 800 Hz. */
    BMI088_ACC_ODR_800HZ  = 0x0B,
    /* 1600 Hz. */
    BMI088_ACC_ODR_1600HZ = 0x0C,
} bmi088_acc_odr_t;

/**
 * @brief BMI088 accelerometer low-pass filter bandwidth parameter.
 * @details Refer to the 3 dB cut-off frequency of the digital low-pass filter in section 4.4.1 of the datasheet.
 */
typedef enum {
    /** No oversampling. */
    BMI088_ACC_BWP_NORMAL = 0xA0,
    /** 2-fold oversampling. */
    BMI088_ACC_BWP_OSR2   = 0x90,
    /** 4-fold oversampling. */
    BMI088_ACC_BWP_OSR4   = 0x80,
} bmi088_acc_bwp_t;

/**
 * @brief BMI088 gyro output data rate (ODR) and low-pass filter bandwidth parameter.
 * @details Refer to the filter bandwidth of in section 5.5.6 of the datasheet.
 */
typedef enum {
    /** 100 Hz ODR, 12 Hz filter bandwidth. */
    BMI088_GYRO_ODR_100HZ_BWP_12HZ   = 0x05,
    /** 100 Hz ODR, 32 Hz filter bandwidth. */
    BMI088_GYRO_ODR_100HZ_BWP_32HZ   = 0x07,
    /** 200 Hz ODR, 23 Hz filter bandwidth. */
    BMI088_GYRO_ODR_200HZ_BWP_23HZ   = 0x04,
    /** 200 Hz ODR, 64 Hz filter bandwidth. */
    BMI088_GYRO_ODR_200HZ_BWP_64HZ   = 0x06,
    /** 400 Hz ODR, 47 Hz filter bandwidth. */
    BMI088_GYRO_ODR_400HZ_BWP_47HZ   = 0x03,
    /** 1000 Hz ODR, 116 Hz filter bandwidth. */
    BMI088_GYRO_ODR_1000HZ_BWP_116HZ = 0x02,
    /** 2000 Hz ODR, 230 Hz filter bandwidth. */
    BMI088_GYRO_ODR_2000HZ_BWP_230HZ = 0x01,
    /** 2000 Hz ODR, 532 Hz filter bandwidth. */
    BMI088_GYRO_ODR_2000HZ_BWP_532HZ = 0x00,
} bmi088_gyro_odr_bwp;

/**
 * @brief Initializes the BMI088.
 */
void bmi088_init(void);

/**
 * @brief Sets the maximum measurement range.
 *
 * @param acc_range Accelerometer maximum measurement range.
 * @param gyro_range Gyro maximum measurement range.
 */
void bmi088_set_range(bmi088_acc_range_t acc_range, bmi088_gyro_range_t gyro_range);

/**
 * @brief Sets the output data rate (ODR) and low-pass filter bandwidth parameter (BWP).
 *
 * @param acc_odr Accelerometer ODR.
 * @param acc_bwp Accelerometer BWP.
 * @param gyro_odr_bwp Gyro ODR and BWP.
 */
void bmi088_set_odr_bwp(bmi088_acc_odr_t acc_odr, bmi088_acc_bwp_t acc_bwp,
                        bmi088_gyro_odr_bwp gyro_odr_bwp);

/**
 * @brief Reads the accelerometer measurement.
 *
 * @param[out] acc Acceleration in x, y, and z axes (m/s^2).
 */
void bmi088_read_acc(float acc[3]);

/**
 * @brief Reads the gyro measurement.
 *
 * @param[out] ang Angular velocity in x, y, and z axes (rad/s).
 */
void bmi088_read_gyro(float ang[3]);

#endif

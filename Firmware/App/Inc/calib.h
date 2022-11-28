/**
 * @file calib.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Sensor measurement calibration.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef CALIB_H
#define CALIB_H

/**
 * @brief Calibrates the accelerometer measurement.
 *
 * @param[in] acc_raw Raw acceleration (m/s^2).
 * @param[out] acc Calibrated acceleration (m/s^2).
 */
void calib_acc(const float acc_raw[3], float acc[3]);

/**
 * @brief Calibrates the gyro measurement.
 *
 * @param[in] ang_raw Raw angular velocity (rad/s).
 * @param[out] ang Calibrated angular velocity (rad/s).
 */
void calib_gyro(const float ang_raw[3], float ang[3]);

/**
 * @brief Calibrates the magnetometer measurement.
 *
 * @param[in] mag_raw Raw magnetic field (mG).
 * @param[out] mag Calibrated magnetic field (mG).
 */
void calib_mag(const float mag_raw[3], float mag[3]);

#endif

#ifndef AHRS_H
#define AHRS_H

#include <stdint.h>

/**
 * @brief AHRS callback function to measure the acceleration, angular velocity, and magnetic field with the sensor.
 *
 * @param sensor Sensor driver structure.
 * @param[out] acc Acceleration (m/s^2).
 * @param[out] ang Angular velocity (rad/s).
 * @param[out] mag Magnetic field (uT).
 */
typedef void (*ahrs_measure_t)(void *sensor, float acc[3], float ang[3], float mag[3]);

/**
 * @brief AHRS structure.
 */
typedef struct {
    float dt;

    void *sensor;
    ahrs_measure_t measure;

    float ang_stddev;

    float q[4], rpy[3];
    float q_meas[4], rpy_meas[3];
    float P[16], Q[16], R[36];
} ahrs_t;

/**
 * @brief Initializes the AHRS.
 *
 * @param ahrs AHRS structure.
 * @param dt Sampling time (s).
 * @param sensor Sensor driver structure.
 * @param measure Callback function to measure the acceleration, angular velocity, and magnetic field.
 */
void ahrs_init(ahrs_t *ahrs, float dt, void *sensor, ahrs_measure_t measure);

/**
 * @brief Sets the standard deviations used in the system model of the AHRS.
 *
 * @param ahrs AHRS structure.
 * @param init_att_stddev Standard deviation of the initial attitude.
 * @param norm_acc_stddev Standard deviation of the normalized acceleration.
 * @param ang_stddev Standard deviation of the angular velocity (rad/s).
 * @param norm_mag_stddev Standard deviation of the normalized magnetic field (uT).
 */
void ahrs_set_stddev(ahrs_t *ahrs, float init_att_stddev, float norm_acc_stddev, float ang_stddev,
                     float norm_mag_stddev);

/**
 * @brief Calculates the initial attitude.
 *
 * @param ahrs AHRS structure.
 * @param num_sample Number of samples from the accelerometer and magnetomerter to calculate the initial attitude.
 */
void ahrs_init_attitude(ahrs_t *ahrs, uint32_t num_sample);

/**
 * @brief Updates the attitude from the measurements.
 */
void ahrs_update(ahrs_t *ahrs);

#endif

/**
 * @file application.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Application main.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#define GYRO_HSPI hspi1
#define ACC_HSPI hspi1
#define MAG_HSPI hspi1
#define BARO_HSPI hspi1

#define CONTROL_HTIM htim6

/**
 * @brief Initializes the program.
 */
void setup(void);

/**
 * @brief Program main loop.
 */
void loop(void);

#endif

/**
 * @file config.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Configuration parameters.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef CONFIG_H
#define CONFIG_H

///@{
/** Gyro bias. */
#define GYRO_BIAS_X ( 4.31881460e-03f)
#define GYRO_BIAS_Y (-3.06885352e-03f)
#define GYRO_BIAS_Z (-6.71779391e-04f)
///@}

/** Heuristic drift reduction increment. */
#define GYRO_HDR_INC 0.0001f

///@{
/** Magnetometer soft iron calibration matrix. */
#define MAG_SOFT_IRON_XX ( 9.94427990e-01f)
#define MAG_SOFT_IRON_XY ( 1.83568371e-02f)
#define MAG_SOFT_IRON_XZ ( 6.36170031e-04f)
#define MAG_SOFT_IRON_YY ( 1.00756510e+00f)
#define MAG_SOFT_IRON_YZ (-1.39079159e-02f)
#define MAG_SOFT_IRON_ZZ ( 9.98581413e-01f)
///@}

///@{
/** Magnetometer hard iron calibration vector. */
#define MAG_HARD_IRON_X (-498.86403906f)
#define MAG_HARD_IRON_Y (-609.94047391f)
#define MAG_HARD_IRON_Z ( 243.05037387f)
///@}

#endif

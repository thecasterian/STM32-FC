/**
 * @file util.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Utility functions.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

/**
 * @brief Packs two bytes into a 16-bit integer.
 *
 * @param B1 Most significant byte.
 * @param B0 Least significant byte.
 */
#define PACK_2(B1, B0) (((B1) << 8) | (B0))

int16_t to_int16(uint16_t x);

#endif

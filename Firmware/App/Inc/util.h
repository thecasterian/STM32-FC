/**
 * @file util.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Utility functions.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdint.h>

uint16_t pack2(uint8_t b1, uint8_t b0);
int16_t to_int16(uint16_t x);

bool is_boolean_u8(uint8_t x);

void memcpy_to_volatile(volatile uint8_t *dst, const uint8_t *src, uint16_t size);
void memcpy_from_volatile(uint8_t *dst, const volatile uint8_t *src, uint16_t size);

float norm2(float x, float y);

#endif

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

/**
 * @brief Function to send bytes to a communication channel.
 *
 * @param buf Buffer for bytes to send.
 * @param size Required size to send.
 */
typedef void (*channel_send_t)(uint8_t *buf, uint16_t size);

/**
 * @brief Function to receive bytes from a communication channel.
 *
 * @param buf Buffer for received bytes.
 * @param size Required size to receive.
 *
 * @return true if the required size of bytes are received, false otherwise.
 */
typedef bool (*channel_receive_t)(uint8_t *buf, uint16_t size);

uint16_t pack2(uint8_t b1, uint8_t b0);
uint8_t byte0(uint16_t x);
uint8_t byte1(uint16_t x);

int16_t to_int16(uint16_t x);

bool is_boolean_u8(uint8_t x);

void memcpy_to_volatile(volatile uint8_t *dst, const uint8_t *src, uint16_t size);
void memcpy_from_volatile(uint8_t *dst, const volatile uint8_t *src, uint16_t size);

float norm2(float x, float y);
float norm3(float x, float y, float z);

#endif

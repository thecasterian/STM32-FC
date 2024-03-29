#include <math.h>
#include "util.h"

uint16_t pack2(uint8_t b1, uint8_t b0) {
    return (b1 << 8) | b0;
}

uint8_t byte0(uint16_t x) {
    return x & 0xFFU;
}

uint8_t byte1(uint16_t x) {
    return x >> 8;
}

int16_t to_int16(uint16_t x) {
    return (int16_t)x;
}

bool is_boolean_u8(uint8_t x) {
    return (x == 0U) || (x == 1U);
}

void memcpy_to_volatile(volatile uint8_t *dst, const uint8_t *src, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }
}

void memcpy_from_volatile(uint8_t *dst, const volatile uint8_t *src, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }
}

float norm2(float x, float y) {
    return sqrtf((x * x) + (y * y));
}

float norm3(float x, float y, float z) {
    return sqrtf((x * x) + (y * y) + (z * z));
}

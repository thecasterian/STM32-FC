#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define PACK_2(B1, B0) (((B1) << 8) | (B0))

int16_t to_int16(uint16_t x);

#endif

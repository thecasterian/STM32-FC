/**
 * @file ring_buffer.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Rx ring buffer.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

void usb_ringbuf_push(uint8_t *buf, uint16_t size);
bool usb_ringbuf_pop(uint8_t *buf, uint16_t size);

void sbus_ringbuf_push(uint8_t *buf, uint16_t size);
bool sbus_ringbuf_pop(uint8_t *buf, uint16_t size);

#endif

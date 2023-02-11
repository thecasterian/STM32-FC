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

#define RING_BUFFER_SIZE 256

typedef struct {
    uint16_t head;                              /* Head of the queue. */
    volatile uint16_t tail;                     /* Tail of the queue. Newest byte is written here. */
    volatile uint8_t buf[RING_BUFFER_SIZE];
} ring_buffer_t;

/**
 * @brief Pushes bytes to the ring buffer.
 *
 * @param ringbuf Ring buffer.
 * @param buf Buffer for bytes to push.
 * @param size Required size to push.
 */
void ring_buffer_push(ring_buffer_t *ringbuf, uint8_t *buf, uint16_t size);

/**
 * @brief Pops bytes from the ring buffer.
 *
 * @param ringbuf Ring buffer.
 * @param buf Buffer for received bytes.
 * @param size Required size to pop.
 * @return true if succeeded to pop, false otherwise.
 */
bool ring_buffer_pop(ring_buffer_t *ringbuf, uint8_t *buf, uint16_t size);

#endif

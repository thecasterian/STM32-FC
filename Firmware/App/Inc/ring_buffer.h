#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

void usb_ringbuf_push(uint8_t *buf, uint16_t size);
bool usb_ringbuf_pop(uint8_t *buf, uint16_t size);

#endif

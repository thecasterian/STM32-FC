#include <string.h>
#include "ring_buffer.h"
#include "util.h"

#define RING_BUFFER_SIZE 256

typedef struct {
    uint16_t head;                              /* Head of the queue. */
    volatile uint16_t tail;                     /* Tail of the queue. Newest byte is written here. */
    volatile uint8_t buf[RING_BUFFER_SIZE];
} ringbuf_t;

static ringbuf_t usb_ringbuf;
static ringbuf_t sbus_ringbuf;

static void ringbuf_push(ringbuf_t *ringbuf, uint8_t *buf, uint16_t size);
static bool ringbuf_pop(ringbuf_t *ringbuf, uint8_t *buf, uint16_t size);

void usb_ringbuf_push(uint8_t *buf, uint16_t size) {
    ringbuf_push(&usb_ringbuf, buf, size);
}

bool usb_ringbuf_pop(uint8_t *buf, uint16_t size) {
    return ringbuf_pop(&usb_ringbuf, buf, size);
}

void sbus_ringbuf_push(uint8_t *buf, uint16_t size) {
    ringbuf_push(&sbus_ringbuf, buf, size);
}

bool sbus_ringbuf_pop(uint8_t *buf, uint16_t size) {
    return ringbuf_pop(&sbus_ringbuf, buf, size);
}

static void ringbuf_push(ringbuf_t *ringbuf, uint8_t *buf, uint16_t size) {
    if ((ringbuf->tail + size) < RING_BUFFER_SIZE) {
        memcpy_to_volatile(&ringbuf->buf[ringbuf->tail], &buf[0], size);
        ringbuf->tail += size;
    } else {
        uint16_t size1 = RING_BUFFER_SIZE - ringbuf->tail;
        uint16_t size2 = size - size1;
        memcpy_to_volatile(&ringbuf->buf[ringbuf->tail], &buf[0], size1);
        memcpy_to_volatile(&ringbuf->buf[0], &buf[size1], size2);
        ringbuf->tail = size2;
    }
}

static bool ringbuf_pop(ringbuf_t *ringbuf, uint8_t *buf, uint16_t size) {
    bool success;

    if (ringbuf->head <= ringbuf->tail) {
        if ((ringbuf->tail - ringbuf->head) < size) {
            success = false;
        } else {
            memcpy_from_volatile(&buf[0], &ringbuf->buf[ringbuf->head], size);
            ringbuf->head += size;
            success = true;
        }
    } else {
        if ((RING_BUFFER_SIZE - ringbuf->head + ringbuf->tail) < size) {
            success = false;
        } else if ((ringbuf->head + size) < RING_BUFFER_SIZE) {
            memcpy_from_volatile(&buf[0], &ringbuf->buf[ringbuf->head], size);
            ringbuf->head += size;
            success = true;
        } else {
            uint16_t size1 = RING_BUFFER_SIZE - ringbuf->head;
            uint16_t size2 = size - size1;
            memcpy_from_volatile(&buf[0], &ringbuf->buf[ringbuf->head], size1);
            memcpy_from_volatile(&buf[size1], &ringbuf->buf[0], size2);
            ringbuf->head = size2;
            success = true;
        }
    }

    return success;
}

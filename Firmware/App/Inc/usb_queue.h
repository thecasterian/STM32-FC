#ifndef USB_QUEUE_H
#define USB_QUEUE_H

#include <stdbool.h>
#include <stdint.h>

#define USB_QUEUE_BUF_SIZE 256

typedef struct {
    uint16_t head;                              /* Head of the queue. */
    uint16_t tail;                              /* Tail of the queue. Newest byte is written here. */
    uint8_t buf[USB_QUEUE_BUF_SIZE];
} usb_queue_t;

extern usb_queue_t usb_queue;

void usb_queue_push(usb_queue_t *queue, uint8_t *buf, uint16_t size);
void usb_queue_pop(usb_queue_t *queue, uint8_t *buf, uint16_t size, bool *success);

#endif

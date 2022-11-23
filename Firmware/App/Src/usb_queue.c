#include <string.h>
#include "usb_queue.h"

usb_queue_t usb_queue;

void usb_queue_push(usb_queue_t *queue, uint8_t *buf, uint16_t size) {
    if ((queue->tail + size) < USB_QUEUE_BUF_SIZE) {
        memcpy(&queue->buf[queue->tail], &buf[0], size);
        queue->tail += size;
    } else {
        uint16_t size1 = USB_QUEUE_BUF_SIZE - queue->tail;
        uint16_t size2 = size - size1;
        memcpy(&queue->buf[queue->tail], &buf[0], size1);
        memcpy(&queue->buf[0], &buf[size1], size2);
        queue->tail = size2;
    }
}

void usb_queue_pop(usb_queue_t *queue, uint8_t *buf, uint16_t size, bool *success) {
    if (queue->head <= queue->tail) {
        if ((queue->tail - queue->head) < size) {
            *success = false;
        } else {
            memcpy(&buf[0], &queue->buf[queue->head], size);
            queue->head += size;
            *success = true;
        }
    } else {
        if ((USB_QUEUE_BUF_SIZE - queue->head + queue->tail) < size) {
            *success = false;
        } else if ((queue->head + size) < USB_QUEUE_BUF_SIZE) {
            memcpy(&buf[0], &queue->buf[queue->head], size);
            queue->head += size;
            *success = true;
        } else {
            uint16_t size1 = USB_QUEUE_BUF_SIZE - queue->head;
            uint16_t size2 = size - size1;
            memcpy(&buf[0], &queue->buf[queue->head], size1);
            memcpy(&buf[size1], &queue->buf[0], size2);
            queue->head = size2;
            *success = true;
        }
    }
}

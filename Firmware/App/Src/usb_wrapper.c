#include "usb_wrapper.h"
#include "usbd_cdc_if.h"

ring_buffer_t usb_rx_ringbuf;

void usb_send(uint8_t *buf, uint16_t size) {
    CDC_Transmit_FS(buf, size);
}

bool usb_receive(uint8_t *buf, uint16_t size) {
    return ring_buffer_pop(&usb_rx_ringbuf, buf, size);
}

#include "usb_wrapper.h"
#include "usbd_cdc_if.h"

void usb_transmit(uint8_t *buf, uint16_t len) {
    CDC_Transmit_FS(buf, len);
}

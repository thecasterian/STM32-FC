/**
 * @file usb_wrapper.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief STM32 USB communication wrapper.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef USB_WRAPPER_H
#define USB_WRAPPER_H

#include "ring_buffer.h"

extern ring_buffer_t usb_rx_ringbuf;

void usb_send(uint8_t *buf, uint16_t size);
bool usb_receive(uint8_t *buf, uint16_t size);

#endif

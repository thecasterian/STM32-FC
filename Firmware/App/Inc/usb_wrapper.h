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

/**
 * @brief Ring buffer for USB Rx.
 */
extern ring_buffer_t usb_rx_ringbuf;

/**
 * @brief Sends bytes through USB.
 *
 * @param buf Buffer for bytes to send.
 * @param size Required size to send.
 */
void usb_send(uint8_t *buf, uint16_t size);

/**
 * @brief Receives bytes through USB.
 *
 * @param buf Buffer for bytes to receive.
 * @param size Required size to receive.
 * @return true if succeeded to receive, false otherwise.
 */
bool usb_receive(uint8_t *buf, uint16_t size);

#endif

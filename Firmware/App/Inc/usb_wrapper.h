/**
 * @file usb_wrapper.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief STM32 USB communication wrapper.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef USB_WRAPPER_H
#define USB_WRAPPER_H

#include <stdint.h>

void usb_transmit(uint8_t *buf, uint16_t len);

#endif

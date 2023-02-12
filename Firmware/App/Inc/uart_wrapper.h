#ifndef UART_WRAPPER_H
#define UART_WRAPPER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes UART1
 */
void uart1_init(void);

/**
 * @brief Receives bytes through UART1.
 *
 * @param buf Buffer for bytes to receive.
 * @param size Required size to receive.
 * @return true if succeeded to receive, false otherwise.
 */
bool uart1_receive(uint8_t *buf, uint16_t size);

#endif

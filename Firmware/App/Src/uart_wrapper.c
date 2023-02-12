#include "ring_buffer.h"
#include "uart_wrapper.h"
#include "usart.h"

static uint8_t sbus_rx[1];
static ring_buffer_t uart1_rx_ringbuf;

void uart1_init(void) {
    HAL_UART_Receive_IT(&huart1, sbus_rx, sizeof(sbus_rx));
}

bool uart1_receive(uint8_t *buf, uint16_t size) {
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE) || __HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) ||
        __HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) || __HAL_UART_GET_FLAG(&huart1, UART_FLAG_PE)) {
        __disable_irq();
        HAL_UART_DeInit(&huart1);
        HAL_UART_Init(&huart1);
        __enable_irq();
    }

    return ring_buffer_pop(&uart1_rx_ringbuf, buf, size);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        ring_buffer_push(&uart1_rx_ringbuf, sbus_rx, sizeof(sbus_rx));
        HAL_UART_Receive_IT(&huart1, sbus_rx, sizeof(sbus_rx));
    }
}

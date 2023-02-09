#include <string.h>
#include "led.h"
#include "ring_buffer.h"
#include "sbus.h"
#include "tim_wrapper.h"
#include "usart.h"

#define SBUS_STX ((uint8_t)(0x0F))
#define SBUS_ETX ((uint8_t)(0x00))

#define SBUS_CH_DAT_LEN 22U

#define SBUS_EPS 16

static struct {
    sbus_packet_t packet;

    bool stx;                                               /* Is STX found? */
    uint16_t cnt;                                           /* Number of received bytes. */
    uint8_t ch_dat[SBUS_CH_DAT_LEN];                        /* Channel data. */

    tim_tick_t tick;                                        /* Tick of last received packet. */
} parser;

static uint8_t sbus_rx[1];

static bool sbus_ch_is_min(uint16_t ch_val);
static bool sbus_ch_is_max(uint16_t ch_val);

void sbus_init(void) {
    HAL_UART_Receive_IT(&huart1, sbus_rx, sizeof(sbus_rx));
}

bool sbus_packet_receive(sbus_packet_t *packet) {
    uint8_t c;
    bool res;
    bool success;
    static uint16_t receive_cnt;

    res = false;

    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE) || __HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) ||
        __HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) || __HAL_UART_GET_FLAG(&huart1, UART_FLAG_PE)) {
        __disable_irq();
        HAL_UART_DeInit(&huart1);
        HAL_UART_Init(&huart1);
        __enable_irq();
    }

    if (!parser.stx) {
        /* Find STX. */
        while (true) {
            success = sbus_ringbuf_pop(&c, 1U);
            if (!success || (c == SBUS_STX)) {
                break;
            }
        }

        if (success && (c == SBUS_STX)) {
            /* STX is found. */
            parser.stx = true;
            parser.cnt++;
        } else {
            parser.stx = false;
            parser.cnt = 0U;
        }
    }

    if (parser.stx) {
        while (true) {
            success = sbus_ringbuf_pop(&c, 1U);
            if (!success) {
                break;
            }

            if (parser.cnt <= SBUS_CH_DAT_LEN) {
                /* Push to the channel data array. */
                parser.ch_dat[parser.cnt - 1U] = c;
            } else if (parser.cnt == SBUS_CH_DAT_LEN + 1U) {
                /* Flag byte. */
                parser.packet.flg = c;
            } else {
                /* Last byte is ETX. */
                if (c != SBUS_ETX) {
                    /* ETX is not found. */
                    parser.stx = false;
                    parser.cnt = 0U;
                    break;
                }

                res = true;
                parser.stx = false;
                parser.cnt = 0U;

                /* Parse channel data. */
                parser.packet.ch[0] = parser.ch_dat[0] | ((parser.ch_dat[1] & 0x07) << 8);
                parser.packet.ch[1] = (parser.ch_dat[1] >> 3) | ((parser.ch_dat[2] & 0x3F) << 5);
                parser.packet.ch[2] = (parser.ch_dat[2] >> 6) | (parser.ch_dat[3] << 2) |
                                      ((parser.ch_dat[4] & 0x01) << 10);
                parser.packet.ch[3] = (parser.ch_dat[4] >> 1) | ((parser.ch_dat[5] & 0x0F) << 7);
                parser.packet.ch[4] = (parser.ch_dat[5] >> 4) | ((parser.ch_dat[6] & 0x7F) << 4);
                parser.packet.ch[5] = (parser.ch_dat[6] >> 7) | (parser.ch_dat[7] << 1) |
                                      ((parser.ch_dat[8] & 0x03) << 9);
                parser.packet.ch[6] = (parser.ch_dat[8] >> 2) | ((parser.ch_dat[9] & 0x1F) << 6);
                parser.packet.ch[7] = (parser.ch_dat[9] >> 5) | (parser.ch_dat[10] << 3);
                parser.packet.ch[8] = parser.ch_dat[11] | ((parser.ch_dat[12] & 0x07) << 8);
                parser.packet.ch[9] = (parser.ch_dat[12] >> 3) | ((parser.ch_dat[13] & 0x3F) << 5);
                parser.packet.ch[10] = (parser.ch_dat[13] >> 6) | (parser.ch_dat[14] << 2) |
                                       ((parser.ch_dat[15] & 0x01) << 10);
                parser.packet.ch[11] = (parser.ch_dat[15] >> 1) | ((parser.ch_dat[16] & 0x0F) << 7);
                parser.packet.ch[12] = (parser.ch_dat[16] >> 4) | ((parser.ch_dat[17] & 0x7F) << 4);
                parser.packet.ch[13] = (parser.ch_dat[17] >> 7) | (parser.ch_dat[18] << 1) |
                                       ((parser.ch_dat[19] & 0x03) << 9);
                parser.packet.ch[14] = (parser.ch_dat[19] >> 2) | ((parser.ch_dat[20] & 0x1F) << 6);
                parser.packet.ch[15] = (parser.ch_dat[20] >> 5) | (parser.ch_dat[21] << 3);

                /* Copy packet. */
                memcpy(packet, &parser.packet, sizeof(*packet));

                parser.tick = control_timer_get_tick();

                if (receive_cnt == 0U) {
                    led_blue_toggle();
                }
                receive_cnt++;
                if (receive_cnt >= 50U) {
                    receive_cnt = 0U;
                }

                break;
            }

            parser.cnt++;
        }
    }

    return res;
}

bool sbus_is_timeout(void) {
    return (control_timer_get_tick() - parser.tick) >= SBUS_TIMEOUT_TICK;
}

bool sbus_packet_is_arming_pos(const sbus_packet_t *packet) {
    static uint16_t arming_start_tick;
    bool res;

    if (!(sbus_ch_is_min(packet->ch[SBUS_THROTTLE_CH]) && sbus_ch_is_min(packet->ch[SBUS_RUDDER_CH]) &&
          sbus_ch_is_min(packet->ch[SBUS_ELEVATOR_CH]) && sbus_ch_is_max(packet->ch[SBUS_AILERON_CH]))) {
        arming_start_tick = control_timer_get_tick();
        res = false;
    } else {
        res = (control_timer_get_tick() - arming_start_tick) >= SBUS_ARMING_TICK;
    }

    return res;
}

bool sbus_packet_is_disarming_pos(const sbus_packet_t *packet) {
    static uint16_t disarming_start_tick;
    bool res;

    if (!(sbus_ch_is_min(packet->ch[SBUS_THROTTLE_CH]) && sbus_ch_is_max(packet->ch[SBUS_RUDDER_CH]) &&
          sbus_ch_is_min(packet->ch[SBUS_ELEVATOR_CH]) && sbus_ch_is_min(packet->ch[SBUS_AILERON_CH]))) {
        disarming_start_tick = control_timer_get_tick();
        res = false;
    } else {
        res = (control_timer_get_tick() - disarming_start_tick) >= SBUS_DISARMING_TICK;
    }

    return res;
}

bool sbus_packet_is_emergency_switch_on(const sbus_packet_t *packet) {
    return sbus_ch_is_max(packet->ch[SBUS_SH_CH]);
}

float sbus_ch_map_range(uint16_t ch_val, float min, float max) {
    float val;

    val = ((float)ch_val - SBUS_MIN) / (SBUS_MAX - SBUS_MIN);
    val = (val * (max - min)) + min;

    return val;
}

static bool sbus_ch_is_min(uint16_t ch_val) {
    return ch_val < (SBUS_MIN + SBUS_EPS);
}

static bool sbus_ch_is_max(uint16_t ch_val) {
    return ch_val > (SBUS_MAX - SBUS_EPS);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        sbus_ringbuf_push(sbus_rx, sizeof(sbus_rx));
        HAL_UART_Receive_IT(&huart1, sbus_rx, sizeof(sbus_rx));
    }
}

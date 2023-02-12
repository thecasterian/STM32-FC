#include <string.h>
#include "led.h"
#include "ring_buffer.h"
#include "sbus.h"
#include "tim_wrapper.h"
#include "usart.h"

#define SBUS_STX ((uint8_t)(0x0F))
#define SBUS_ETX ((uint8_t)(0x00))

#define SBUS_MIN 172
#define SBUS_MAX 1811
#define SBUS_EPS 16

#define SBUS_TIMEOUT_TICK   100U
#define SBUS_ARMING_TICK    2000U
#define SBUS_DISARMING_TICK 2000U

static bool sbus_ch_is_min(uint16_t ch_val);
static bool sbus_ch_is_max(uint16_t ch_val);

void sbus_channel_init(sbus_channel_t *sbus_ch, channel_receive_t receive) {
    sbus_ch->receive = receive;
    sbus_ch->stx = false;
    sbus_ch->cnt = 0U;
}

bool sbus_channel_receive(sbus_channel_t *sbus_ch, sbus_packet_t *packet) {
    uint8_t c;
    bool res;
    bool success;
    static uint16_t receive_cnt;

    res = false;

    if (!sbus_ch->stx) {
        /* Find STX. */
        while (true) {
            success = sbus_ch->receive(&c, 1U);
            if (!success || (c == SBUS_STX)) {
                break;
            }
        }

        if (success && (c == SBUS_STX)) {
            /* STX is found. */
            sbus_ch->stx = true;
            sbus_ch->cnt++;
        } else {
            sbus_ch->stx = false;
            sbus_ch->cnt = 0U;
        }
    }

    if (sbus_ch->stx) {
        while (true) {
            success = sbus_ch->receive(&c, 1U);
            if (!success) {
                break;
            }

            if (sbus_ch->cnt <= SBUS_CH_DAT_LEN) {
                /* Push to the channel data array. */
                sbus_ch->ch_dat[sbus_ch->cnt - 1U] = c;
            } else if (sbus_ch->cnt == SBUS_CH_DAT_LEN + 1U) {
                /* Flag byte. */
                sbus_ch->packet.flg = c;
            } else {
                /* Last byte is ETX. */
                if (c != SBUS_ETX) {
                    /* ETX is not found. */
                    sbus_ch->stx = false;
                    sbus_ch->cnt = 0U;
                    break;
                }

                res = true;
                sbus_ch->stx = false;
                sbus_ch->cnt = 0U;

                /* Parse channel data. */
                sbus_ch->packet.ch[0] = sbus_ch->ch_dat[0] | ((sbus_ch->ch_dat[1] & 0x07) << 8);
                sbus_ch->packet.ch[1] = (sbus_ch->ch_dat[1] >> 3) | ((sbus_ch->ch_dat[2] & 0x3F) << 5);
                sbus_ch->packet.ch[2] = (sbus_ch->ch_dat[2] >> 6) | (sbus_ch->ch_dat[3] << 2) |
                                      ((sbus_ch->ch_dat[4] & 0x01) << 10);
                sbus_ch->packet.ch[3] = (sbus_ch->ch_dat[4] >> 1) | ((sbus_ch->ch_dat[5] & 0x0F) << 7);
                sbus_ch->packet.ch[4] = (sbus_ch->ch_dat[5] >> 4) | ((sbus_ch->ch_dat[6] & 0x7F) << 4);
                sbus_ch->packet.ch[5] = (sbus_ch->ch_dat[6] >> 7) | (sbus_ch->ch_dat[7] << 1) |
                                      ((sbus_ch->ch_dat[8] & 0x03) << 9);
                sbus_ch->packet.ch[6] = (sbus_ch->ch_dat[8] >> 2) | ((sbus_ch->ch_dat[9] & 0x1F) << 6);
                sbus_ch->packet.ch[7] = (sbus_ch->ch_dat[9] >> 5) | (sbus_ch->ch_dat[10] << 3);
                sbus_ch->packet.ch[8] = sbus_ch->ch_dat[11] | ((sbus_ch->ch_dat[12] & 0x07) << 8);
                sbus_ch->packet.ch[9] = (sbus_ch->ch_dat[12] >> 3) | ((sbus_ch->ch_dat[13] & 0x3F) << 5);
                sbus_ch->packet.ch[10] = (sbus_ch->ch_dat[13] >> 6) | (sbus_ch->ch_dat[14] << 2) |
                                       ((sbus_ch->ch_dat[15] & 0x01) << 10);
                sbus_ch->packet.ch[11] = (sbus_ch->ch_dat[15] >> 1) | ((sbus_ch->ch_dat[16] & 0x0F) << 7);
                sbus_ch->packet.ch[12] = (sbus_ch->ch_dat[16] >> 4) | ((sbus_ch->ch_dat[17] & 0x7F) << 4);
                sbus_ch->packet.ch[13] = (sbus_ch->ch_dat[17] >> 7) | (sbus_ch->ch_dat[18] << 1) |
                                       ((sbus_ch->ch_dat[19] & 0x03) << 9);
                sbus_ch->packet.ch[14] = (sbus_ch->ch_dat[19] >> 2) | ((sbus_ch->ch_dat[20] & 0x1F) << 6);
                sbus_ch->packet.ch[15] = (sbus_ch->ch_dat[20] >> 5) | (sbus_ch->ch_dat[21] << 3);

                /* Copy packet. */
                memcpy(packet, &sbus_ch->packet, sizeof(*packet));

                sbus_ch->tick = control_timer_get_tick();

                if (receive_cnt == 0U) {
                    led_blue_toggle();
                }
                receive_cnt++;
                if (receive_cnt >= 50U) {
                    receive_cnt = 0U;
                }

                break;
            }

            sbus_ch->cnt++;
        }
    }

    return res;
}

bool sbus_channel_is_timeout(sbus_channel_t *sbus_ch) {
    return (control_timer_get_tick() - sbus_ch->tick) >= SBUS_TIMEOUT_TICK;
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

float sbus_packet_map_range(uint16_t ch_val, float min, float max) {
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

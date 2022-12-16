#include <math.h>
#include "esc.h"

#define PWM_FREQ_STD_PWM    250.f
#define PWM_FREQ_ONESHOT125 2000.f
#define PWM_FREQ_ONESHOT42  6000.f
#define PWM_FREQ_DSHOT150   150000.f
#define PWM_FREQ_DSHOT300   300000.f
#define PWM_FREQ_DSHOT600   600000.f

#define PULSE_WIDTH_MIN_STD_PWM    1000.f
#define PULSE_WIDTH_MAX_STD_PWM    2000.f
#define PULSE_WIDTH_MIN_ONESHOT125 125.f
#define PULSE_WIDTH_MAX_ONESHOT125 250.f
#define PULSE_WIDTH_MIN_ONESHOT42  42.f
#define PULSE_WIDTH_MAX_ONESHOT42  84.f
#define PULSE_WIDTH_ZERO_DSHOT150  2.50f
#define PULSE_WIDTH_ONE_DSHOT150   5.00f
#define PULSE_WIDTH_ZERO_DSHOT300  1.25f
#define PULSE_WIDTH_ONE_DSHOT300   2.50f
#define PULSE_WIDTH_ZERO_DSHOT600  0.625f
#define PULSE_WIDTH_ONE_DSHOT600   1.25f

#define DSHOT_THROTTLE_MIN 48
#define DSHOT_THROTTLE_MAX 2047

static struct {
    pwm_channel_t mapping[4];
    esc_protocol_t protocol;
    float throttle[4];
} esc_config;

static void esc_send_throttle_std_pwm(motor_position_t position, float throttle);
static void esc_send_throttle_oneshot125(motor_position_t position, float throttle);
static void esc_send_throttle_oneshot42(motor_position_t position, float throttle);
static void esc_send_throttle_dshot150(motor_position_t position, float throttle);
static void esc_send_throttle_dshot300(motor_position_t position, float throttle);
static void esc_send_throttle_dshot600(motor_position_t position, float throttle);

bool esc_set_motor_pwm_mapping(pwm_channel_t mapping[4]) {
    bool res = true;
    int16_t channel_count[4] = {0};

    for (int16_t i = 0; i < 4; i++) {
        if ((mapping[i] < 0) || (mapping[i] >= 4)) {
            res = false;
            break;
        }

        channel_count[mapping[i]]++;
    }

    for (int16_t i = 0; i < 4; i++) {
        if (channel_count[i] != 1) {
            res = false;
            break;
        }
    }

    if (res) {
        for (int16_t i = 0; i < 4; i++) {
            esc_config.mapping[i] = mapping[i];
        }
    }

    return res;
}

bool esc_set_protocol(esc_protocol_t protocol) {
    bool res = true;

    esc_config.protocol = protocol;

    switch (protocol) {
    case ESC_PRTCL_STD_PWM:
        pwm_set_frequency(PWM_FREQ_STD_PWM);
        break;
    case ESC_PRTCL_ONESHOT125:
        pwm_set_frequency(PWM_FREQ_ONESHOT125);
        break;
    case ESC_PRTCL_ONESHOT42:
        pwm_set_frequency(PWM_FREQ_ONESHOT42);
        break;
    case ESC_PRTCL_DSHOT150:
        pwm_set_frequency(PWM_FREQ_DSHOT150);
        break;
    case ESC_PRTCL_DSHOT300:
        pwm_set_frequency(PWM_FREQ_DSHOT300);
        break;
    case ESC_PRTCL_DSHOT600:
        pwm_set_frequency(PWM_FREQ_DSHOT600);
        break;
    default:
        res = false;
        break;
    }

    return res;
}

bool esc_set_throttle(motor_position_t position, float throttle) {
    bool res = true;

    if ((position < 0) || (position >= 4) || (throttle < 0.f) || (throttle > 1.f)) {
        res = false;
    } else {
        esc_config.throttle[position] = throttle;
    }

    return res;
}

void esc_send_throttle(void) {
    if (pwm_en) {
        for (motor_position_t i = 0; i < 4; i++) {
            switch (esc_config.protocol) {
            case ESC_PRTCL_STD_PWM:
                esc_send_throttle_std_pwm(i, esc_config.throttle[i]);
                break;
            case ESC_PRTCL_ONESHOT125:
                esc_send_throttle_oneshot125(i, esc_config.throttle[i]);
                break;
            case ESC_PRTCL_ONESHOT42:
                esc_send_throttle_oneshot42(i, esc_config.throttle[i]);
                break;
            case ESC_PRTCL_DSHOT150:
                esc_send_throttle_dshot150(i, esc_config.throttle[i]);
                break;
            case ESC_PRTCL_DSHOT300:
                esc_send_throttle_dshot300(i, esc_config.throttle[i]);
                break;
            case ESC_PRTCL_DSHOT600:
                esc_send_throttle_dshot600(i, esc_config.throttle[i]);
                break;
            default:
                break;
            }
        }
    }
}

static void esc_send_throttle_std_pwm(motor_position_t position, float throttle) {
    float pulse_width[1];

    pulse_width[0] = ((PULSE_WIDTH_MAX_STD_PWM - PULSE_WIDTH_MIN_STD_PWM) * throttle) + PULSE_WIDTH_MIN_STD_PWM;
    pwm_send_packet(esc_config.mapping[position], pulse_width, 1U);
}

static void esc_send_throttle_oneshot125(motor_position_t position, float throttle) {
    float pulse_width[1];

    pulse_width[0] = ((PULSE_WIDTH_MAX_ONESHOT125 - PULSE_WIDTH_MIN_ONESHOT125) * throttle)
                     + PULSE_WIDTH_MIN_ONESHOT125;
    pwm_send_packet(esc_config.mapping[position], pulse_width, 1U);
}

static void esc_send_throttle_oneshot42(motor_position_t position, float throttle) {
    float pulse_width[1];

    pulse_width[0] = ((PULSE_WIDTH_MAX_ONESHOT42 - PULSE_WIDTH_MIN_ONESHOT42) * throttle)
                     + PULSE_WIDTH_MIN_ONESHOT42;
    pwm_send_packet(esc_config.mapping[position], pulse_width, 1U);
}

static void esc_send_throttle_dshot150(motor_position_t position, float throttle) {
    uint16_t command, checksum;
    float pulse_width[16];

    command = lrintf(throttle * (DSHOT_THROTTLE_MAX - DSHOT_THROTTLE_MIN)) + DSHOT_THROTTLE_MIN;
    command <<= 1;

    checksum = command ^ (command >> 4) ^ (command >> 8);
    command = (command << 4) | checksum;

    for (int16_t i = 0; i < 16; i++) {
        pulse_width[i] = (command & (1 << (15 - i))) == 0 ? PULSE_WIDTH_ZERO_DSHOT150 : PULSE_WIDTH_ONE_DSHOT150;
    }
    pwm_send_packet(esc_config.mapping[position], pulse_width, 16U);

}

static void esc_send_throttle_dshot300(motor_position_t position, float throttle) {
    uint16_t command, checksum;
    float pulse_width[16];

    command = lrintf(throttle * (DSHOT_THROTTLE_MAX - DSHOT_THROTTLE_MIN)) + DSHOT_THROTTLE_MIN;
    command <<= 1;

    checksum = command ^ (command >> 4) ^ (command >> 8);
    command = (command << 4) | checksum;

    for (int16_t i = 0; i < 16; i++) {
        pulse_width[i] = (command & (1 << (15 - i))) == 0 ? PULSE_WIDTH_ZERO_DSHOT300 : PULSE_WIDTH_ONE_DSHOT300;
    }
    pwm_send_packet(esc_config.mapping[position], pulse_width, 16U);
}

static void esc_send_throttle_dshot600(motor_position_t position, float throttle) {
    uint16_t command, checksum;
    float pulse_width[16];

    command = lrintf(throttle * (DSHOT_THROTTLE_MAX - DSHOT_THROTTLE_MIN)) + DSHOT_THROTTLE_MIN;
    command <<= 1;

    checksum = command ^ (command >> 4) ^ (command >> 8);
    command = (command << 4) | checksum;

    for (int16_t i = 0; i < 16; i++) {
        pulse_width[i] = (command & (1 << (15 - i))) == 0 ? PULSE_WIDTH_ZERO_DSHOT600 : PULSE_WIDTH_ONE_DSHOT600;
    }
    pwm_send_packet(esc_config.mapping[position], pulse_width, 16U);
}

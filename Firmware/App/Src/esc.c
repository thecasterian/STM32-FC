#include <math.h>
#include "esc.h"

#define CONTROL_FREQ    1000.f
#define COMPARE_MAX     84000.f
#define PULSE_WIDTH_MIN 125.f
#define PULSE_WIDTH_MAX 250.f

static struct {
    pwm_channel_t mapping[4];
} esc_config;

bool esc_set_motor_pwm_mapping(const pwm_channel_t mapping[4]) {
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

bool esc_set_throttle(const float throttle[4]) {
    bool res = true;
    float width;
    uint32_t period[4];

    for (int16_t i = 0; i < 4; i++) {
        if ((throttle[i] < 0.f) || (throttle[i] > 1.f)) {
            res = false;
            break;
        }
    }

    if (res) {
        for (int16_t i = 0; i < 4; i++) {
            width = ((PULSE_WIDTH_MAX - PULSE_WIDTH_MIN) * throttle[i]) + PULSE_WIDTH_MIN;
            period[esc_config.mapping[i]] = lrintf(width * 1e-6f * CONTROL_FREQ * COMPARE_MAX);
        }
        pwm_set_period(period);
    }

    return res;
}

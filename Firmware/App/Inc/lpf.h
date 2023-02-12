#ifndef LPF_H
#define LPF_H

#include <stdint.h>

#define LPF_MAX_SIGNAL_NUM 3

/**
 * @brief Third-order Butterworth low-pass filter.
 */
typedef struct {
    /** Number of signals. */
    uint16_t nsignals;
    /** Transfer function numerator coefficients. */
    float n[4];
    /** Transfer function denominator coefficients. */
    float d[4];

    /** Inputs. */
    float x[4][LPF_MAX_SIGNAL_NUM];
    /** Outputs. */
    float y[4][LPF_MAX_SIGNAL_NUM];
} lpf3_t;

/**
 * @brief Initializes the third-order Butterworth low-pass filter.
 *
 * @param lpf Third-order Butterworth low-pass filter.
 * @param nsignals Number of signals.
 * @param dt Sampling time (s).
 * @param fc Cutoff frequency (Hz).
 */
void lpf3_init(lpf3_t *lpf, uint16_t nsignals, float dt, float fc);

/**
 * @brief Updates the filter.
 *
 * @param lpf Low pass filter.
 * @param[in] x Values of the input signals.
 * @param[out] y Values of the output signals.
 */
void lpf3_update(lpf3_t *lpf, const float x[], float y[]);

#endif

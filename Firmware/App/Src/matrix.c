#include <math.h>
#include <string.h>
#include "matrix.h"

#define MATRIX_MAX_SIZE 64

void matrix_zero(float *m, uint16_t r, uint16_t c) {
    for (uint16_t i = 0U; i < r * c; i++) {
        m[i] = 0.f;
    }
}

void matrix_transpose(const float *m, uint16_t r, uint16_t c, float *res) {
    float tmp[MATRIX_MAX_SIZE];

    for (uint16_t i = 0; i < r; i++) {
        for (uint16_t j = 0; j < c; j++) {
            tmp[j * r + i] = m[i * c + j];
        }
    }

    memcpy(res, tmp, r * c * sizeof(*res));
}

void matrix_cholesky(const float *m, uint16_t r, float *res) {
    float tmp[MATRIX_MAX_SIZE] = { 0.f };
    float s;

    for (uint16_t i = 0U; i < r; i++) {
        for (uint16_t j = 0U; j <= i; j++) {
            s = m[i * r + j];
            for (uint16_t k = 0U; k < j; k++) {
                s -= tmp[i * r + k] * tmp[j * r + k];
            }

            if (i == j) {
                if (s < 0.f) {
                    return;
                }
                tmp[i * r + j] = sqrtf(s);
            } else {
                tmp[i * r + j] = s / tmp[j * r + j];
            }
        }
    }

    memcpy(res, tmp, r * r * sizeof(*res));
}

void matrix_inv_sym(const float *m, uint16_t r, float *res) {
    float L[MATRIX_MAX_SIZE], L_inv[MATRIX_MAX_SIZE] = { 0.f }, tmp[MATRIX_MAX_SIZE];

    matrix_cholesky(m, r, L);

    for (uint16_t i = 0U; i < r; i++) {
        L_inv[i * r + i] = 1.f / L[i * r + i];
        for (uint16_t j = 0U; j < i; j++) {
            for (uint16_t k = j; k < i; k++) {
                L_inv[i * r + j] += L[i * r + k] * L_inv[k * r + j];
            }
            L_inv[i * r + j] = -L_inv[i * r + j] / L[i * r + i];
        }
    }

    matrix_transpose(L_inv, r, r, tmp);
    matrix_mul(tmp, L_inv, r, r, r, res);
}

void matrix_add_scalar(const float *m, float t, uint16_t r, uint16_t c, float *res) {
    for (uint16_t i = 0U; i < r; i++) {
        for (uint16_t j = 0U; j < c; j++) {
            res[i * c + j] = m[i * c + j] + t;
        }
    }
}

void matrix_add_eye(const float *m, float t, uint16_t r, float *res) {
    for (uint16_t i = 0U; i < r; i++) {
        for (uint16_t j = 0U; j < r; j++) {
            res[i * r + j] = (i == j) ? m[i * r + j] + t : m[i * r + j];
        }
    }
}

void matrix_mul_scalar(const float *m, float t, uint16_t r, uint16_t c, float *res) {
    for (uint16_t i = 0U; i < r; i++) {
        for (uint16_t j = 0U; j < c; j++) {
            res[i * c + j] = m[i * c + j] * t;
        }
    }
}

void matrix_add(const float *a, const float *b, uint16_t r, uint16_t c, float *res) {
    for (uint16_t i = 0U; i < r; i++) {
        for (uint16_t j = 0U; j < c; j++) {
            res[i * c + j] = a[i * c + j] + b[i * c + j];
        }
    }
}

void matrix_sub(const float *a, const float *b, uint16_t r, uint16_t c, float *res) {
    for (uint16_t i = 0U; i < r; i++) {
        for (uint16_t j = 0U; j < c; j++) {
            res[i * c + j] = a[i * c + j] - b[i * c + j];
        }
    }
}

void matrix_mul(const float *a, const float *b, uint16_t r, uint16_t c, uint16_t n, float *res) {
    float tmp[MATRIX_MAX_SIZE] = { 0.f };

    for (uint16_t i = 0U; i < r; i++) {
        for (uint16_t j = 0U; j < n; j++) {
            for (uint16_t k = 0U; k < c; k++) {
                tmp[i * n + j] += a[i * c + k] * b[k * n + j];
            }
        }
    }

    memcpy(res, tmp, r * n * sizeof(*res));
}

void matrix_get_upper_triangular(const float *m, uint16_t r, uint16_t c, float *res) {
    uint16_t cnt;

    cnt = 0U;
    for (uint16_t i = 0; i < r; i++) {
        for (uint16_t j = i; j < c; j++) {
            res[cnt] = m[i * c + j];
            cnt++;
        }
    }
}

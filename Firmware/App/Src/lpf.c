#include <string.h>
#include "const.h"
#include "lpf.h"

void lpf3_init(lpf3_t *lpf, uint16_t nsignals, float dt, float fc) {
    float k, k2, k3;

    lpf->nsignals = nsignals;

    k = 1.f / (PI * fc * dt);
    k2 = k * k;
    k3 = k2 * k;

    lpf->n[3] = 1.f;
    lpf->n[2] = 3.f;
    lpf->n[1] = 3.f;
    lpf->n[0] = 1.f;

    lpf->d[3] = k3 + 2.f * k2 + 2.f * k + 1.f;
    lpf->d[2] = -3.f * k3 - 2.f * k2 + 2.f * k + 3.f;
    lpf->d[1] = 3.f * k3 - 2.f * k2 - 2.f * k + 3.f;
    lpf->d[0] = -k3 + 2.f * k2 - 2.f * k + 1.f;
}

void lpf3_update(lpf3_t *lpf, const float x[], float y[]) {
    memcpy(lpf->x[0], x, sizeof(float) * lpf->nsignals);

    for (int16_t i = 0; i < lpf->nsignals; i++) {
        lpf->y[0][i] = (
            lpf->n[3] * lpf->x[0][i] + lpf->n[2] * lpf->x[1][i] + lpf->n[1] * lpf->x[2][i] + lpf->n[0] * lpf->x[3][i] -
            lpf->d[2] * lpf->y[1][i] - lpf->d[1] * lpf->y[2][i] - lpf->d[0] * lpf->y[3][i]
        ) / lpf->d[3];
    }

    memcpy(y, lpf->y[0], sizeof(float) * lpf->nsignals);

    for (int16_t i = 3; i > 0; i--) {
        memcpy(lpf->x[i], lpf->x[i - 1], sizeof(float) * lpf->nsignals);
        memcpy(lpf->y[i], lpf->y[i - 1], sizeof(float) * lpf->nsignals);
    }
}

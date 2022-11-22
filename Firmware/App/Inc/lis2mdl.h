#ifndef LIS2MDL_H
#define LIS2MDL_H

#include "spi.h"

/* LIS2MDL output data rate (ODR). */
typedef enum {
    LIS2MDL_ODR_10Hz  = 0x00,
    LIS2MDL_ODR_20Hz  = 0x04,
    LIS2MDL_ODR_50Hz  = 0x08,
    LIS2MDL_ODR_100Hz = 0x0C,
} lis2mdl_odr_t;

typedef struct {
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *nss_port;
    uint16_t nss_pin;

    uint8_t who_am_i;
} lis2mdl_t;

void lis2mdl_init(lis2mdl_t *lis2mdl, SPI_HandleTypeDef *hspi, GPIO_TypeDef *nss_port, uint16_t nss_pin);
void lis2mdl_set_odr(lis2mdl_t *lis2mdl, lis2mdl_odr_t odr);

void lis2mdl_read_mag(lis2mdl_t *lis2mdl, float *mag);

#endif

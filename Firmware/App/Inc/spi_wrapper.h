#ifndef SPI_WRAPPER_H
#define SPI_WRAPPER_H

#include <stdint.h>

void bmi088_read_acc_reg(uint8_t reg, uint8_t *data, uint16_t size);
void bmi088_write_acc_reg(uint8_t reg, uint8_t data);
void bmi088_read_gyro_reg(uint8_t reg, uint8_t *data, uint16_t size);
void bmi088_write_gyro_reg(uint8_t reg, uint8_t data);

void lis2mdl_read_reg(uint8_t reg, uint8_t *data, uint16_t size);
void lis2mdl_write_reg(uint8_t reg, uint8_t data);

void bmp280_read_reg(uint8_t reg, uint8_t *data, uint16_t size);
void bmp280_write_reg(uint8_t reg, uint8_t data);

#endif

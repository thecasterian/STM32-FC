#include "spi.h"
#include "spi_wrapper.h"

#define GYRO_HSPI hspi1
#define ACC_HSPI hspi1
#define MAG_HSPI hspi1
#define BARO_HSPI hspi1

#define FLAG_READ 0x80
#define MASK_WRITE 0x7F

void bmi088_read_acc_reg(uint8_t reg, uint8_t *data, uint16_t size) {
    uint8_t tx_buf[1] = {reg | FLAG_READ};
    uint8_t rx_buf[1] = {0};

    HAL_GPIO_WritePin(ACC_NSS_GPIO_Port, ACC_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ACC_HSPI, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(&ACC_HSPI, rx_buf, sizeof(rx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(&ACC_HSPI, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ACC_NSS_GPIO_Port, ACC_NSS_Pin, GPIO_PIN_SET);
}

void bmi088_write_acc_reg(uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg, data};

    HAL_GPIO_WritePin(ACC_NSS_GPIO_Port, ACC_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ACC_HSPI, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ACC_NSS_GPIO_Port, ACC_NSS_Pin, GPIO_PIN_SET);
}

void bmi088_read_gyro_reg(uint8_t reg, uint8_t *data, uint16_t size) {
    uint8_t tx_buf[1] = {reg | FLAG_READ};

    HAL_GPIO_WritePin(GYRO_NSS_GPIO_Port, GYRO_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&GYRO_HSPI, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(&GYRO_HSPI, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GYRO_NSS_GPIO_Port, GYRO_NSS_Pin, GPIO_PIN_SET);
}

void bmi088_write_gyro_reg(uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg, data};

    HAL_GPIO_WritePin(GYRO_NSS_GPIO_Port, GYRO_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&GYRO_HSPI, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GYRO_NSS_GPIO_Port, GYRO_NSS_Pin, GPIO_PIN_SET);
}

void lis2mdl_read_reg(uint8_t reg, uint8_t *data, uint16_t size) {
    uint8_t tx_buf[1] = {reg | FLAG_READ};

    HAL_GPIO_WritePin(MAG_NSS_GPIO_Port, MAG_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&MAG_HSPI, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(&MAG_HSPI, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(MAG_NSS_GPIO_Port, MAG_NSS_Pin, GPIO_PIN_SET);
}

void lis2mdl_write_reg(uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg, data};

    HAL_GPIO_WritePin(MAG_NSS_GPIO_Port, MAG_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&MAG_HSPI, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(MAG_NSS_GPIO_Port, MAG_NSS_Pin, GPIO_PIN_SET);
}

void bmp280_read_reg(uint8_t reg, uint8_t *data, uint16_t size) {
    uint8_t tx_buf[1] = {reg};

    HAL_GPIO_WritePin(BARO_NSS_GPIO_Port, BARO_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&BARO_HSPI, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(&BARO_HSPI, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(BARO_NSS_GPIO_Port, BARO_NSS_Pin, GPIO_PIN_SET);
}

void bmp280_write_reg(uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg & MASK_WRITE, data};

    HAL_GPIO_WritePin(BARO_NSS_GPIO_Port, BARO_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&BARO_HSPI, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(BARO_NSS_GPIO_Port, BARO_NSS_Pin, GPIO_PIN_SET);
}

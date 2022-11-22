#include "bmi088.h"
#include "config.h"
#include "const.h"
#include "util.h"

/* Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi088-ds001.pdf */

#define FLAG_READ 0x80

#define REG_ACC_CHIP_ID   0x00
#define REG_ACC_DATA      0x12
#define REG_ACC_CONF      0x40
#define REG_ACC_RANGE     0x41
#define REG_ACC_PWR_CONF  0x7C
#define REG_ACC_PWR_CTRL  0x7D
#define REG_ACC_SOFTRESET 0x7E

#define REG_GYRO_CHIP_ID   0x00
#define REG_GYRO_DATA      0x02
#define REG_GYRO_RANGE     0x0F
#define REG_GYRO_BANDWIDTH 0x10
#define REG_GYRO_LPM1      0x11

#define ACC_CHIP_ID         0x1E
#define ACC_PWR_SAVE_ACTIVE 0x00
#define ACC_ENABLE_ON       0x04
#define ACC_SOFTRESET       0xB6

#define GYRO_CHIP_ID     0x0F
#define GYRO_PM_NORMAL   0x00

#define ACC_SOFTRESET_DELAY 1UL
#define ACC_PWR_ON_DELAY    5UL
#define GYRO_PWR_ON_DELAY   30UL

static const float acc_range[4] = {
    [BMI088_ACC_RANGE_3G]  = 3.f,
    [BMI088_ACC_RANGE_6G]  = 6.f,
    [BMI088_ACC_RANGE_12G] = 12.f,
    [BMI088_ACC_RANGE_24G] = 24.f,
};
static const float gyro_range[5] = {
    [BMI088_GYRO_RANGE_2000DPS] = 2000.f,
    [BMI088_GYRO_RANGE_1000DPS] = 1000.f,
    [BMI088_GYRO_RANGE_500DPS]  = 500.f,
    [BMI088_GYRO_RANGE_250DPS]  = 250.f,
    [BMI088_GYRO_RANGE_125DPS]  = 125.f,
};

static const float gyro_bias[3] = {
    GYRO_BIAS_X,
    GYRO_BIAS_Y,
    GYRO_BIAS_Z,
};

static void bmi088_soft_reset_acc(bmi088_t *bmi088);
static void bmi088_power_on(bmi088_t *bmi088);

static void bmi088_read_acc_dummy(bmi088_t *bmi088);

static void bmi088_read_acc_reg(bmi088_t *bmi088, uint8_t reg, uint8_t *data, uint16_t size);
static void bmi088_write_acc_reg(bmi088_t *bmi088, uint8_t reg, uint8_t data);
static void bmi088_read_gyro_reg(bmi088_t *bmi088, uint8_t reg, uint8_t *data, uint16_t size);
static void bmi088_write_gyro_reg(bmi088_t *bmi088, uint8_t reg, uint8_t data);

void bmi088_init(bmi088_t *bmi088, SPI_HandleTypeDef *hspi, GPIO_TypeDef *acc_nss_port, uint16_t acc_nss_pin,
                 GPIO_TypeDef *gyro_nss_port, uint16_t gyro_nss_pin) {
    bmi088->hspi = hspi;
    bmi088->acc_nss_port = acc_nss_port;
    bmi088->acc_nss_pin = acc_nss_pin;
    bmi088->gyro_nss_port = gyro_nss_port;
    bmi088->gyro_nss_pin = gyro_nss_pin;

    /* Enter the SPI mode via reading a dummy byte (only required for the accelerometer). */
    bmi088_read_acc_dummy(bmi088);

    /* Read chip IDs. */
    bmi088_read_acc_reg(bmi088, REG_ACC_CHIP_ID, &bmi088->acc_chip_id, 1U);
    bmi088_read_gyro_reg(bmi088, REG_GYRO_CHIP_ID, &bmi088->gyro_chip_id, 1U);
    if (bmi088->acc_chip_id != ACC_CHIP_ID) {
        HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
    }
    if (bmi088->gyro_chip_id != GYRO_CHIP_ID) {
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
    }

    /* Soft-reset the accelerometer. */
    bmi088_soft_reset_acc(bmi088);

    /* Power on. */
    bmi088_power_on(bmi088);

    /* Default range. */
    bmi088->acc_range = BMI088_ACC_RANGE_6G;
    bmi088->gyro_range = BMI088_GYRO_RANGE_2000DPS;
}

void bmi088_set_range(bmi088_t *bmi088, bmi088_acc_range_t acc_range, bmi088_gyro_range_t gyro_range) {
    bmi088_write_acc_reg(bmi088, REG_ACC_RANGE, acc_range);
    bmi088_write_gyro_reg(bmi088, REG_GYRO_RANGE, gyro_range);
    bmi088->acc_range = acc_range;
    bmi088->gyro_range = gyro_range;
}

void bmi088_set_odr(bmi088_t *bmi088, bmi088_acc_odr_t acc_odr, bmi088_acc_bwp_t acc_bwp,
                    bmi088_gyro_odr_bwp gyro_odr_bwp) {
    bmi088_write_acc_reg(bmi088, REG_ACC_CONF, acc_bwp | acc_odr);
    bmi088_write_gyro_reg(bmi088, REG_GYRO_BANDWIDTH, gyro_odr_bwp);
}

void bmi088_read_acc(bmi088_t *bmi088, float *acc) {
    uint8_t data[6];
    uint16_t x;

    bmi088_read_acc_reg(bmi088, REG_ACC_DATA, data, sizeof(data));

    for (int16_t i = 0; i < 3; i++) {
        x = PACK_2(data[2 * i + 1], data[2 * i]);
        acc[i] = to_int16(x) / 32768.f * acc_range[bmi088->acc_range] * ACC_GRAV;
    }
}

void bmi088_read_gyro(bmi088_t *bmi088, float *gyro) {
    uint8_t data[6];
    uint16_t x;

    bmi088_read_gyro_reg(bmi088, REG_GYRO_DATA, data, sizeof(data));

    for (int16_t i = 0; i < 3; i++) {
        x = PACK_2(data[2 * i + 1], data[2 * i]);
        gyro[i] = (to_int16(x) / 32767.f * gyro_range[bmi088->gyro_range] * DEG_TO_RAD) - gyro_bias[i];
    }
}

static void bmi088_soft_reset_acc(bmi088_t *bmi088) {
    /* Write to the soft-reset register and wait until the reset value is written properly. */
    bmi088_write_acc_reg(bmi088, REG_ACC_SOFTRESET, ACC_SOFTRESET);
    HAL_Delay(ACC_SOFTRESET_DELAY);

    /* Enter the SPI mode via reading a dummy byte (only required for the accelerometer). */
    bmi088_read_acc_dummy(bmi088);
}

static void bmi088_power_on(bmi088_t *bmi088) {
    /* Acceleromoter. */
    bmi088_write_acc_reg(bmi088, REG_ACC_PWR_CONF, ACC_PWR_SAVE_ACTIVE);
    HAL_Delay(ACC_PWR_ON_DELAY);
    bmi088_write_acc_reg(bmi088, REG_ACC_PWR_CTRL, ACC_ENABLE_ON);
    HAL_Delay(ACC_PWR_ON_DELAY);

    /* Gyro. */
    bmi088_write_gyro_reg(bmi088, REG_GYRO_LPM1, GYRO_PM_NORMAL);
    HAL_Delay(GYRO_PWR_ON_DELAY);
}

static void bmi088_read_acc_dummy(bmi088_t *bmi088) {
    uint8_t data;
    bmi088_read_acc_reg(bmi088, REG_ACC_CHIP_ID, &data, 1U);
}

static void bmi088_read_acc_reg(bmi088_t *bmi088, uint8_t reg, uint8_t *data, uint16_t size) {
    uint8_t tx_buf[1] = {reg | FLAG_READ};
    uint8_t rx_buf[1] = {0};

    HAL_GPIO_WritePin(bmi088->acc_nss_port, bmi088->acc_nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(bmi088->hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(bmi088->hspi, rx_buf, sizeof(rx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(bmi088->hspi, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(bmi088->acc_nss_port, bmi088->acc_nss_pin, GPIO_PIN_SET);
}

static void bmi088_write_acc_reg(bmi088_t *bmi088, uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg, data};

    HAL_GPIO_WritePin(bmi088->acc_nss_port, bmi088->acc_nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(bmi088->hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(bmi088->acc_nss_port, bmi088->acc_nss_pin, GPIO_PIN_SET);
}

static void bmi088_read_gyro_reg(bmi088_t *bmi088, uint8_t reg, uint8_t *data, uint16_t size) {
    uint8_t tx_buf[1] = {reg | FLAG_READ};

    HAL_GPIO_WritePin(bmi088->gyro_nss_port, bmi088->gyro_nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(bmi088->hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(bmi088->hspi, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(bmi088->gyro_nss_port, bmi088->gyro_nss_pin, GPIO_PIN_SET);
}

static void bmi088_write_gyro_reg(bmi088_t *bmi088, uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg, data};

    HAL_GPIO_WritePin(bmi088->gyro_nss_port, bmi088->gyro_nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(bmi088->hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(bmi088->gyro_nss_port, bmi088->gyro_nss_pin, GPIO_PIN_SET);
}

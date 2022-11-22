#include "config.h"
#include "lis2mdl.h"
#include "util.h"

/* Datasheet: https://www.st.com/resource/en/datasheet/lis2mdl.pdf */

#define FLAG_READ 0x80

#define REG_WHO_AM_I  0x4F
#define REG_CFG_REG_A 0x60
#define REG_CFG_REG_C 0x62
#define REG_OUTX_L   0x68

#define WHO_AM_I     0x40
#define I2C_DIS      0x20
#define BDU          0x10
#define SPI_4W       0x04
#define COMP_TEMP_EN 0x80
#define MD_CONT      0x00

#define SENSITIVITY 1.5f                        /* mG / LSB */

static const float magcal_a[3][3] = {
    {MAG_SOFT_IRON_XX, MAG_SOFT_IRON_XY, MAG_SOFT_IRON_XZ},
    {MAG_SOFT_IRON_XY, MAG_SOFT_IRON_YY, MAG_SOFT_IRON_YZ},
    {MAG_SOFT_IRON_XZ, MAG_SOFT_IRON_YZ, MAG_SOFT_IRON_ZZ},
};
static const float magcal_b[3] = {
    MAG_HARD_IRON_X,
    MAG_HARD_IRON_Y,
    MAG_HARD_IRON_Z,
};

static void lis2mdl_read_reg(lis2mdl_t *lis2mdl, uint8_t reg, uint8_t *data, uint16_t size);
static void lis2mdl_write_reg(lis2mdl_t *lis2mdl, uint8_t reg, uint8_t data);

void lis2mdl_init(lis2mdl_t *lis2mdl, SPI_HandleTypeDef *hspi, GPIO_TypeDef *nss_port, uint16_t nss_pin) {
    lis2mdl->hspi = hspi;
    lis2mdl->nss_port = nss_port;
    lis2mdl->nss_pin = nss_pin;

    /* Enable 4-wire SPI. */
    lis2mdl_write_reg(lis2mdl, REG_CFG_REG_C, I2C_DIS | BDU | SPI_4W);

    /* Read who-am-i. */
    lis2mdl_read_reg(lis2mdl, REG_WHO_AM_I, &lis2mdl->who_am_i, 1U);
    if (lis2mdl->who_am_i != WHO_AM_I) {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
        return;
    }

    /* Enable the magnetometer temperature compensation and select the mode of operation. */
    lis2mdl_write_reg(lis2mdl, REG_CFG_REG_A, COMP_TEMP_EN | MD_CONT);
}

void lis2mdl_set_odr(lis2mdl_t *lis2mdl, lis2mdl_odr_t odr) {
    lis2mdl_write_reg(lis2mdl, REG_CFG_REG_A, COMP_TEMP_EN | MD_CONT | odr);
}

void lis2mdl_read_mag(lis2mdl_t *lis2mdl, float *mag) {
    uint8_t data[6];
    uint16_t x;
    float uncal[3];

    lis2mdl_read_reg(lis2mdl, REG_OUTX_L, data, sizeof(data));

    /* Read uncalibrated magnetic field. */
    for (int16_t i = 0; i < 3; i++) {
        x = PACK_2(data[2 * i + 1], data[2 * i]);
        uncal[i] = to_int16(x) * SENSITIVITY;
    }

    /* Calibrate soft- and hard-iron effect. */
    for (int16_t i = 0; i < 3; i++) {
        mag[i] = 0.0f;
        for (int16_t j = 0; j < 3; j++) {
            mag[i] += magcal_a[i][j] * (uncal[j] - magcal_b[j]);
        }
    }
}

static void lis2mdl_read_reg(lis2mdl_t *lis2mdl, uint8_t reg, uint8_t *data, uint16_t size) {
    uint8_t tx_buf[1] = {reg | FLAG_READ};

    HAL_GPIO_WritePin(lis2mdl->nss_port, lis2mdl->nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(lis2mdl->hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(lis2mdl->hspi, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(lis2mdl->nss_port, lis2mdl->nss_pin, GPIO_PIN_SET);
}

static void lis2mdl_write_reg(lis2mdl_t *lis2mdl, uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg, data};

    HAL_GPIO_WritePin(lis2mdl->nss_port, lis2mdl->nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(lis2mdl->hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(lis2mdl->nss_port, lis2mdl->nss_pin, GPIO_PIN_SET);
}

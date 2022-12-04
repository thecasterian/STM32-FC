#include "bmi088.h"
#include "const.h"
#include "main.h"
#include "spi_wrapper.h"
#include "util.h"

/* Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi088-ds001.pdf */

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

static const float acc_range_sf[4] = {
    [BMI088_ACC_RANGE_3G]  = 3.f,
    [BMI088_ACC_RANGE_6G]  = 6.f,
    [BMI088_ACC_RANGE_12G] = 12.f,
    [BMI088_ACC_RANGE_24G] = 24.f,
};
static const float gyro_range_sf[5] = {
    [BMI088_GYRO_RANGE_2000DPS] = 2000.f,
    [BMI088_GYRO_RANGE_1000DPS] = 1000.f,
    [BMI088_GYRO_RANGE_500DPS]  = 500.f,
    [BMI088_GYRO_RANGE_250DPS]  = 250.f,
    [BMI088_GYRO_RANGE_125DPS]  = 125.f,
};

static struct {
    bmi088_acc_range_t acc_range;
    bmi088_gyro_range_t gyro_range;
} bmi088_config;

static void bmi088_soft_reset_acc(void);
static void bmi088_power_on(void);

static void bmi088_read_acc_dummy(void);

void bmi088_init(void) {
    uint8_t acc_chip_id, gyro_chip_id;

    /* Enter the SPI mode via reading a dummy byte (only required for the accelerometer). */
    bmi088_read_acc_dummy();

    /* Read chip IDs. */
    bmi088_read_acc_reg(REG_ACC_CHIP_ID, &acc_chip_id, 1U);
    bmi088_read_gyro_reg(REG_GYRO_CHIP_ID, &gyro_chip_id, 1U);
    // TODO: validate chip IDs.

    /* Soft-reset the accelerometer. */
    bmi088_soft_reset_acc();

    /* Power on. */
    bmi088_power_on();

    /* Default range. */
    bmi088_config.acc_range = BMI088_ACC_RANGE_6G;
    bmi088_config.gyro_range = BMI088_GYRO_RANGE_2000DPS;
}

void bmi088_set_range(bmi088_acc_range_t acc_range, bmi088_gyro_range_t gyro_range) {
    bmi088_write_acc_reg(REG_ACC_RANGE, acc_range);
    bmi088_write_gyro_reg(REG_GYRO_RANGE, gyro_range);
    acc_range = acc_range;
    gyro_range = gyro_range;
}

void bmi088_set_odr_bwp(bmi088_acc_odr_t acc_odr, bmi088_acc_bwp_t acc_bwp,
                        bmi088_gyro_odr_bwp gyro_odr_bwp) {
    bmi088_write_acc_reg(REG_ACC_CONF, acc_bwp | acc_odr);
    bmi088_write_gyro_reg(REG_GYRO_BANDWIDTH, gyro_odr_bwp);
}

void bmi088_read_acc(float acc[3]) {
    uint8_t data[6];
    uint16_t x;

    bmi088_read_acc_reg(REG_ACC_DATA, data, sizeof(data));

    for (int16_t i = 0; i < 3; i++) {
        x = PACK_2(data[2 * i + 1], data[2 * i]);
        acc[i] = to_int16(x) / 32768.f * acc_range_sf[bmi088_config.acc_range] * ACC_GRAV;
    }
}

void bmi088_read_gyro(float ang[3]) {
    uint8_t data[6];
    uint16_t x;

    bmi088_read_gyro_reg(REG_GYRO_DATA, data, sizeof(data));

    for (int16_t i = 0; i < 3; i++) {
        x = PACK_2(data[2 * i + 1], data[2 * i]);
        ang[i] = to_int16(x) / 32767.f * gyro_range_sf[bmi088_config.gyro_range] * DEG_TO_RAD;
    }
}

static void bmi088_soft_reset_acc(void) {
    /* Write to the soft-reset register and wait until the reset value is written properly. */
    bmi088_write_acc_reg(REG_ACC_SOFTRESET, ACC_SOFTRESET);
    HAL_Delay(ACC_SOFTRESET_DELAY);

    /* Enter the SPI mode via reading a dummy byte (only required for the accelerometer). */
    bmi088_read_acc_dummy();
}

static void bmi088_power_on(void) {
    /* Acceleromoter. */
    bmi088_write_acc_reg(REG_ACC_PWR_CONF, ACC_PWR_SAVE_ACTIVE);
    HAL_Delay(ACC_PWR_ON_DELAY);
    bmi088_write_acc_reg(REG_ACC_PWR_CTRL, ACC_ENABLE_ON);
    HAL_Delay(ACC_PWR_ON_DELAY);

    /* Gyro. */
    bmi088_write_gyro_reg(REG_GYRO_LPM1, GYRO_PM_NORMAL);
    HAL_Delay(GYRO_PWR_ON_DELAY);
}

static void bmi088_read_acc_dummy(void) {
    uint8_t data;
    bmi088_read_acc_reg(REG_ACC_CHIP_ID, &data, 1U);
}

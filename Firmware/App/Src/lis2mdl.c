#include "lis2mdl.h"
#include "spi_wrapper.h"
#include "util.h"

/* Datasheet: https://www.st.com/resource/en/datasheet/lis2mdl.pdf */

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

void lis2mdl_init(void) {
    uint8_t who_am_i;

    /* Enable 4-wire SPI. */
    lis2mdl_write_reg(REG_CFG_REG_C, I2C_DIS | BDU | SPI_4W);

    /* Read who-am-i. */
    lis2mdl_read_reg(REG_WHO_AM_I, &who_am_i, 1U);
    // TODO: validate who am i.

    /* Enable the magnetometer temperature compensation and select the mode of operation. */
    lis2mdl_write_reg(REG_CFG_REG_A, COMP_TEMP_EN | MD_CONT);
}

void lis2mdl_set_odr(lis2mdl_odr_t odr) {
    lis2mdl_write_reg(REG_CFG_REG_A, COMP_TEMP_EN | MD_CONT | odr);
}

void lis2mdl_read_mag(float *mag) {
    uint8_t data[6];
    uint16_t x;

    lis2mdl_read_reg(REG_OUTX_L, data, sizeof(data));

    /* Read magnetic field. */
    for (int16_t i = 0; i < 3; i++) {
        x = PACK_2(data[2 * i + 1], data[2 * i]);
        mag[i] = to_int16(x) * SENSITIVITY;
    }
}

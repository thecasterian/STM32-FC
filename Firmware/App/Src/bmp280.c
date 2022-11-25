#include <stdbool.h>
#include "bmp280.h"
#include "util.h"

/* Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf */

#define MASK_WRITE 0x7F

#define REG_CALIB00   0x88
#define REG_ID        0xD0
#define REG_RESET     0xE0
#define REG_STATUS    0xF3
#define REG_CTRL_MEAS 0xF4
#define REG_CONFIG    0xF5
#define REG_PRESS_MSB 0xF7

#define CHIP_ID     0x58
#define RESET       0xB6
#define SLEEP_MODE  0x00
#define NORMAL_MODE 0x03

#define POS_OSRS_T 5
#define POS_OSRS_P 2
#define POS_T_SB   5
#define POS_FILTER 2

#define MASK_IM_UPDATE 0x01

static void bmp280_read_compen_param(bmp280_t *bmp280);
static void bmp280_compen_temp(bmp280_t *bmp280, int32_t adc_temp, int32_t *t_fine, float *temp);
static void bmp280_compen_pres(bmp280_t *bmp280, int32_t adc_pres, int32_t t_fine, float *pres);

static void bmp280_read_reg(bmp280_t *bmp280, uint8_t reg, uint8_t *data, uint16_t size);
static void bmp280_write_reg(bmp280_t *bmp280, uint8_t reg, uint8_t data);

void bmp280_init(bmp280_t *bmp280, SPI_HandleTypeDef *hspi, GPIO_TypeDef *nss_port, uint16_t nss_pin) {
    bmp280->hspi = hspi;
    bmp280->nss_port = nss_port;
    bmp280->nss_pin = nss_pin;

    /* Read chip ID. */
    bmp280_read_reg(bmp280, REG_ID, &bmp280->chip_id, 1U);
    if (bmp280->chip_id != CHIP_ID) {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
    }

    /* Soft reset. */
    bmp280_write_reg(bmp280, REG_RESET, RESET);
    HAL_Delay(3UL);

    /* Read compensation parameter. */
    bmp280_read_compen_param(bmp280);
}

void bmp280_set_param(bmp280_t *bmp280, bmp280_ospl_t pres_ospl, bmp280_ospl_t temp_ospl, bmp280_stby_time_t stby_time,
                      bmp280_iir_coeff_t iir_coeff) {
    /* Register "config" must be written in the sleep mode. */
    bmp280_write_reg(bmp280, REG_CTRL_MEAS, SLEEP_MODE);

    bmp280_write_reg(bmp280, REG_CONFIG, (stby_time << POS_T_SB) | (iir_coeff << POS_FILTER));
    bmp280_write_reg(bmp280, REG_CTRL_MEAS, (temp_ospl << POS_OSRS_T) | (pres_ospl << POS_OSRS_P) | NORMAL_MODE);

    uint8_t tmp;
    HAL_Delay(1UL);
    bmp280_read_reg(bmp280, REG_CONFIG, &tmp, 1U);
    if (tmp != ((stby_time << POS_T_SB) | (iir_coeff << POS_FILTER))) {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
    }
}

void bmp280_read_pres_temp(bmp280_t *bmp280, float *pres, float *temp) {
    uint8_t data[6];
    uint32_t adc_pres, adc_temp;
    int32_t t_fine;

    bmp280_read_reg(bmp280, REG_PRESS_MSB, data, sizeof(data));

    /* ADC values of pressure and temperature are in 20 bits. */
    adc_pres = (((uint32_t)data[0]) << 12) | (data[1] << 4) | (data[2] >> 4);
    adc_temp = (((uint32_t)data[3]) << 12) | (data[4] << 4) | (data[5] >> 4);

    /* Compensate temperature and pressure. */
    bmp280_compen_temp(bmp280, adc_temp, &t_fine, temp);
    bmp280_compen_pres(bmp280, adc_pres, t_fine, pres);
}

static void bmp280_read_compen_param(bmp280_t *bmp280) {
    uint8_t data[24];

    bmp280_read_reg(bmp280, REG_CALIB00, data, sizeof(data));

    bmp280->dig_t1 = PACK_2(data[1], data[0]);
    bmp280->dig_t2 = to_int16(PACK_2(data[3], data[2]));
    bmp280->dig_t3 = to_int16(PACK_2(data[5], data[4]));
    bmp280->dig_p1 = PACK_2(data[7], data[6]);
    bmp280->dig_p2 = to_int16(PACK_2(data[9], data[8]));
    bmp280->dig_p3 = to_int16(PACK_2(data[11], data[10]));
    bmp280->dig_p4 = to_int16(PACK_2(data[13], data[12]));
    bmp280->dig_p5 = to_int16(PACK_2(data[15], data[14]));
    bmp280->dig_p6 = to_int16(PACK_2(data[17], data[16]));
    bmp280->dig_p7 = to_int16(PACK_2(data[19], data[18]));
    bmp280->dig_p8 = to_int16(PACK_2(data[21], data[20]));
    bmp280->dig_p9 = to_int16(PACK_2(data[23], data[22]));
}

static void bmp280_compen_temp(bmp280_t *bmp280, int32_t adc_temp, int32_t *t_fine, float *temp) {
    int32_t var1, var2;

    var1 = (((adc_temp >> 3) - (bmp280->dig_t1 << 1)) * bmp280->dig_t2) >> 11;
    var2 = (adc_temp >> 4) - bmp280->dig_t1;
    var2 = (((var2 * var2) >> 12) * bmp280->dig_t3) >> 14;
    *t_fine = var1 + var2;
    *temp = ((*t_fine) * 5 + 128) >> 8;
}

static void bmp280_compen_pres(bmp280_t *bmp280, int32_t adc_pres, int32_t t_fine, float *pres) {
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * bmp280->dig_p6;
    var2 = var2 + ((var1 * bmp280->dig_p5) << 17);
    var2 = var2 + ((bmp280->dig_p4) << 35);
    var1 = ((var1 * var1 * bmp280->dig_p3) >> 8) + ((var1 * bmp280->dig_p2) << 12);
    var1 = (((1LL << 47) + var1) * bmp280->dig_p1) >> 33;
    if (var1 == 0) {
        *pres = 0.f;
    } else {
        p = 1048576 - adc_pres;
        p =  (((p << 31) - var2) * 3125) / var1;
        var1 = (bmp280->dig_p9 * (p >> 13) * (p >> 13)) >> 25;
        var2 = (bmp280->dig_p8 * p) >> 19;
        p = ((p + var1 + var2) >> 8) + (bmp280->dig_p7 << 4);
        *pres = p / 256.f;
    }
}

static void bmp280_read_reg(bmp280_t *bmp280, uint8_t reg, uint8_t *data, uint16_t size) {
    uint8_t tx_buf[1] = {reg};

    HAL_GPIO_WritePin(bmp280->nss_port, bmp280->nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(bmp280->hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_SPI_Receive(bmp280->hspi, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(bmp280->nss_port, bmp280->nss_pin, GPIO_PIN_SET);
}

static void bmp280_write_reg(bmp280_t *bmp280, uint8_t reg, uint8_t data) {
    uint8_t tx_buf[2] = {reg & MASK_WRITE, data};

    HAL_GPIO_WritePin(bmp280->nss_port, bmp280->nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(bmp280->hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(bmp280->nss_port, bmp280->nss_pin, GPIO_PIN_SET);
}

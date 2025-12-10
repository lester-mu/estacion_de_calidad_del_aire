#include "bmp280.h"
#include "i2c.h"
#include <util/delay.h>

bmp280_calib_param_t bmp280_calib_param;
int32_t t_fine;

static uint8_t bmp280_read_byte(uint8_t reg_addr) {
    uint8_t data;
    i2c_start();
    i2c_write(BMP280_ADDR << 1);
    i2c_write(reg_addr);
    i2c_start();
    i2c_write((BMP280_ADDR << 1) | 0x01);
    data = i2c_read_nack();
    i2c_stop();
    return data;
}

static void bmp280_read_bytes(uint8_t reg_addr, uint8_t *data, uint8_t len) {
    i2c_start();
    i2c_write(BMP280_ADDR << 1);
    i2c_write(reg_addr);
    i2c_start();
    i2c_write((BMP280_ADDR << 1) | 0x01);

    for (uint8_t i = 0; i < len; i++) {
        if (i == len - 1) {
            data[i] = i2c_read_nack();
        } else {
            data[i] = i2c_read_ack();
        }
    }
    i2c_stop();
}

static void bmp280_write_byte(uint8_t reg_addr, uint8_t data) {
    i2c_start();
    i2c_write(BMP280_ADDR << 1);
    i2c_write(reg_addr);
    i2c_write(data);
    i2c_stop();
}

static void bmp280_read_calibration_data(void) {
    uint8_t calib_data[24];
    bmp280_read_bytes(BMP280_REG_CALIB00, calib_data, 24);

    bmp280_calib_param.dig_T1 = (uint16_t)calib_data[0] | ((uint16_t)calib_data[1] << 8);
    bmp280_calib_param.dig_T2 = (int16_t)calib_data[2] | ((int16_t)calib_data[3] << 8);
    bmp280_calib_param.dig_T3 = (int16_t)calib_data[4] | ((int16_t)calib_data[5] << 8);
    bmp280_calib_param.dig_P1 = (uint16_t)calib_data[6] | ((uint16_t)calib_data[7] << 8);
    bmp280_calib_param.dig_P2 = (int16_t)calib_data[8] | ((int16_t)calib_data[9] << 8);
    bmp280_calib_param.dig_P3 = (int16_t)calib_data[10] | ((int16_t)calib_data[11] << 8);
    bmp280_calib_param.dig_P4 = (int16_t)calib_data[12] | ((int16_t)calib_data[13] << 8);
    bmp280_calib_param.dig_P5 = (int16_t)calib_data[14] | ((int16_t)calib_data[15] << 8);
    bmp280_calib_param.dig_P6 = (int16_t)calib_data[16] | ((int16_t)calib_data[17] << 8);
    bmp280_calib_param.dig_P7 = (int16_t)calib_data[18] | ((int16_t)calib_data[19] << 8);
    bmp280_calib_param.dig_P8 = (int16_t)calib_data[20] | ((int16_t)calib_data[21] << 8);
    bmp280_calib_param.dig_P9 = (int16_t)calib_data[22] | ((int16_t)calib_data[23] << 8);
}

void BMP280_init(void) {
    _delay_ms(10);
    bmp280_read_calibration_data();
    bmp280_write_byte(BMP280_REG_CTRL_MEAS, (BMP280_OVERSAMPLING_X16 << 5) | (BMP280_OVERSAMPLING_X16 << 2) | BMP280_MODE_NORMAL);
    bmp280_write_byte(BMP280_REG_CONFIG, (0x04 << 5) | (0x04 << 2));
    _delay_ms(100);
}

uint8_t BMP280_read_chip_id(void) {
    return bmp280_read_byte(BMP280_REG_ID);
}

void BMP280_read_raw_data(int32_t *raw_temp, int32_t *raw_press) {
    uint8_t data[6];
    bmp280_read_bytes(BMP280_REG_PRESS_MSB, data, 6);
    *raw_press = (int32_t)data[0] << 12 | (int32_t)data[1] << 4 | (data[2] >> 4);
    *raw_temp  = (int32_t)data[3] << 12 | (int32_t)data[4] << 4 | (data[5] >> 4);
}

float BMP280_compensate_temperature(int32_t raw_temp) {
    float var1, var2, temp_c;
    var1 = (((float)raw_temp) / 16384.0 - ((float)bmp280_calib_param.dig_T1) / 1024.0) * ((float)bmp280_calib_param.dig_T2);
    var2 = ((((float)raw_temp) / 131072.0 - ((float)bmp280_calib_param.dig_T1) / 8192.0) *
            (((float)raw_temp) / 131072.0 - ((float)bmp280_calib_param.dig_T1) / 8192.0)) * ((float)bmp280_calib_param.dig_T3);
    t_fine = (int32_t)(var1 + var2);
    temp_c = (var1 + var2) / 5120.0;
    return temp_c;
}

float BMP280_compensate_pressure(int32_t raw_press) {
    float var1, var2, pressure;
    var1 = ((float)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((float)bmp280_calib_param.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((float)bmp280_calib_param.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((float)bmp280_calib_param.dig_P4) * 65536.0);
    var1 = (((float)bmp280_calib_param.dig_P3) * var1 * var1 / 524288.0 + ((float)bmp280_calib_param.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((float)bmp280_calib_param.dig_P1);
    if (var1 == 0.0) {
        return 0;
    }
    pressure = 1048576.0 - ((float)raw_press);
    pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((float)bmp280_calib_param.dig_P9) * pressure * pressure / 2147483648.0;
    var2 = pressure * ((float)bmp280_calib_param.dig_P8) / 32768.0;
    pressure = pressure + (var1 + var2 + ((float)bmp280_calib_param.dig_P7)) / 16.0;
    return pressure;
}

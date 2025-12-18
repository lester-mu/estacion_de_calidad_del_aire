#ifndef BMP280_H
#define BMP280_H

#include <stdint.h> // Para uint8_t, int16_t, uint16_t, int32_t
#include <math.h>   // Para la función pow() en el cálculo de altitud

// Dirección I2C del BMP280
// Las direcciones estándar son 0x76 (SDO conectado a GND) o 0x77 (SDO conectado a VCC).
// Verifica la conexión del pin SDO de tu sensor. 0x76 es la más común.
#define BMP280_ADDR     0x76

// Direcciones de registros del BMP280
#define BMP280_REG_ID           0xD0    // Register ID (debe leer 0x58)
#define BMP280_REG_RESET        0xE0    // Register de Reset
#define BMP280_REG_STATUS       0xF3    // Register de Estado
#define BMP280_REG_CTRL_MEAS    0xF4    // Register de Control de Medición
#define BMP280_REG_CONFIG       0xF5    // Register de Configuración
#define BMP280_REG_PRESS_MSB    0xF7    // Datos de medición de presión (MSB)
#define BMP280_REG_PRESS_LSB    0xF8    // Datos de medición de presión (LSB)
#define BMP280_REG_PRESS_XLSB   0xF9    // Datos de medición de presión (XLSB)
#define BMP280_REG_TEMP_MSB     0xFA    // Datos de medición de temperatura (MSB)
#define BMP280_REG_TEMP_LSB     0xFB    // Datos de medición de temperatura (LSB)
#define BMP280_REG_TEMP_XLSB    0xFC    // Datos de medición de temperatura (XLSB)

// Inicio de registros de datos de calibración
#define BMP280_REG_CALIB00      0x88    // dig_T1_LSB

// Máscaras y valores de bits para la configuración del registro CTRL_MEAS
// Configuraciones de sobremuestreo (osrs_t, osrs_p)
#define BMP280_OVERSAMPLING_SKIPPED 0x00
#define BMP280_OVERSAMPLING_X1      0x01
#define BMP280_OVERSAMPLING_X2      0x02
#define BMP280_OVERSAMPLING_X4      0x03
#define BMP280_OVERSAMPLING_X8      0x04
#define BMP280_OVERSAMPLING_X16     0x05

// Configuraciones de modo
#define BMP280_MODE_SLEEP       0x00
#define BMP280_MODE_FORCED      0x01 // Tanto 0x01 como 0x02 son 'forced' mode
#define BMP280_MODE_NORMAL      0x03

// Máscaras y valores de bits para la configuración del registro CONFIG
// Valores de t_sb (tiempo de espera)
#define BMP280_STANDBY_0_5_MS   0x00
#define BMP280_STANDBY_62_5_MS  0x01
#define BMP280_STANDBY_125_MS   0x02
#define BMP280_STANDBY_250_MS   0x03
#define BMP280_STANDBY_500_MS   0x04
#define BMP280_STANDBY_1000_MS  0x05
#define BMP280_STANDBY_10_MS    0x06
#define BMP280_STANDBY_20_MS    0x07

// Valores de coeficiente de filtro IIR
#define BMP280_FILTER_OFF       0x00
#define BMP280_FILTER_2         0x01
#define BMP280_FILTER_4         0x02
#define BMP280_FILTER_8         0x03
#define BMP280_FILTER_16        0x04

// Estructura de parámetros de calibración (según el datasheet del BMP280)
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} bmp280_calib_param_t;

// Variables globales (extern para que otros archivos puedan acceder a ellas)
extern bmp280_calib_param_t bmp280_calib_param;
extern int32_t t_fine; // Temperatura de resolución fina, necesaria para la compensación de presión

// Prototipos de funciones públicas
void BMP280_init(void);
uint8_t BMP280_read_chip_id(void);
void BMP280_read_raw_data(int32_t *raw_temp, int32_t *raw_press);
float BMP280_compensate_temperature(int32_t raw_temp);
float BMP280_compensate_pressure(int32_t raw_press);
float BMP280_get_altitude(float pressure_pa, float sea_level_pa);

#endif // BMP280_H


#ifndef BH1750_H
#define BH1750_H

#include <avr/io.h>

#define BH1750_ADDR         0x23 // Dirección I2C del BH1750 (0x23 o 0x5C)

// Comandos de modo de medición
#define BH1750_POWER_DOWN   0x00
#define BH1750_POWER_ON     0x01
#define BH1750_RESET        0x07

// Modos de medición continua
#define BH1750_CONT_HIGH_RES_MODE  0x10 // 1 lux, 120ms
#define BH1750_CONT_HIGH_RES_MODE2 0x11 // 0.5 lux, 120ms
#define BH1750_CONT_LOW_RES_MODE   0x13 // 4 lux, 16ms

void BH1750_init(void);
uint16_t BH1750_read_lux(void);

#endif // BH1750_H

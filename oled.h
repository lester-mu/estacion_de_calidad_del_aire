#ifndef OLED_H
#define OLED_H

#include <avr/io.h> // Necesario para uint8_t

// Dirección I2C del OLED (cambia si tu módulo tiene otra)
// 0x3C para la mayoría de los módulos 128x64
#define OLED_ADDR 0x3C

void OLED_command(uint8_t cmd);
void OLED_data(uint8_t data);
void OLED_init(void);
void OLED_clear(void);
void OLED_set_cursor(uint8_t page, uint8_t column);
void OLED_char(char c);
void OLED_print(uint8_t page, uint8_t column, const char *str);

#endif // OLED_H


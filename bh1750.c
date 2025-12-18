#include "bh1750.h"
#include "i2c.h"
#include <util/delay.h>

void BH1750_init(void) {
    i2c_start();
    i2c_write(BH1750_ADDR << 1); 
    i2c_write(BH1750_POWER_ON);  
    i2c_stop();

    _delay_ms(10); 

    i2c_start();
    i2c_write(BH1750_ADDR << 1); 
    i2c_write(BH1750_CONT_HIGH_RES_MODE);
    i2c_stop();

    _delay_ms(180); 
}

uint16_t BH1750_read_lux(void) {
    uint8_t msb, lsb;
    uint16_t raw_lux = 0; 

    i2c_start();
    i2c_write((BH1750_ADDR << 1) | 0x01);

    msb = i2c_read_ack();  
    lsb = i2c_read_nack(); 
    i2c_stop();

    raw_lux = (msb << 8) | lsb;

    float actual_lux_float = (float)raw_lux / 1.2;

    return (uint16_t)actual_lux_float;
}


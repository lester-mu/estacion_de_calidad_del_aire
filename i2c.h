#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);  // Lee con ACK
uint8_t i2c_read_nack(void); // Lee con NACK (último byte)

#endif // I2C_H

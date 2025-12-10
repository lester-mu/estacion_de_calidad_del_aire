#include "sgp.h"
#include "i2c.h"
#include <util/delay.h>


#define I2C_WRITE 0
#define I2C_READ  1


uint16_t sgp30_eCO2;
uint16_t sgp30_TVOC;

// Funcion privada para el calculo de CRC8 
static uint8_t SGP30_crc8(const uint8_t *data, int len) {
    uint8_t crc = 0xFF;
    for (int j = 0; j < len; j++) {
        crc ^= data[j];
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
        }
    }
    return crc;
}


// Funcion privada para enviar comandos de 2 bytes
static uint8_t SGP30_send_command(uint16_t command) {
    uint8_t command_bytes[2];
    command_bytes[0] = command >> 8;
    command_bytes[1] = command & 0xFF;

    i2c_start(); 
    i2c_write((SGP30_I2C_ADDRESS << 1) | I2C_WRITE);
    i2c_write(command_bytes[0]);
    i2c_write(command_bytes[1]);
    i2c_stop(); 
    
    return 1;
}

uint8_t SGP30_init(void) {
    if (!SGP30_send_command(0x2003)) {
        return 0;
    }
    _delay_ms(10); 
    return 1;
}

uint8_t SGP30_request_measure(void) {
    return SGP30_send_command(0x2008);
}

uint8_t SGP30_read_data(void) {
    uint8_t read_buffer[6];

    i2c_start();
    i2c_write((SGP30_I2C_ADDRESS << 1) | I2C_READ);
    
    for (uint8_t i = 0; i < 5; i++) {
        read_buffer[i] = i2c_read_ack();
    }
    read_buffer[5] = i2c_read_nack();
    i2c_stop();

    // Verificación de CRC 
    uint8_t crc1 = SGP30_crc8(&read_buffer[0], 2);
    if (crc1 != read_buffer[2]) {
        return 0; // Error de CRC
    }
    uint8_t crc2 = SGP30_crc8(&read_buffer[3], 2);
    if (crc2 != read_buffer[5]) {
        return 0; // Error de CRC
    }

    sgp30_eCO2 = (read_buffer[0] << 8) | read_buffer[1];
    sgp30_TVOC = (read_buffer[3] << 8) | read_buffer[4];

    return 1;
}

uint8_t SGP30_measure_air_quality(uint16_t* eco2, uint16_t* tvoc) {
    if (!SGP30_request_measure()) {
        return 0;
    }
    _delay_ms(12); 
    
    if (!SGP30_read_data()) {
        return 0;
    }
    
    *eco2 = sgp30_eCO2;
    *tvoc = sgp30_TVOC;
    
    return 1;
}

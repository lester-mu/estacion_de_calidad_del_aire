#ifndef SGP30_H_
#define SGP30_H_

#include <stdint.h>

#define SGP30_I2C_ADDRESS 0x58

// Variables para almacenar los valores de las mediciones
extern uint16_t sgp30_eCO2;
extern uint16_t sgp30_TVOC;

uint8_t SGP30_init(void);

uint8_t SGP30_request_measure(void);

uint8_t SGP30_read_data(void);

uint8_t SGP30_measure_air_quality(uint16_t* eco2, uint16_t* tvoc);

#endif /* SGP30_H_ */

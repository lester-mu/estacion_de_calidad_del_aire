#ifndef SGP30_H_
#define SGP30_H_

#include <stdint.h>

#define SGP30_I2C_ADDRESS 0x58

// Variables para almacenar los valores de las mediciones
extern uint16_t sgp30_eCO2;
extern uint16_t sgp30_TVOC;

/**
 * @brief Inicializa el sensor SGP30.
 *        Envía el comando de inicialización y espera el tiempo de calentamiento.
 * @return uint8_t 1 en caso de éxito, 0 en caso de fallo (no se encontró el sensor).
 */
uint8_t SGP30_init(void);

/**
 * @brief Envía el comando para solicitar una nueva medición de calidad del aire.
 * @return uint8_t 1 si el comando fue enviado correctamente, 0 si no.
 */
uint8_t SGP30_request_measure(void);

/**
 * @brief Lee los últimos datos medidos de eCO2 y TVOC.
 *        Debe llamarse ~12ms después de SGP30_request_measure().
 *        Los resultados se guardan en las variables globales sgp30_eCO2 y sgp30_TVOC.
 * @return uint8_t 1 en caso de éxito (CRC correcto), 0 en caso de fallo.
 */
uint8_t SGP30_read_data(void);

/**
 * @brief Realiza el ciclo completo de pedir y leer una medición.
 *        Esta función combina request y read, con la espera necesaria.
 * @param eco2 Puntero para guardar el valor de eCO2.
 * @param tvoc Puntero para guardar el valor de TVOC.
 * @return uint8_t 1 en caso de éxito, 0 en caso de fallo.
 */
uint8_t SGP30_measure_air_quality(uint16_t* eco2, uint16_t* tvoc);

#endif /* SGP30_H_ */

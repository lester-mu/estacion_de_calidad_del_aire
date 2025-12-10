#ifndef DHT11_H_
#define DHT11_H_

#include <stdint.h>

// Lista de posibles estados/errores del sensor
enum DHT_Status_t {
	DHT_Ok,
	DHT_Error_Checksum,
	DHT_Error_Timeout,
	DHT_Error_Temperature,
	DHT_Error_Humidity
};

// Funciones que se pueden llamar desde main.c
void DHT_Setup();
enum DHT_Status_t DHT_GetStatus();
enum DHT_Status_t DHT_Read(double* Temperature, double* Humidity);

#endif /* DHT11_H_ */

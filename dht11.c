#define F_CPU 16000000UL

#include "dht11.h"
#include <util/delay.h>
#include <avr/io.h>

#define DHT_TYPE 11

#if (DHT_TYPE == 11)
	#define __DHT_Delay_Setup     1000
	#define __DHT_Temperature_Min 0
	#define __DHT_Temperature_Max 50
	#define __DHT_Humidity_Min    20
	#define __DHT_Humidity_Max    90
	#define __DHT_Delay_Read      20
#endif

// variable global interna para el estado
enum DHT_Status_t __DHT_STATUS;

// prototipos de funciones internas
static enum DHT_Status_t DHT_ReadRaw(uint8_t Data[4]);
static double ExtractTemperature(uint8_t Data2, uint8_t Data3);
static double ExtractHumidity(uint8_t Data0, uint8_t Data1);

//implementacion de funciones

void DHT_Setup() {
	_delay_ms(__DHT_Delay_Setup);
	__DHT_STATUS = DHT_Ok;
}

enum DHT_Status_t DHT_GetStatus() {
	return __DHT_STATUS;
}

static enum DHT_Status_t DHT_ReadRaw(uint8_t Data[4]) {
	uint8_t buffer[5] = {0};
	uint8_t retries, i;
	int8_t j;

	__DHT_STATUS = DHT_Ok;

	DDRD |= (1 << PD2);
	PORTD &= ~(1 << PD2);
	_delay_ms(__DHT_Delay_Read);

	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);

	retries = 0;
	while (PIND & (1 << PD2)) {
		_delay_us(2);
		if (++retries > 40) return (__DHT_STATUS = DHT_Error_Timeout);
	}

	retries = 0;
	while (!(PIND & (1 << PD2))) {
		_delay_us(2);
		if (++retries > 50) return (__DHT_STATUS = DHT_Error_Timeout);
	}

	retries = 0;
	while (PIND & (1 << PD2)) {
		_delay_us(2);
		if (++retries > 50) return (__DHT_STATUS = DHT_Error_Timeout);
	}

	for (i = 0; i < 5; i++) {
		for (j = 7; j >= 0; j--) {
			retries = 0;
			while (!(PIND & (1 << PD2))) {
				_delay_us(2);
				if (++retries > 35) return (__DHT_STATUS = DHT_Error_Timeout);
			}

			_delay_us(35);
			if (PIND & (1 << PD2)) {
				buffer[i] |= (1 << j);
			}

			retries = 0;
			while (PIND & (1 << PD2)) {
				_delay_us(2);
				if (++retries > 50) return (__DHT_STATUS = DHT_Error_Timeout);
			}
		}
	}

	if ((uint8_t)(buffer[0] + buffer[1] + buffer[2] + buffer[3]) != buffer[4]) {
		return (__DHT_STATUS = DHT_Error_Checksum);
	}

	for (i = 0; i < 4; i++) Data[i] = buffer[i];

	return (__DHT_STATUS = DHT_Ok);
}

enum DHT_Status_t DHT_Read(double *Temperature, double *Humidity) {
	uint8_t data[4] = {0};

	if (DHT_ReadRaw(data) == DHT_Ok) {
		*Temperature = ExtractTemperature(data[2], data[3]);
		*Humidity = ExtractHumidity(data[0], data[1]);

		if (*Temperature < __DHT_Temperature_Min || *Temperature > __DHT_Temperature_Max) {
			return (__DHT_STATUS = DHT_Error_Temperature);
		}
		if (*Humidity < __DHT_Humidity_Min || *Humidity > __DHT_Humidity_Max) {
			return (__DHT_STATUS = DHT_Error_Humidity);
		}
	}
	return __DHT_STATUS;
}

static double ExtractTemperature(uint8_t Data2, uint8_t Data3) {
    return (double)Data2 + ((double)Data3 * 0.1);
}

static double ExtractHumidity(uint8_t Data0, uint8_t Data1) {
    return (double)Data0 + ((double)Data1 * 0.1);
}


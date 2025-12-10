#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>


#define GUVA_ADC_CHANNEL    3
#define LOOP_DELAY_MS       2000

#define WEIGHT_CO2          40
#define WEIGHT_TVOC         35
#define WEIGHT_HUMIDITY     20
#define WEIGHT_UV           5

#include "adc.h"
#include "i2c.h"
#include "oled.h"
#include "bh1750.h"
#include "bmp280.h"
#include "dht11.h"
#include "guva.h"
#include "sgp.h"

void setup_peripherals(void);
const char* determinar_estado_clima(uint16_t lux, uint8_t hum, uint16_t uv_deci);
void update_display(uint8_t error_flag, uint16_t quality_index, const char* estado_clima);

int main(void) {
    setup_peripherals();

    while (1) {
        uint8_t hum_pct = 0;
        uint16_t lux = 0;
        uint16_t uv_index_deci = 0;
        uint16_t eco2_ppm = 0;
        uint16_t tvoc_ppb = 0;

        uint8_t error_flag = 0;
        
        double temp_float, hum_float;
        if (DHT_Read(&temp_float, &hum_float) != DHT_Ok) {
            error_flag = 1;
        } else {
            hum_pct = (uint8_t)hum_float;
        }

         lux = BH1750_read_lux();
        if (lux == 0xFFFF) {
            lux = 0; error_flag = 1;
        }

        int32_t raw_temp, raw_press;
        BMP280_read_raw_data(&raw_temp, &raw_press);
        BMP280_compensate_temperature(raw_temp);

        float uv_float = GUVA_read_uv_index(GUVA_ADC_CHANNEL);
        if (uv_float < 0) {
            error_flag = 1;
        } else {
            uv_index_deci = (uint16_t)(uv_float * 10.0f);
        }

        if (!SGP30_measure_air_quality(&eco2_ppm, &tvoc_ppb)) {
            error_flag = 1;
        }

        uint16_t quality_index = 0;
        if (!error_flag) {
            uint16_t co2_score = (eco2_ppm < 800) ? 100 : (eco2_ppm < 1000) ? 80 : (eco2_ppm < 1500) ? 60 : (eco2_ppm < 2000) ? 40 : 20;
            uint16_t tvoc_score = (tvoc_ppb < 100) ? 100 : (tvoc_ppb < 200) ? 80 : (tvoc_ppb < 300) ? 60 : (tvoc_ppb < 400) ? 40 : 20;
            uint16_t hum_score = (hum_pct >= 30 && hum_pct <= 60) ? 100 : (hum_pct >= 20 && hum_pct <= 70) ? 70 : 40;
            uint16_t uv_score = (uv_index_deci <= 20) ? 100 : (uv_index_deci <= 50) ? 80 : (uv_index_deci <= 70) ? 60 : (uv_index_deci <= 100) ? 40 : 20;

            uint32_t weighted_sum = (co2_score * WEIGHT_CO2) + (tvoc_score * WEIGHT_TVOC) + (hum_score * WEIGHT_HUMIDITY) + (uv_score * WEIGHT_UV);
            quality_index = weighted_sum / (WEIGHT_CO2 + WEIGHT_TVOC + WEIGHT_HUMIDITY + WEIGHT_UV);
        }

        const char* estado_clima = determinar_estado_clima(lux, hum_pct, uv_index_deci);

        PORTD &= ~((1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7));
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4));

        if (error_flag) {
            PORTD |= (1 << PD0);
        } else {
            PORTD &= ~(1 << PD0);
            if (quality_index > 85) PORTD |= (1 << PD3);
            else if (quality_index > 65) PORTD |= (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
            else if (quality_index > 45) PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
            else PORTB |= (1 << PB3) | (1 << PB4);
        }

        update_display(error_flag, quality_index, estado_clima);

        _delay_ms(LOOP_DELAY_MS);
    }
    return 0;
}

void setup_peripherals(void) {
    i2c_init();
    ADC_init();
    OLED_init();
    BH1750_init();
    BMP280_init();
    DHT_Setup();

    DDRD |= (1 << PD0) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);

    OLED_clear();
    OLED_print(0, 0, "Inicializando...");

    if (SGP30_init()) {
        OLED_print(2, 0, "SGP30 OK");
        OLED_print(3, 0, "Calentando...");
        _delay_ms(3000);
    } else {
        OLED_print(2, 0, "SGP30 ERROR!");
        while(1);
    }
    OLED_clear();
}

const char* determinar_estado_clima(uint16_t lux, uint8_t hum, uint16_t uv_deci) {
    const uint16_t UMBRAL_LUZ_SOL = 15000;
    const uint16_t UMBRAL_LUZ_PARCIAL = 2000;
    const uint16_t UMBRAL_LUZ_OSCURO = 50;
    const uint8_t  UMBRAL_HUMEDAD_LLUVIA = 85;
    const uint16_t UMBRAL_UV_SOL_DIRECTO = 30;

    if (lux < UMBRAL_LUZ_OSCURO) {
        return "Noche / Oscuro";
    }
    if (hum > UMBRAL_HUMEDAD_LLUVIA) {
        return "Posible Lluvia";
    }
    if (lux > UMBRAL_LUZ_SOL && uv_deci > UMBRAL_UV_SOL_DIRECTO) {
        return "Dia Muy Soleado";
    }
    if (lux > UMBRAL_LUZ_SOL) {
        return "Dia Soleado";
    }
    if (lux > UMBRAL_LUZ_PARCIAL) {
        return "Parcial. Nublado";
    }
    return "Dia Nublado";
}

void update_display(uint8_t error_flag, uint16_t quality_index, const char* estado_clima)
{
    static uint8_t last_error_state = 2;
    static uint16_t last_quality = 999;
    static const char* last_estado_clima = NULL;

    char buffer[22];

    if (error_flag != last_error_state) {
        OLED_clear();
        last_error_state = error_flag;
    }

    if (error_flag) {
        OLED_print(2, 0, " FALLO DEL SISTEMA ");
        OLED_print(4, 0, "  Error al leer un  ");
        OLED_print(5, 0, "       sensor       ");
        
        last_quality = 999; 
        last_estado_clima = NULL;

    } else {
        if (quality_index != last_quality) {
            const char* calidad_texto;
            if (quality_index > 85)      calidad_texto = "Muy Buena";
            else if (quality_index > 65) calidad_texto = "Buena";
            else if (quality_index > 45) calidad_texto = "Regular";
            else                         calidad_texto = "Mala";
            
            OLED_print(1, 0, "Calidad del Aire:");
            snprintf(buffer, sizeof(buffer), "    %-14s", calidad_texto);
            OLED_print(2, 0, buffer);
            
            last_quality = quality_index;
        }

        if (estado_clima != last_estado_clima) {
            OLED_print(5, 0, "Estado del Clima:");
            snprintf(buffer, sizeof(buffer), " %-19s", estado_clima);
            OLED_print(6, 0, buffer);

            last_estado_clima = estado_clima;
        }
    }
}


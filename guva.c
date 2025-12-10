#include "guva.h"
#include "adc.h"

// Constante para la conversión, calculada como (VREF * 10) / RESOLUCION_ADC
// Para VREF=5V y ADC 10-bit: (5.0 * 10) / 1023.0 = 0.0488758
#define UV_CONVERSION_FACTOR (50.0 / 1023.0)

void GUVA_init(void) {
    // No se requiere inicialización específica para el sensor en sí,
    // pero nos aseguramos de que el ADC esté inicializado en el main.
}

float GUVA_read_uv_index(uint8_t adc_channel) {
    uint16_t adc_value = ADC_read(adc_channel);
    float uv_index = (float)adc_value * UV_CONVERSION_FACTOR;
    return uv_index;
}

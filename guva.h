#ifndef GUVA_S12SD_H_
#define GUVA_S12SD_H_

#include <stdint.h>

void GUVA_init(void);
float GUVA_read_uv_index(uint8_t adc_channel);

#endif 

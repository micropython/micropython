#ifndef MICROPY_INCLUDED_MACHINE_ADCBLOCK_H
#define MICROPY_INCLUDED_MACHINE_ADCBLOCK_H

#include "esp_adc_cal.h"

typedef struct _madcblock_obj_t {
    mp_obj_base_t base;
    adc_unit_t unit_id;
    mp_int_t bits;
    adc_bits_width_t width;
    esp_adc_cal_characteristics_t *characteristics[ADC_ATTEN_MAX];
} madcblock_obj_t;

extern madcblock_obj_t madcblock_obj[];

extern void madcblock_bits_helper(madcblock_obj_t *self, mp_int_t bits);
extern mp_int_t madcblock_read_helper(madcblock_obj_t *self, adc_channel_t channel_id);
extern mp_int_t madcblock_read_uv_helper(madcblock_obj_t *self, adc_channel_t channel_id, adc_atten_t atten);

#endif // MICROPY_INCLUDED_MACHINE_ADCBLOCK_H

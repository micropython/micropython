#ifndef MICROPY_INCLUDED_MACHINE_ADC_H
#define MICROPY_INCLUDED_MACHINE_ADC_H

#include "machine_adcblock.h"

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    madcblock_obj_t *block;
    adc_channel_t channel_id;
    gpio_num_t gpio_id;
} machine_adc_obj_t;

const machine_adc_obj_t *madc_search_helper(madcblock_obj_t *block, adc_channel_t channel_id, gpio_num_t gpio_id);
void madc_init_helper(const machine_adc_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

#endif // MICROPY_INCLUDED_MACHINE_ADC_H

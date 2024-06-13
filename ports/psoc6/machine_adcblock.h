#ifndef MICROPY_INCLUDED_MACHINE_ADCBLOCK_H
#define MICROPY_INCLUDED_MACHINE_ADCBLOCK_H

// These should go into the specific BSP
#define DEFAULT_ADC_BITS        12
#define ADC_BLOCK_CHANNEL_MAX   6
#define DEFAULT_ADC_ACQ_NS      1000

#define ADCBLOCK0               (0)
#define ADCBLOCK_CHANNEL_MAX    (1)
#define MAX_BLOCKS              (1)
#define MAX_CHANNELS            (6)

typedef struct _machine_adc_obj_t machine_adc_obj_t; /* Forward declaration of adc_obj */

typedef struct _machine_adcblock_obj_t {
    mp_obj_base_t base;
    cyhal_adc_t adc_obj;
    uint8_t id;
    uint8_t bits;
    machine_adc_obj_t *channel[ADC_BLOCK_CHANNEL_MAX];
} machine_adcblock_obj_t;

typedef struct
{
    uint16_t block_id;
    uint16_t channel;
    uint32_t pin;
}adc_block_channel_pin_map_t;

void mod_adcblock_deinit(void);
#endif // MICROPY_INCLUDED_MACHINE_ADCBLOCK_H

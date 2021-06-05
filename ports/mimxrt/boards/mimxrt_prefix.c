#include <stdio.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "pin.h"

#define PIN_AF(_name, _af_mode, _instance, _pad_config) \
    { \
        .base = { &machine_pin_af_type }, \
        .name = MP_QSTR_##_name, \
        .af_mode = (uint32_t)(_af_mode), \
        .instance = (void *)(_instance), \
        .pad_config = (uint32_t)(_pad_config), \
    } \

#define PIN_ADC(_instance, _channel) \
    { \
        .instance = (_instance), \
        .channel = (_channel) \
    } \

#define PIN(_name, _gpio, _pin, _af_list, _adc_list_len, _adc_list) \
    { \
        .base = { &machine_pin_type }, \
        .name = MP_QSTR_##_name, \
        .gpio = (_gpio), \
        .pin = (uint32_t)(_pin), \
        .muxRegister = (uint32_t)&(IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_##_name]), \
        .configRegister = (uint32_t)&(IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_##_name]), \
        .af_list_len = (uint8_t)(sizeof((_af_list)) / sizeof(machine_pin_af_obj_t)), \
        .adc_list_len = (_adc_list_len), \
        .af_list = (_af_list), \
        .adc_list = (_adc_list), \
    } \

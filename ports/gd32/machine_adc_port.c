// machine_adc_port.c - machine.ADC for GD32F4xx
// Included by extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.
// The extmod layer owns machine_adc_type; this file only provides hooks.

#include "gd32f4xx.h"
#include "py/mperrno.h"
#include "pin_defs.h"

#define ADC_MAX_OBJ   (16)

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    uint8_t unit;
    uint8_t channel;
    int16_t pin;
} machine_adc_obj_t;

static machine_adc_obj_t machine_adc_pool[ADC_MAX_OBJ];
static uint8_t machine_adc_used = 0;

// PA0-PA7 -> ch0-7, PB0-PB1 -> ch8-9, PC0-PC5 -> ch10-15
static int pin_to_adc_channel(uint8_t pin_enc) {
    uint8_t port = PIN_PORT(pin_enc);
    uint8_t num = PIN_NUM(pin_enc);
    if (port == 0 && num <= 7) {
        return num;
    }
    if (port == 1 && num <= 1) {
        return 8 + num;
    }
    if (port == 2 && num <= 5) {
        return 10 + num;
    }
    return -1;
}

static void adc_unit_prepare(uint8_t unit) {
    uint32_t adc = (unit == 0) ? ADC0 : (unit == 1) ? ADC1 : ADC2;
    static uint8_t ready = 0;
    uint8_t mask = (uint8_t)(1u << unit);
    if (ready & mask) {
        return;
    }

    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
    switch (unit) {
        case 0: rcu_periph_clock_enable(RCU_ADC0); break;
        case 1: rcu_periph_clock_enable(RCU_ADC1); break;
        default: rcu_periph_clock_enable(RCU_ADC2); break;
    }

    adc_deinit();
    adc_special_function_config(adc, ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(adc, ADC_CONTINUOUS_MODE, DISABLE);
    adc_data_alignment_config(adc, ADC_DATAALIGN_RIGHT);
    adc_resolution_config(adc, ADC_RESOLUTION_12B);
    adc_enable(adc);
    mp_hal_delay_ms(1);
    adc_calibration_enable(adc);
    ready |= mask;
}

static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type, size_t n_args,
                                        size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_int_t pin_enc = mp_obj_get_int(args[0]);
    int ch = pin_to_adc_channel((uint8_t)pin_enc);
    if (ch < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("pin has no ADC channel"));
    }
    if (machine_adc_used >= ADC_MAX_OBJ) {
        mp_raise_ValueError(MP_ERROR_TEXT("too many ADC objects"));
    }

    machine_adc_obj_t *self = &machine_adc_pool[machine_adc_used++];
    self->base.type = type;
    self->unit = 0;
    self->channel = (uint8_t)ch;
    self->pin = (int16_t)pin_enc;

    // analog mode on the pin
    uint8_t port = PIN_PORT((uint8_t)pin_enc);
    uint8_t num = PIN_NUM((uint8_t)pin_enc);
    rcu_periph_clock_enable(pin_port_rcu(port));
    gpio_mode_set(pin_port_base(port), GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pin_hal_mask(num));

    // keep kw args accepted but unused for now
    (void)n_kw;

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADC(%u, channel=%u)", self->unit, self->channel);
}

static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
    adc_unit_prepare(self->unit);
    uint32_t adc = (self->unit == 0) ? ADC0 : (self->unit == 1) ? ADC1 : ADC2;

    adc_channel_length_config(adc, ADC_ROUTINE_CHANNEL, 1);
    adc_routine_channel_config(adc, 0, self->channel, ADC_SAMPLETIME_480);
    adc_software_trigger_enable(adc, ADC_ROUTINE_CHANNEL);

    mp_uint_t start = mp_hal_ticks_ms();
    while (adc_flag_get(adc, ADC_FLAG_EOC) == RESET) {
        if ((mp_hal_ticks_ms() - start) > 100) {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
    }
    adc_flag_clear(adc, ADC_FLAG_EOC);
    return (mp_int_t)adc_routine_data_read(adc);
}

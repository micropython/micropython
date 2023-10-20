
#include "common-hal/countio/Counter.h"
#include "shared-bindings/countio/Counter.h"

#include "atmel_start_pins.h"

#include "eic_handler.h"
#include "samd/external_interrupts.h"
#include "py/runtime.h"

void common_hal_countio_counter_construct(countio_counter_obj_t *self,
    const mcu_pin_obj_t *pin, countio_edge_t edge, digitalio_pull_t pull) {
    if (!pin->has_extint) {
        mp_raise_RuntimeError(translate("Pin must support hardware interrupts"));
    }


    if (eic_get_enable()) {
        if (!eic_channel_free(pin->extint_channel)) {
            mp_raise_RuntimeError(translate("A hardware interrupt channel is already in use"));
        }
    } else {
        turn_on_external_interrupt_controller();
    }

    // These default settings apply when the EIC isn't yet enabled.
    self->eic_channel = pin->extint_channel;

    self->pin = pin->number;

    gpio_set_pin_function(self->pin, GPIO_PIN_FUNCTION_A);

    enum gpio_pull_mode asf_pull = GPIO_PULL_OFF;
    switch (pull) {
        case PULL_UP:
            asf_pull = GPIO_PULL_UP;
            break;
        case PULL_DOWN:
            asf_pull = GPIO_PULL_DOWN;
            break;
        case PULL_NONE:
        default:
            break;
    }
    gpio_set_pin_pull_mode(self->pin, asf_pull);

    set_eic_channel_data(self->eic_channel, (void *)self);

    self->count = 0;
    claim_pin(pin);

    set_eic_handler(self->eic_channel, EIC_HANDLER_COUNTER);

    uint32_t sense_setting = EIC_CONFIG_SENSE0_BOTH_Val;
    switch (edge) {
        case EDGE_RISE:
            sense_setting = EIC_CONFIG_SENSE0_RISE_Val;
            break;
        case EDGE_FALL:
            sense_setting = EIC_CONFIG_SENSE0_FALL_Val;
            break;
        case EDGE_RISE_AND_FALL:
        default:
            break;
    }
    turn_on_eic_channel(self->eic_channel, sense_setting);
}

bool common_hal_countio_counter_deinited(countio_counter_obj_t *self) {
    return self->pin == NO_PIN;
}

void common_hal_countio_counter_deinit(countio_counter_obj_t *self) {
    if (common_hal_countio_counter_deinited(self)) {
        return;
    }

    set_eic_handler(self->eic_channel, EIC_HANDLER_NO_INTERRUPT);
    turn_off_eic_channel(self->eic_channel);


    reset_pin_number(self->pin);
    self->pin = NO_PIN;

}

mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t *self) {
    return self->count;
}

void common_hal_countio_counter_set_count(countio_counter_obj_t *self,
    mp_int_t new_count) {
    self->count = new_count;
}

void counter_interrupt_handler(uint8_t channel) {
    countio_counter_obj_t *self = get_eic_channel_data(channel);

    self->count += 1;

}

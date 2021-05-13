
#include "common-hal/countio/Counter.h"

#include "atmel_start_pins.h"

#include "eic_handler.h"
#include "samd/external_interrupts.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

void common_hal_countio_counter_construct(countio_counter_obj_t *self,
    const mcu_pin_obj_t *pin_a) {
    if (!pin_a->has_extint) {
        mp_raise_RuntimeError(translate("Pin must support hardware interrupts"));
    }


    if (eic_get_enable()) {
        if (!eic_channel_free(pin_a->extint_channel)) {
            mp_raise_RuntimeError(translate("A hardware interrupt channel is already in use"));
        }
    } else {
        turn_on_external_interrupt_controller();
    }

    // These default settings apply when the EIC isn't yet enabled.
    self->eic_channel_a = pin_a->extint_channel;

    self->pin_a = pin_a->number;

    gpio_set_pin_function(self->pin_a, GPIO_PIN_FUNCTION_A);
    gpio_set_pin_pull_mode(self->pin_a, GPIO_PULL_UP);

    set_eic_channel_data(self->eic_channel_a, (void *)self);

    self->count = 0;


    claim_pin(pin_a);


    set_eic_handler(self->eic_channel_a, EIC_HANDLER_COUNTER);
    turn_on_eic_channel(self->eic_channel_a, EIC_CONFIG_SENSE0_FALL_Val);

}

bool common_hal_countio_counter_deinited(countio_counter_obj_t *self) {
    return self->pin_a == NO_PIN;
}

void common_hal_countio_counter_deinit(countio_counter_obj_t *self) {
    if (common_hal_countio_counter_deinited(self)) {
        return;
    }

    set_eic_handler(self->eic_channel_a, EIC_HANDLER_NO_INTERRUPT);
    turn_off_eic_channel(self->eic_channel_a);


    reset_pin_number(self->pin_a);
    self->pin_a = NO_PIN;

}

mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t *self) {
    return self->count;
}

void common_hal_countio_counter_set_count(countio_counter_obj_t *self,
    mp_int_t new_count) {
    self->count = new_count;
}

void common_hal_countio_counter_reset(countio_counter_obj_t *self) {
    self->count = 0;
}

void counter_interrupt_handler(uint8_t channel) {
    countio_counter_obj_t *self = get_eic_channel_data(channel);

    self->count += 1;

}

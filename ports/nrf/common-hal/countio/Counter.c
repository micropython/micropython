
#include "common-hal/countio/Counter.h"
#include "nrfx_gpiote.h"

// obj array to map pin number -> self since nrfx hide the mapping
static countio_counter_obj_t *_countio_objs[NUMBER_OF_PINS];

static void _intr_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    countio_counter_obj_t *self = _countio_objs[pin];
    if (!self) {
        return;
    }
    self->count += 1;
}

void common_hal_countio_counter_construct(countio_counter_obj_t *self,
    const mcu_pin_obj_t *pin_a) {

    self->pin_a = pin_a->number;
    _countio_objs[self->pin_a] = self;

    self->count = 0;

    nrfx_gpiote_in_config_t cfg = {
        .sense = NRF_GPIOTE_POLARITY_HITOLO,
        .pull = NRF_GPIO_PIN_PULLUP,
        .is_watcher = false,
        .hi_accuracy = true,
        .skip_gpio_setup = false
    };

    nrfx_gpiote_in_init(self->pin_a, &cfg, _intr_handler);
    nrfx_gpiote_in_event_enable(self->pin_a, true);

    claim_pin(pin_a);

}

bool common_hal_countio_counter_deinited(countio_counter_obj_t *self) {
    return self->pin_a == NO_PIN;
}

void common_hal_countio_counter_deinit(countio_counter_obj_t *self) {
    if (common_hal_countio_counter_deinited(self)) {
        return;
    }
    _countio_objs[self->pin_a] = NULL;

    nrfx_gpiote_in_event_disable(self->pin_a);
    nrfx_gpiote_in_uninit(self->pin_a);
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

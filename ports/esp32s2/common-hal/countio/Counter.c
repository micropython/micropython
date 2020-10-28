#include "common-hal/countio/Counter.h"

#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "driver/pcnt.h"

static void pcnt_init(countio_counter_obj_t* self) {
    int unit = PCNT_UNIT_0;
    // Prepare configuration for the PCNT unit
    pcnt_config_t pcnt_config = {
        // Set PCNT input signal and control GPIOs
        .pulse_gpio_num = self->pin->number,
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,
        .channel = PCNT_CHANNEL_0,
        .unit = unit,
        // What to do on the positive / negative edge of pulse input?
        .pos_mode = PCNT_COUNT_INC,   // Count up on the positive edge
        .neg_mode = PCNT_COUNT_DIS,   // Keep the counter value on the negative edge
    };
    // Initialize PCNT unit
    pcnt_unit_config(&pcnt_config);

    // Configure and enable the input filter
    pcnt_set_filter_value(unit, 100);
    pcnt_filter_enable(unit);

    // Initialize PCNT's counter
    pcnt_counter_pause(unit);
    pcnt_counter_clear(unit);

    // Everything is set up, now go to counting
    pcnt_counter_resume(unit);
}

void common_hal_countio_counter_construct(countio_counter_obj_t* self,
    const mcu_pin_obj_t* pin) {
    claim_pin(pin);
    self->pin = pin;
    pcnt_init(self);
}

bool common_hal_countio_counter_deinited(countio_counter_obj_t* self) {
    return self->pin == NULL;
}

void common_hal_countio_counter_deinit(countio_counter_obj_t* self) {
    if (common_hal_countio_counter_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t* self) {
    int16_t count;
    pcnt_get_counter_value(PCNT_UNIT_0, &count);
    return count+self->count;
}

void common_hal_countio_counter_set_count(countio_counter_obj_t* self,
        mp_int_t new_count) {
    self->count = new_count;
    pcnt_counter_clear(PCNT_UNIT_0);
}

void common_hal_countio_counter_reset(countio_counter_obj_t* self) {
   common_hal_countio_counter_set_count(self, 0);
}

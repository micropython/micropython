#include "shared-bindings/countio/Counter.h"

#include "py/runtime.h"
#include "py/mpstate.h"
#include "supervisor/shared/translate.h"

#include "common-hal/pwmio/PWMOut.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "src/rp2_common/hardware_irq/include/hardware/irq.h"


void common_hal_countio_counter_construct(countio_counter_obj_t *self,
    const mcu_pin_obj_t *pin_a) {

    if (pwm_gpio_to_channel(pin_a->number) != PWM_CHAN_B) {
        mp_raise_RuntimeError(translate("Pin must be on PWM Channel B"));
    }

    self->pin_a = pin_a->number;
    self->slice_num = pwm_gpio_to_slice_num(self->pin_a);

    if (MP_STATE_PORT(counting)[self->slice_num] != NULL) {
        mp_raise_RuntimeError(translate("PWM slice already in use"));
    }

    uint8_t ab_channel = pwm_gpio_to_channel(self->pin_a);
    if (!pwmio_claim_slice_ab_channels(self->slice_num)) {
        mp_raise_RuntimeError(translate("PWM slice channel A already in use"));
    }

    pwm_clear_irq(self->slice_num);
    pwm_set_irq_enabled(self->slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, counter_interrupt_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
    pwm_init(self->slice_num, &cfg, false);
    gpio_set_function(self->pin_a, GPIO_FUNC_PWM);

    claim_pin(pin_a);

    MP_STATE_PORT(counting)[self->slice_num] = self;

    self->count = 0;
    pwm_set_enabled(self->slice_num, true);
}


void reset_countio(void) {
    for (size_t i = 0; i < NUM_PWM_SLICES; i++) {
        if (MP_STATE_PORT(counting)[i] != NULL) {
            common_hal_countio_counter_deinit(MP_STATE_PORT(counting)[i]);
        }
    }
}

bool common_hal_countio_counter_deinited(countio_counter_obj_t *self) {
    return self->pin_a == 0;
}

void common_hal_countio_counter_deinit(countio_counter_obj_t *self) {
    if (common_hal_countio_counter_deinited(self)) {
        return;
    }

    pwm_set_enabled(self->slice_num, false);
    pwm_set_irq_enabled(self->slice_num, false);

    pwmio_release_slice_ab_channels(self->slice_num);

    reset_pin_number(self->pin_a);

    MP_STATE_PORT(counting)[self->slice_num] = NULL;
    self->pin_a = 0;
    self->slice_num = 0;
}

mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t *self) {
    self->count += pwm_get_counter(self->slice_num);
    pwm_set_counter(self->slice_num, 0);
    return self->count;
}

void common_hal_countio_counter_set_count(countio_counter_obj_t *self,
    mp_int_t new_count) {
    pwm_set_counter(self->slice_num, 0);
    self->count = new_count;
}

void common_hal_countio_counter_reset(countio_counter_obj_t *self) {
    pwm_set_counter(self->slice_num, 0);
    self->count = 0;
}

void counter_interrupt_handler(void) {
    uint32_t mask = pwm_get_irq_status_mask();

    uint8_t i = 1;
    uint8_t pos = 0;
    while (!(i & mask)) {
        i = i << 1;
        ++pos;
    }

    countio_counter_obj_t *self = MP_STATE_PORT(counting)[pos];
    if (self != NULL) {
        pwm_clear_irq(self->slice_num);
        self->count += 65536;
    }
}

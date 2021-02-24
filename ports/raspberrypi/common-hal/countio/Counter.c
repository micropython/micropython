#include "common-hal/countio/Counter.h"

#include "py/runtime.h"
#include "py/mpstate.h"
#include "supervisor/shared/translate.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "src/rp2_common/hardware_irq/include/hardware/irq.h"


void common_hal_countio_counter_construct(countio_counter_obj_t* self,
    const mcu_pin_obj_t* pin_a) {

    if (pwm_gpio_to_channel(pin_a->number) != PWM_CHAN_B) {
        mp_raise_RuntimeError(translate("Pin must be on PWM Channel B"));
    }

    self->pin_a = pin_a->number;
    self->slice_num = pwm_gpio_to_slice_num(self->pin_a);

    pwm_clear_irq(self->slice_num);
    pwm_set_irq_enabled(self->slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, counter_interrupt_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
    pwm_init(self->slice_num, &cfg, false);
    gpio_set_function(self->pin_a, GPIO_FUNC_PWM);

    self->count = 0;

    claim_pin(pin_a);

    MP_STATE_PORT(counting) = self;

    pwm_set_enabled(self->slice_num, true);
}

bool common_hal_countio_counter_deinited(countio_counter_obj_t* self) {
    return self->pin_a == 0;
}

void common_hal_countio_counter_deinit(countio_counter_obj_t* self) {
    if (common_hal_countio_counter_deinited(self)) {
        return;
    }

    pwm_set_enabled(self->slice_num, false);
    pwm_set_irq_enabled(self->slice_num, false);

    reset_pin_number(self->pin_a);
    gpio_init(self->pin_a);
    self->pin_a = 0;
    self->slice_num = 0;
    MP_STATE_PORT(counting) = NULL;
}

mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t* self) {
    self->count += pwm_get_counter(self->slice_num);
    pwm_set_counter(self->slice_num, 0);
    return self->count;
}

void common_hal_countio_counter_set_count(countio_counter_obj_t* self,
        mp_int_t new_count) {
    pwm_set_counter(self->slice_num, 0);
    self->count = new_count;
}

void common_hal_countio_counter_reset(countio_counter_obj_t* self){
    pwm_set_counter(self->slice_num, 0);
    self->count = 0;
}

void counter_interrupt_handler() {
    countio_counter_obj_t *self = MP_STATE_PORT(counting);
    pwm_clear_irq(self->slice_num);
    self->count += 65536;
}

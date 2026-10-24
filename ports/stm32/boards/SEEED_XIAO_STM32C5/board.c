
// Link-time stubs for STM32C5 drivers not yet ported.
void timer_irq_handler(void *handle) {
    (void)handle;
}
void timer_init0(void) {
}
void timer_deinit(void) {
}
void spi_init0(void) {
}
void spi_deinit_all(void) {
}
void extint_init0(void) {
}

#include STM32_HAL_H
#include "boards/stm32c5xx_hal_compat.h"
TIM_HandleTypeDef TIM5_Handle;

#include "py/mphal.h"

void board_early_init(void) {
    // Keep the IMU heater and battery measurement circuit off.
    mp_hal_gpio_clock_enable(pin_A8->gpio);
    mp_hal_pin_low(pin_A8);
    mp_hal_pin_output(pin_A8);
    mp_hal_pin_low(pin_A15);
    mp_hal_pin_output(pin_A15);

    // Leave the onboard CAN transceiver in standby until explicitly used.
    mp_hal_gpio_clock_enable(pin_B14->gpio);
    mp_hal_pin_high(pin_B14);
    mp_hal_pin_output(pin_B14);
}

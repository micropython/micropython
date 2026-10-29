// Stub implementations for modules not yet ported to STM32C5.
// These provide link-time symbols so the build succeeds.

#include "py/mphal.h"

// Timer stubs
void timer_irq_handler(void *handle) {
    (void)handle;
}
void timer_init0(void) {
}
void timer_deinit(void) {
}

// SPI stubs
void spi_init0(void) {
}
void spi_deinit_all(void) {
}

// EXTINT stubs
void extint_init0(void) {
}


// Timer handle stub (referenced by stm32_it.c IRQ handlers)
#include STM32_HAL_H
#include "boards/stm32c5xx_hal_compat.h"
TIM_HandleTypeDef TIM5_Handle;

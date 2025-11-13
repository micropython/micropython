#include_next "hardware/pio.h"

#ifndef MP_HARDWARE_PIO_H
#define MP_HARDWARE_PIO_H

#ifdef __cplusplus
extern "C" {
#endif

#if PICO_PIO_USE_GPIO_BASE
void pio_sm_set_pins_internal(PIO pio, uint sm, uint32_t pins);
void pio_sm_set_pins_with_mask_internal(PIO pio, uint sm, uint32_t pin_values, uint32_t pin_mask);
void pio_sm_set_pindirs_with_mask_internal(PIO pio, uint sm, uint32_t pin_dirs, uint32_t pin_mask);
#else
#define pio_sm_set_pins_internal pio_sm_set_pins
#define pio_sm_set_pins_with_mask_internal pio_sm_set_pins_with_mask
#define pio_sm_set_pindirs_with_mask_internal pio_sm_set_pindirs_with_mask
#endif

#ifdef __cplusplus
}
#endif

#endif

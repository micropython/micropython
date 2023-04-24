#ifndef MICROPY_INCLUDED_PSOC6_PIN_H
#define MICROPY_INCLUDED_PSOC6_PIN_H

// cy includes
#include "cybsp.h"
#include "cyhal.h"
#include "cy_pdl.h"

// std includes
#include <stdint.h>


// Note: Used to define gpio value QSTRs
// Note: not in MPY guidelines but may needed later for board-specific active-low/active-high pins
#define GPIO_STATE_ON                   (0U)
#define GPIO_STATE_OFF                  (1U)

#define gpio_init_success               CY_RSLT_SUCCESS
#define gpio_init_rslt                  cy_rslt_t
#define gpio_pin_t                      cyhal_gpio_t
#define gpio_pin_dir_t                  cyhal_gpio_direction_t
#define gpio_pin_drive_mode_t           cyhal_gpio_drive_mode_t

// function to pass params to HAL init function
gpio_init_rslt gpio_init(cyhal_gpio_t pin, cyhal_gpio_direction_t direction, cyhal_gpio_drive_mode_t drive_mode, bool init_val);

// function to get HSIOM config of a pin
en_hsiom_sel_t pin_get_hsiom_func(uint32_t pin);

// function to get cypdl drive modes, correlated to cyhal drive modes in file: cyhal_gpio.c
// Note: drive modes are enumed in cy_gpio.h and are also distinguised for pins with input buffer on or off (configured as input or output respectively)
uint32_t gpio_get_drive(uint32_t pin);

// function to check if pin is in mode Pin.OPEN_DRAIN.
// drive comparisons done with PDL drive modes since function is from PDL (not HAL)
bool gpio_is_open_drain(uint32_t pin);

// function to check if pin is in mode Pin.OUT; TODO: can be also implemented by checking input buffer on/off
bool gpio_is_out(uint32_t pin);

// function to check if pin is in mode Pin.IN; TODO: can be also implemented by checking input buffer on/off
bool gpio_is_in(uint32_t pin);

// function to check if pin has pull Pin.PULL_UP
bool gpio_is_pull_up(uint32_t pin);

// function to check if pin has pull Pin.PULL_DOWN
bool gpio_is_pull_down(uint32_t pin);

// function to check Pin.value
uint8_t gpio_get_value(uint32_t pin);

// function to call Pin.value only for pins with mode Pin.IN; used for __call__ function
// uses mp_const_none type for None/undefined return
int8_t gpio_get_value_call(uint32_t pin);

// function to set Pin.value to 1; sets the output buffer which drives the output driver
void gpio_set_value(uint32_t pin);

// function to set Pin.value to 0; clear the output buffer which drives the output driver
void gpio_clear_value(uint32_t pin);

// function to toggle Pin.value; toggle the output buffer which drives the output driver
void gpio_toggle_value(uint32_t pin);

#endif // MICROPY_INCLUDED_PSOC6_PIN_H

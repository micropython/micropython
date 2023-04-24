/* CYHAL GPIO functions */

// std includes
#include <stdbool.h>


// port-specific includes
#include "psoc6_gpio.h"


gpio_init_rslt gpio_init(cyhal_gpio_t pin, cyhal_gpio_direction_t direction, cyhal_gpio_drive_mode_t drive_mode, bool init_val) {
    // Free up the cy gpio obj
    // this function checks if the pin is init'd before and only frees it if so.
    // else does nothing
    cyhal_gpio_free(pin);
    // Then do actual gpio init (or reinit)
    return cyhal_gpio_init(pin, direction, drive_mode, init_val);
}

// function to get HSIOM config of a pin
en_hsiom_sel_t pin_get_hsiom_func(uint32_t pin) {
    return Cy_GPIO_GetHSIOM(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
}

// function to get cypdl drive modes, correlated to cyhal drive modes in file: cyhal_gpio.c
// Note: drive modes are enumed in cy_gpio.h and are also distinguised for pins with input buffer on or off (configured as input or output respectively)
uint32_t gpio_get_drive(uint32_t pin) {
    return Cy_GPIO_GetDrivemode(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
}

// function to check if pin is in mode Pin.OPEN_DRAIN.
// drive comparisons done with PDL drive modes since wrapped function is from PDL (not HAL)
// Note: MPY only allows OPEN DRAIN and DRIVE LOW (0/Z) mode to be set from the machine.Pin class methods, as per ts documentations, -
// but in some applications (such as i2c etc), the application internally might also set OPEN DRAIN and DRIVE HIGH (1/Z) mode. Hence both the modes -
// must be checked for, while checking for open drain.
// Additionally, open drain can be implemented with the GPIO INPUT BUFFER on or off, hence both of those cases must be checked too.
// More info on pg. 245 of PSoC_6_MCU_PSoC_62_Architecture_Technical_Reference_Manual_v8_EN
bool gpio_is_open_drain(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_OD_DRIVESLOW || gpio_get_drive(pin) == CY_GPIO_DM_OD_DRIVESHIGH || gpio_get_drive(pin) == CY_GPIO_DM_OD_DRIVESLOW_IN_OFF || gpio_get_drive(pin) == CY_GPIO_DM_OD_DRIVESHIGH_IN_OFF) {
        return true;
    } else {
        return false;
    }
}

// function to check if pin is in mode Pin.OUT; TODO: can be also implemented by checking input buffer on/off
bool gpio_is_out(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_STRONG_IN_OFF) { // pin cfgd as o/p drive so Input buffer is off.
        return true;
    } else {
        return false;
    }
}

// function to check if pin is in mode Pin.IN; TODO: can be also implemented by checking input buffer on/off
bool gpio_is_in(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_HIGHZ) {
        return true;
    } else {
        return false;
    }
}


// function to check if pin has pull Pin.PULL_UP
bool gpio_is_pull_up(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_PULLUP_IN_OFF || gpio_get_drive(pin) == CY_GPIO_DM_PULLUP) {
        return true;
    } else {
        return false;
    }
}

// function to check if pin has pull Pin.PULL_DOWN
bool gpio_is_pull_down(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_PULLDOWN_IN_OFF || gpio_get_drive(pin) == CY_GPIO_DM_PULLDOWN) {
        return true;
    } else {
        return false;
    }
}

// function to check Pin.value
uint8_t gpio_get_value(uint32_t pin) {
    if (gpio_is_out(pin)) { // if Pin.Mode is Pin.OUT, read output driver
        return Cy_GPIO_ReadOut(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
    } else { // if Pin.Mode is Pin.IN, read pin.
        return Cy_GPIO_Read(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
    }
}

// function to call Pin.value only for pins with mode Pin.IN; used for __call__ function
// uses mp_const_none type for None/undefined return
int8_t gpio_get_value_call(uint32_t pin) {
    if (gpio_is_in(pin)) { // if Pin.Mode is Pin.IN, return current pin input value
        return Cy_GPIO_Read(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
    } else if (gpio_is_out(pin)) { // if Pin.Mode is Pin.OUT, then return is undefined
        return -1; // undefined
    } else { // Pin.Mode is Pin.OPEN_DRAIN
        if (Cy_GPIO_ReadOut(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin)) == 0) {  // if 0 is driven in open_drain, then undefined
            return -1;     // undefined
        } else {
            return Cy_GPIO_Read(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
        }
    }
}

// function to set Pin.value to 1; sets the output buffer which drives the output driver
void gpio_set_value(uint32_t pin) {
    return Cy_GPIO_Set(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
}

// function to set Pin.value to 0; clear the output buffer which drives the output driver
void gpio_clear_value(uint32_t pin) {
    return Cy_GPIO_Clr(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
}

// function to toggle Pin.value; toggle the output buffer which drives the output driver
void gpio_toggle_value(uint32_t pin) {
    return Cy_GPIO_Inv(CYHAL_GET_PORTADDR(pin), CYHAL_GET_PIN(pin));
}

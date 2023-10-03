// micropython includes
#include "py/mphal.h"
#include "py/runtime.h"


// port-specific includes
#include "modmachine.h"
#include "machine_pin.h"
#include "extmod/virtpin.h"
#include "mplogger.h"
#include "cyhal.h"
#include "cy_gpio.h"

// enums to map MPY parameters to CYHAL params to avoid confusion
// these are used as returns from functions and comparisons to MP_QSTRs in mapping functions
// enum to hold pin modes
enum {GPIO_MODE_NONE = 0, GPIO_MODE_IN, GPIO_MODE_OUT, GPIO_MODE_OPEN_DRAIN}; // GPIO_MODE_ALT, GPIO_MODE_ALT_OPEN_DRAIN, GPIO_MODE_ANALOG};

// enum to hold pin drive strengths
// enum {GPIO_DRIVE_NONE = 0, GPIO_DRIVE_CAP_0, GPIO_DRIVE_CAP_1};

// enum to hold pulls
enum {GPIO_PULL_NONE = 0, GPIO_PULL_UP, GPIO_PULL_DOWN};

// // enum for alt functions
// enum {HSIOM_GPIO_FUNC}; // see file gpio_psoc6_02_124_bga.h

// // enums and structs to handle ARGS in MPY

static machine_pin_phy_obj_t machine_pin_phy_obj[] = {
    {PIN_P0_0, "P0_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P0_1, "P0_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P0_2, "P0_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P0_3, "P0_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P0_4, "P0_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P0_5, "P0_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P1_0, "P1_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P1_1, "P1_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P1_2, "P1_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P1_3, "P1_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P1_4, "P1_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P1_5, "P1_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P2_0, "P2_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P2_1, "P2_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P2_2, "P2_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P2_3, "P2_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P2_4, "P2_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P2_5, "P2_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P2_6, "P2_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P2_7, "P2_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P3_0, "P3_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P3_1, "P3_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P3_2, "P3_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P3_3, "P3_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P3_4, "P3_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P3_5, "P3_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P4_0, "P4_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P4_1, "P4_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P5_0, "P5_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P5_1, "P5_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P5_2, "P5_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P5_3, "P5_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P5_4, "P5_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P5_5, "P5_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P5_6, "P5_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P5_7, "P5_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P6_0, "P6_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P6_1, "P6_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P6_2, "P6_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P6_3, "P6_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P6_4, "P6_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P6_5, "P6_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P6_6, "P6_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P6_7, "P6_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P7_0, "P7_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P7_1, "P7_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P7_2, "P7_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P7_3, "P7_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P7_4, "P7_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P7_5, "P7_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P7_6, "P7_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P7_7, "P7_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P8_0, "P8_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P8_1, "P8_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P8_2, "P8_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P8_3, "P8_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P8_4, "P8_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P8_5, "P8_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P8_6, "P8_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P8_7, "P8_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P9_0, "P9_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P9_1, "P9_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P9_2, "P9_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P9_3, "P9_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P9_4, "P9_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P9_5, "P9_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P9_6, "P9_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P9_7, "P9_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P10_0, "P10_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P10_1, "P10_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P10_2, "P10_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P10_3, "P10_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P10_4, "P10_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P10_5, "P10_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P10_6, "P10_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P10_7, "P10_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P11_0, "P11_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P11_1, "P11_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P11_2, "P11_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P11_3, "P11_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P11_4, "P11_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P11_5, "P11_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P11_6, "P11_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P11_7, "P11_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P12_0, "P12_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P12_1, "P12_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P12_2, "P12_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P12_3, "P12_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P12_4, "P12_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P12_5, "P12_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P12_6, "P12_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P12_7, "P12_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_P13_0, "P13_0", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P13_1, "P13_1", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P13_2, "P13_2", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P13_3, "P13_3", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P13_4, "P13_4", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P13_5, "P13_5", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P13_6, "P13_6", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_P13_7, "P13_7", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},

    {PIN_USBDP, "USBDP", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE},
    {PIN_USBDM, "USBDM", (PIN_PHY_FUNC_DIO), PIN_PHY_FUNC_NONE}
};

#define pin_phy_assert_null(x) { if (x == NULL) { return NULL; } }

machine_pin_phy_obj_t *pin_phy_find_by_name(mp_obj_t pin_name) {
    size_t slen;
    const char *s = mp_obj_str_get_data(pin_name, &slen);
    for (int i = 0; i < MP_ARRAY_SIZE(machine_pin_phy_obj); i++) {
        if (slen == strlen(machine_pin_phy_obj[i].name) && strncmp(s, machine_pin_phy_obj[i].name, slen) == 0) {
            return &machine_pin_phy_obj[i];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid pin: Pin not defined!"));
    return NULL;
}

bool pin_phy_is_alloc(machine_pin_phy_obj_t *pin_phy) {
    if (pin_phy->allocated_func != PIN_PHY_FUNC_NONE) {
        return true;
    }
    return false;
}

void pin_phy_allocate_obj(machine_pin_phy_obj_t *pin_phy, machine_pin_phy_func_t func) {
    if ((pin_phy->available_funcs & func) == func) {
        pin_phy->allocated_func = func;
    }
}

machine_pin_phy_obj_t *pin_phy_alloc(mp_obj_t pin_name, machine_pin_phy_func_t func) {
    machine_pin_phy_obj_t *pin_phy = pin_phy_find_by_name(pin_name);
    pin_phy_assert_null(pin_phy);

    pin_phy_allocate_obj(pin_phy, func);
    return pin_phy;
}

machine_pin_phy_obj_t *pin_phy_realloc(mp_obj_t pin_name, machine_pin_phy_func_t func) {
    machine_pin_phy_obj_t *pin_phy = pin_phy_find_by_name(pin_name);
    pin_phy_assert_null(pin_phy);

    if (pin_phy_is_alloc(pin_phy)) {
        pin_phy_free(pin_phy);
    }

    pin_phy_allocate_obj(pin_phy, func);

    return pin_phy;
}

void pin_phy_free(machine_pin_phy_obj_t *pin_phy) {
    // machine_pin_phy_func_t current_func = pin_phy->allocated_func;
    pin_phy->allocated_func = PIN_PHY_FUNC_NONE;

    // TODO: each module should provide its own instance deinitialization
    // We can not just free the physical_pin, whithout deiniting its corresponding
    // peripheral object
    // machine_mod_deinit[func](pin_phy);
}

// Function definitions
// helper function to translate pin_name(string) into machine_pin_io_obj_t index.
int pin_find(mp_obj_t pin) {
    int wanted_pin = -1;
    if (mp_obj_is_small_int(pin)) {
        // Pin defined by the index of pin table
        wanted_pin = mp_obj_get_int(pin);
    } else if (mp_obj_is_str(pin)) {
        // Search by name
        size_t slen;
        const char *s = mp_obj_str_get_data(pin, &slen);
        for (int i = 0; i < MP_ARRAY_SIZE(machine_pin_obj); i++) {
            if (slen == strlen(machine_pin_obj[i].pin_name) && strncmp(s, machine_pin_obj[i].pin_name, slen) == 0) {
                wanted_pin = i;
                break;
            }
        }
    }

    if (!(0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj))) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin: Pin not defined!"));
    }

    return wanted_pin;
}

// helper function to translate pin_name(string) into machine_pin_io_obj_t->pin_addr
int pin_addr_by_name(mp_obj_t pin) {
    if (mp_obj_is_str(pin)) {
        return machine_pin_obj[pin_find(pin)].pin_addr;
    } else {
        return -1; // expecting a str as input
    }
}

#define MAX_PIN_IO MP_ARRAY_SIZE(machine_pin_phy_obj)
machine_pin_io_obj_t *pin_io[MAX_PIN_IO] = {NULL};

static inline int pin_io_allocate(void) {
    uint16_t i;
    for (i = 0; i < MAX_PIN_IO; i++) {
        if (pin_io[i] == NULL) {
            break;
        }
    }

    return i;
}

// Note: Used to define gpio value QSTRs
// Note: not in MPY guidelines but may needed later for board-specific active-low/active-high pins
#define GPIO_STATE_ON                   (0U)
#define GPIO_STATE_OFF                  (1U)


// function to get cypdl drive modes, correlated to cyhal drive modes in file: cyhal_gpio.c
// Note: drive modes are enumed in cy_gpio.h and are also distinguished for pins with input buffer on or off (configured as input or output respectively)
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
STATIC bool gpio_is_open_drain(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_OD_DRIVESLOW || gpio_get_drive(pin) == CY_GPIO_DM_OD_DRIVESHIGH || gpio_get_drive(pin) == CY_GPIO_DM_OD_DRIVESLOW_IN_OFF || gpio_get_drive(pin) == CY_GPIO_DM_OD_DRIVESHIGH_IN_OFF) {
        return true;
    } else {
        return false;
    }
}

// function to check if pin is in mode Pin.OUT; TODO: can be also implemented by checking input buffer on/off
STATIC bool gpio_is_out(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_STRONG_IN_OFF) { // pin cfgd as o/p drive so Input buffer is off.
        return true;
    } else {
        return false;
    }
}

// function to check if pin is in mode Pin.IN; TODO: can be also implemented by checking input buffer on/off
STATIC bool gpio_is_in(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_HIGHZ) {
        return true;
    } else {
        return false;
    }
}

// function to check if pin has pull Pin.PULL_UP
STATIC bool gpio_is_pull_up(uint32_t pin) {
    if (gpio_get_drive(pin) == CY_GPIO_DM_PULLUP_IN_OFF || gpio_get_drive(pin) == CY_GPIO_DM_PULLUP) {
        return true;
    } else {
        return false;
    }
}

// function to check if pin has pull Pin.PULL_DOWN
STATIC bool gpio_is_pull_down(uint32_t pin) {
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
STATIC int8_t gpio_get_value_call(uint32_t pin) {
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


// Mandatory MPY functions
// Pin.__call__
STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mplogger_print("machine pin call\n");

    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_io_obj_t *self = self_in;

    // Note: see https://docs.micropython.org/en/latest/library/machine.Pin.html#machine.Pin.value
    if (n_args == 0) {
        // get pin value if pin is in input mode, returns NONE (undefined) if pin is in output mode
        int8_t call_value = gpio_get_value_call(self->pin_addr);
        if (call_value != -1) {
            return MP_OBJ_NEW_SMALL_INT(call_value); // if pin is input, return value
        } else {
            return mp_const_none; // else return undefined, none
        }
    } else {
        // set pin
        bool value = mp_obj_is_true(args[0]);
        if (gpio_is_in(self->pin_addr) || gpio_is_out(self->pin_addr) || gpio_is_open_drain(self->pin_addr)) { // set the output buffer of output driver with given value;
            if (value) {                              // if Pin.Mode is Pin.IN, value will reflect when pin is next set as output.
                gpio_set_value(self->pin_addr);
            } else {
                gpio_clear_value(self->pin_addr);
            }
        }
    } // given how the PSoC architecture is, if the "drive mode" is set correctly, the same set/clr functions can be used for all the "modes".
      // refer pg 245 of PSoC6 Arch TRM: https://www.infineon.com/dgdl/Infineon-PSoC_6_MCU_PSoC_62_Architecture_Technical_Reference_Manual-AdditionalTechnicalInformation-v08_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f94758401d1&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-files

    return mp_const_none;
}

// Pin.print()
STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mplogger_print("machine pin print\n");

    machine_pin_io_obj_t *self = self_in;
    en_hsiom_sel_t pin_func = Cy_GPIO_GetHSIOM(CYHAL_GET_PORTADDR(self->pin_addr), CYHAL_GET_PIN(self->pin_addr));
    // en_hsiom_sel_t pin_func = pin_get_hsiom_func(self->pin_addr);
    qstr mode_qstr = MP_QSTR_None; // TODO: compare with rp2, init value needed here due to "-werror=maybe-uninitialized"
    qstr pull_qstr = MP_QSTR_None;
    uint8_t pin_value = -1;

    if (pin_func == HSIOM_SEL_GPIO) {
        if (gpio_is_open_drain(self->pin_addr)) {
            mode_qstr = MP_QSTR_OPEN_DRAIN;
        } else if (gpio_is_out(self->pin_addr)) {
            mode_qstr = MP_QSTR_OUT;
        } else if (gpio_is_in(self->pin_addr)) {
            mode_qstr = MP_QSTR_IN;
        } else { // only pull up and pull down are prescribed in MPY docs
            if (gpio_is_pull_up(self->pin_addr)) {
                pull_qstr = MP_QSTR_PULL_UP;
                if (gpio_get_drive(self->pin_addr) < 8) { // drive enum is less than 8 when input buffer is off (or pin is cfgd as output)
                    mode_qstr = MP_QSTR_OUT;
                } else {
                    mode_qstr = MP_QSTR_IN;
                }
            } else if (gpio_is_pull_down(self->pin_addr)) {
                pull_qstr = MP_QSTR_PULL_DOWN;
                if (gpio_get_drive(self->pin_addr) < 8) {
                    mode_qstr = MP_QSTR_OUT;
                } else {
                    mode_qstr = MP_QSTR_IN;
                }
            } else {
                mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("no params set for given pin object\n"));
            }
        }
        pin_value = gpio_get_value(self->pin_addr);
    } else {
        mode_qstr = MP_QSTR_ALT;
    }

    mp_printf(print, "Pin:%u or %s, Mode=%q, Pull=%q, Value=%u", self->pin_addr, self->pin_name, mode_qstr, pull_qstr, pin_value);
}

static cyhal_gpio_direction_t mp_to_cy_get_gpio_direction(uint8_t mode) {
    cyhal_gpio_direction_t direction;

    switch (mode) {
        case GPIO_MODE_IN:
            direction = CYHAL_GPIO_DIR_INPUT;
            break;

        case GPIO_MODE_OUT:
        case GPIO_MODE_OPEN_DRAIN:
            direction = CYHAL_GPIO_DIR_OUTPUT;
            break;

        case GPIO_MODE_NONE:
            mp_raise_NotImplementedError(MP_ERROR_TEXT("Pin mode is a required argument\n"));
            break;

        // TODO: you won´t enter here. Remove
        default:
            mp_raise_NotImplementedError(MP_ERROR_TEXT("Pin modes ALT, ALT_OPEN_DRAIN or ANALOG not implemented\n"));
            break;
    }
    return direction;
}

static cyhal_gpio_drive_mode_t mp_to_cy_get_gpio_drive(uint8_t mode, uint8_t pull) {
    cyhal_gpio_drive_mode_t cy_drive;

    // drive modes explained here: https://community.infineon.com/t5/Knowledge-Base-Articles/Drive-Modes-in-PSoC-GPIO/ta-p/248470
    // see app note: https://www.infineon.com/dgdl/Infineon-AN2094_PSoC_1_Getting_Started_With_GPIO-ApplicationNotes-v12_00-EN.pdf?fileId=8ac78c8c7cdc391c017d072966174e13&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-application_note

    switch (mode) {
        case GPIO_MODE_IN: {
            // mpy_drive value is ignored. Only relevant for outputs.
            switch (pull) {
                case GPIO_PULL_UP:
                    cy_drive = CYHAL_GPIO_DRIVE_PULLUP;
                    break;

                case GPIO_PULL_DOWN:
                    cy_drive = CYHAL_GPIO_DRIVE_PULLDOWN;
                    break;

                case GPIO_PULL_NONE:
                default:
                    cy_drive = CYHAL_GPIO_DRIVE_PULL_NONE;
                    break;
            }
        }
        break;

        case GPIO_MODE_OUT: {
            switch (pull) {
                case GPIO_PULL_UP:
                    cy_drive = CYHAL_GPIO_DRIVE_PULLUP;
                    break;

                case GPIO_PULL_DOWN:
                    cy_drive = CYHAL_GPIO_DRIVE_PULLDOWN;
                    break;

                case GPIO_PULL_NONE:
                default:
                    // cyhal will set drive strong for output if PULL NONE.
                    cy_drive = CYHAL_GPIO_DRIVE_PULL_NONE;
                    break;
            }
        }
        break;

        case GPIO_MODE_OPEN_DRAIN:
            // mpy_drive is ignored. Not relevant for open drain output.
            // mpy_pull is ignored. Not configurable for open drain output.
            cy_drive = CYHAL_GPIO_DRIVE_OPENDRAINDRIVESLOW;
            break;
    }

    return cy_drive;
}

bool machine_pin_is_inited(machine_pin_io_obj_t *self) {
    // The mode is a mandatory argument for initialization
    // Thus, if other than NONE, the cyhal_gpio object is already initialized.
    if (self->mode != GPIO_MODE_NONE) {
        return true;
    }
    return false;
}

// helper function to parse given initial params and invoke HAL-level GPIO functions
STATIC mp_obj_t machine_pin_obj_init_helper(machine_pin_io_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mplogger_print("init helper function called\n");

    enum {ARG_mode, ARG_pull, ARG_value, ARG_drive}; // , ARG_alt};
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_mode,  MP_ARG_OBJ,                     {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_pull,  MP_ARG_OBJ,                     {.u_rom_obj = MP_ROM_NONE}},
        {MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ,    {.u_rom_obj = MP_ROM_NONE}},
        // {MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_OBJ,    {.u_rom_obj = MP_ROM_NONE}},
        // {MP_QSTR_alt,   MP_ARG_KW_ONLY | MP_ARG_OBJ,    {.u_rom_obj = MP_ROM_NONE}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool pin_already_inited = machine_pin_is_inited(self);

    bool value = false; // Default low. Initial value irrelevant if not specified.
    if (args[ARG_value].u_obj != mp_const_none) {
        value = mp_obj_is_true(args[ARG_value].u_obj);
    }

    // cyhal_gpio_direction_t direction; // = CYHAL_GPIO_DIR_OUTPUT; // initially set as output
    // cyhal_gpio_drive_mode_t drive = CYHAL_GPIO_DRIVE_PULLUPDOWN; // initially set both pull up-down (safe for both IN-OUT)

    self->mode = GPIO_MODE_NONE;
    if (args[ARG_mode].u_obj != mp_const_none) {
        self->mode = mp_obj_get_int(args[ARG_mode].u_obj);
        // mp_int_t mode = mp_obj_get_int(args[ARG_mode].u_obj);

        // if (mode == GPIO_MODE_IN) {
        //     // direction = CYHAL_GPIO_DIR_INPUT;
        //     if (args[ARG_pull].u_obj == mp_const_none) { // if pull not provided, set pull as NONE for input (recommended)
        //         drive = CYHAL_GPIO_DRIVE_PULL_NONE;
        //     }
        // } else if (mode == GPIO_MODE_OUT) {
        //     // direction = CYHAL_GPIO_DIR_OUTPUT;
        //     if (args[ARG_pull].u_obj == mp_const_none) { // if pull not provided, set pull as STRONG for output (recommended)
        //         drive = CYHAL_GPIO_DRIVE_STRONG;
        //     }
        // } else if (mode == GPIO_MODE_OPEN_DRAIN) {
        //     drive = CYHAL_GPIO_DRIVE_OPENDRAINDRIVESLOW; // covers both the cases of open_drain -> strong 0 when value=0, highZ when value=1

        // see constructor here: https://docs.micropython.org/en/latest/library/machine.Pin.html
        // also drive modes explained here: https://community.infineon.com/t5/Knowledge-Base-Articles/Drive-Modes-in-PSoC-GPIO/ta-p/248470
        // see app note: https://www.infineon.com/dgdl/Infineon-AN2094_PSoC_1_Getting_Started_With_GPIO-ApplicationNotes-v12_00-EN.pdf?fileId=8ac78c8c7cdc391c017d072966174e13&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-application_note

        // Note: below code is redundant since definition of open drain is later understood.
        // if(value == 1)
        //     drive = CYHAL_GPIO_DRIVE_NONE; // Hi-Z as per MPY docs
        // else if (value == 0)
        //     drive = CYHAL_GPIO_DRIVE_OPENDRAINDRIVESLOW; // drive low as per MPY docs
        // else
        //     mp_printf(&mp_plat_print, "Specify init value of pin for open-drain\n"); //never reached since init val assumed above, may be removed
        // } else {
        //     // Alternate function. PIN.ALT
        //     mp_raise_NotImplementedError(MP_ERROR_TEXT("Pin.ALT modes not implemented\n"));
        // }
    }

    // // check for drive strength
    // self->drive = GPIO_DRIVE_NONE;
    // if (args[ARG_drive].u_obj != mp_const_none) {
    //     self->drive = mp_obj_get_int(args[ARG_drive].u_obj);
    //     // mp_raise_NotImplementedError(MP_ERROR_TEXT("CYHAL has only one drive strength for output mode\n")); // see Cy_GPIO_GetDriveSel() for PDL drive modes
    // }

    // check for pulls
    // uint8_t pull = 0;
    self->pull = GPIO_PULL_NONE;
    if (args[ARG_pull].u_obj != mp_const_none) {
        self->pull = mp_obj_get_int(args[ARG_pull].u_obj);
        // pull = mp_obj_get_int(args[ARG_pull].u_obj);
        // if (pull == GPIO_PULL_UP) {
        //     drive = CYHAL_GPIO_DRIVE_PULLUP; // TODO: should we also force the value to 1 in case of pull up
        // } else if (pull == GPIO_PULL_DOWN) {
        //     drive = CYHAL_GPIO_DRIVE_PULLDOWN; // TODO: should we also force the value to 0 in case of pull down
        // } else {
        //     mp_raise_ValueError(MP_ERROR_TEXT("invalid value of pull\n"));
        // }
    }

    // // Argument kept
    // if (args[ARG_alt].u_obj != mp_const_none) {
    //     mp_raise_NotImplementedError(MP_ERROR_TEXT("Alternate functions are implemented by their respective machine modules\n"));
    // }

    cyhal_gpio_direction_t direction = mp_to_cy_get_gpio_direction(self->mode);
    cyhal_gpio_drive_mode_t drive = mp_to_cy_get_gpio_drive(self->mode, self->pull);

    cy_rslt_t result = CY_RSLT_SUCCESS;
    if (pin_already_inited) {
        result = cyhal_gpio_configure(self->pin_phy->addr, direction, drive);
    } else {
        cyhal_gpio_free(self->pin_addr); // TODO: remove
        result = cyhal_gpio_init(self->pin_phy->addr, direction, drive, value);
    }
    mplogger_print("Direction: %d, Drive:%d, Value:%d\n", direction, drive, value);

    if (result != CY_RSLT_SUCCESS) {
        mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("CYHAL GPIO error: Init unsuccessful\n"));
    }
    return mp_const_none;
}

// Machine Pin methods - port-specific definitions
// Pin constructor(id,mode,pull,value=value,drive=drive,alt=alt)

/* Note: The constructor is able accept a minimum of one param madatorily, namely the pin (eg. "P13_7").
However, without any other config/params, the rest of the member functions being called might
render unpredictable behavior.

Two possible solutions to this issue:
1. Set a flag when params are set (obj_init_helper called) and member functions can check this flag and raise exception if its not set.
2. Raise minimmum number of expected params in constructor to 2.  */


// TODO: It makes no sense to create an instance of a pin without specifying what type.
// So at least the mode positional argument needs to be provided.
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mplogger_print("%q constructor invoked\n", MP_QSTR_Pin);

    // Inspects the argument list and checks number of arguments - Function signature from mpy side
    // Signature: mp_arg_check_num(n_args, n_kw, self->n_args_min, self->n_args_max, self->is_kw);
    mp_arg_check_num(n_args, n_kw, 1, 6, true);

    machine_pin_phy_obj_t *pin_phy = pin_phy_realloc(args[0], PIN_PHY_FUNC_DIO);

    // object ptr for the class obj instantiated
    // machine_pin_io_obj_t *self = NULL;
    int free_pin_io = pin_io_allocate();
    pin_io[free_pin_io] = mp_obj_malloc(machine_pin_io_obj_t, &machine_pin_type);
    machine_pin_io_obj_t *pin = pin_io[free_pin_io];
    pin_phy_assert_null(pin);
    machine_pin_io_obj_t *self = pin;
    // pin = mp_obj_malloc(machine_pin_io_obj_t, &machine_pin_type);

    pin->pin_phy = pin_phy;
    pin->pin_addr = pin_phy->addr;
    pin->pin_name = pin_phy->name;

    // get the wanted pin index
    // int wanted_pin = pin_find(args[0]);

    // Note: we have different init args based on the type of pin. so Pin("LED", Pin.OUT) may not always make sense
    // assign machine_pin obj to obj pointer
    // self = &machine_pin_obj[wanted_pin];

    // go into param arg parsing if args apart from "id" are provided (for ex. pin.Mode, pin.PULL etc)
    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args); // skipping "id" as an arg as it is a part of self*.
    }
    return MP_OBJ_FROM_PTR(self);
}

// pin.value([value])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// instantiates obj of Pin class
// Pin.init(mode,pull,value=value,drive=drive,alt=alt)
STATIC mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mplogger_print("Pin class object instantiated\n"); // TODO: remove later
    // redirect args except self to the obj init helper function
    // args[0] here is self ptr
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_obj_init_obj, 1, machine_pin_obj_init);


// Pin.toggle()
STATIC mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_io_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (gpio_is_in(self->pin_addr) || gpio_is_out(self->pin_addr) || gpio_is_open_drain(self->pin_addr)) { // toggle the output buffer of output driver with given value;
        gpio_toggle_value(self->pin_addr); // for output it takes effect instantly; for input pins, the effect will show when
        // pin is set as input next. For open drain, behavior shifts between 0 and Hi-Z.
    }
    return mp_const_none;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

// Pin.high()
STATIC mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_io_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (gpio_is_in(self->pin_addr) || gpio_is_out(self->pin_addr) || gpio_is_open_drain(self->pin_addr)) { // toggle the output buffer of output driver with given value;
        gpio_set_value(self->pin_addr); // for output it takes effect instantly; for input pins, the effect will show when
        // pin is set as input next. For open drain, behavior shifts between 0 and HiZ.
    }
    return mp_const_none;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

// Pin.low()
STATIC mp_obj_t machine_pin_low(mp_obj_t self_in) {
    machine_pin_io_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (gpio_is_in(self->pin_addr) || gpio_is_out(self->pin_addr) || gpio_is_open_drain(self->pin_addr)) { // toggle the output buffer of output driver with given value;
        gpio_clear_value(self->pin_addr); // for output it takes effect instantly; for input pins, the effect will show when
        // pin is set as input next. For open drain, behavior shifts between 0 and HiZ.
    }
    return mp_const_none;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // Instance methods
    { MP_ROM_QSTR(MP_QSTR___name__),                MP_ROM_QSTR(MP_QSTR_machine) },
    { MP_ROM_QSTR(MP_QSTR_init),                    MP_ROM_PTR(&machine_pin_obj_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle),                  MP_ROM_PTR(&machine_pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),                   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_low),                     MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),                    MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),                     MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),                      MP_ROM_PTR(&machine_pin_low_obj) },
    // { MP_ROM_QSTR(MP_QSTR_mode),                 MP_ROM_PTR(&machine_pin_mode_obj) }, //TODO: got unknown error!
    // Pin mode constants
    { MP_ROM_QSTR(MP_QSTR_IN),                      MP_ROM_INT(GPIO_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT),                     MP_ROM_INT(GPIO_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),              MP_ROM_INT(GPIO_MODE_OPEN_DRAIN) },
    // { MP_ROM_QSTR(MP_QSTR_ALT_OPEN_DRAIN),          MP_ROM_INT(GPIO_MODE_ALT_OPEN_DRAIN) },
    // Pin drive constants
    // TODO: add/uncomment below if/when CYPDL drive modes implemented
    // { MP_ROM_QSTR(MP_QSTR_DRIVE_0),                 MP_ROM_INT(GPIO_DRIVE_CAP_0) },
    // { MP_ROM_QSTR(MP_QSTR_DRIVE_1),                 MP_ROM_INT(GPIO_DRIVE_CAP_1) },
    // { MP_ROM_QSTR(MP_QSTR_DRIVE_2),                 MP_ROM_INT(GPIO_DRIVE_CAP_2) },
    // { MP_ROM_QSTR(MP_QSTR_DRIVE_3),                 MP_ROM_INT(GPIO_DRIVE_CAP_3) },
    // pin.pull
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),                 MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN),               MP_ROM_INT(GPIO_PULL_DOWN) },
    // Pin initial value constants
    // Note: not in MPY guidelines but may needed later for board-specific active-low/active-high pins
    { MP_ROM_QSTR(MP_QSTR_STATE_HIGH),              MP_ROM_INT(GPIO_STATE_ON) },
    { MP_ROM_QSTR(MP_QSTR_STATE_LOW),               MP_ROM_INT(GPIO_STATE_OFF) },
    // // Pin alt constants
    // { MP_ROM_QSTR(MP_QSTR_ALT),                     MP_ROM_INT(GPIO_MODE_ALT) }
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_io_obj_t *self = self_in;
    switch (request) {
        case MP_PIN_READ: {
            return cyhal_gpio_read(self->pin_addr);
        }
        case MP_PIN_WRITE: {
            cyhal_gpio_write(self->pin_addr, arg);
            return 0;
        }
    }
    return -1;
}

STATIC const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    print, machine_pin_print,
    call, machine_pin_call,
    protocol, &pin_pin_p,
    locals_dict, &machine_pin_locals_dict
    );


machine_pin_io_obj_t machine_pin_obj[] = {
    {{&machine_pin_type}, PIN_P0_0, "P0_0"},
    {{&machine_pin_type}, PIN_P0_1, "P0_1"},
    {{&machine_pin_type}, PIN_P0_2, "P0_2"},
    {{&machine_pin_type}, PIN_P0_3, "P0_3"},
    {{&machine_pin_type}, PIN_P0_4, "P0_4"},
    {{&machine_pin_type}, PIN_P0_5, "P0_5"},

    {{&machine_pin_type}, PIN_P1_0, "P1_0"},
    {{&machine_pin_type}, PIN_P1_1, "P1_1"},
    {{&machine_pin_type}, PIN_P1_2, "P1_2"},
    {{&machine_pin_type}, PIN_P1_3, "P1_3"},
    {{&machine_pin_type}, PIN_P1_4, "P1_4"},
    {{&machine_pin_type}, PIN_P1_5, "P1_5"},

    {{&machine_pin_type}, PIN_P2_0, "P2_0"},
    {{&machine_pin_type}, PIN_P2_1, "P2_1"},
    {{&machine_pin_type}, PIN_P2_2, "P2_2"},
    {{&machine_pin_type}, PIN_P2_3, "P2_3"},
    {{&machine_pin_type}, PIN_P2_4, "P2_4"},
    {{&machine_pin_type}, PIN_P2_5, "P2_5"},
    {{&machine_pin_type}, PIN_P2_6, "P2_6"},
    {{&machine_pin_type}, PIN_P2_7, "P2_7"},

    {{&machine_pin_type}, PIN_P3_0, "P3_0"},
    {{&machine_pin_type}, PIN_P3_1, "P3_1"},
    {{&machine_pin_type}, PIN_P3_2, "P3_2"},
    {{&machine_pin_type}, PIN_P3_3, "P3_3"},
    {{&machine_pin_type}, PIN_P3_4, "P3_4"},
    {{&machine_pin_type}, PIN_P3_5, "P3_5"},

    {{&machine_pin_type}, PIN_P4_0, "P4_0"},
    {{&machine_pin_type}, PIN_P4_1, "P4_1"},

    {{&machine_pin_type}, PIN_P5_0, "P5_0"},
    {{&machine_pin_type}, PIN_P5_1, "P5_1"},
    {{&machine_pin_type}, PIN_P5_2, "P5_2"},
    {{&machine_pin_type}, PIN_P5_3, "P5_3"},
    {{&machine_pin_type}, PIN_P5_4, "P5_4"},
    {{&machine_pin_type}, PIN_P5_5, "P5_5"},
    {{&machine_pin_type}, PIN_P5_6, "P5_6"},
    {{&machine_pin_type}, PIN_P5_7, "P5_7"},

    {{&machine_pin_type}, PIN_P6_0, "P6_0"},
    {{&machine_pin_type}, PIN_P6_1, "P6_1"},
    {{&machine_pin_type}, PIN_P6_2, "P6_2"},
    {{&machine_pin_type}, PIN_P6_3, "P6_3"},
    {{&machine_pin_type}, PIN_P6_4, "P6_4"},
    {{&machine_pin_type}, PIN_P6_5, "P6_5"},
    {{&machine_pin_type}, PIN_P6_6, "P6_6"},
    {{&machine_pin_type}, PIN_P6_7, "P6_7"},

    {{&machine_pin_type}, PIN_P7_0, "P7_0"},
    {{&machine_pin_type}, PIN_P7_1, "P7_1"},
    {{&machine_pin_type}, PIN_P7_2, "P7_2"},
    {{&machine_pin_type}, PIN_P7_3, "P7_3"},
    {{&machine_pin_type}, PIN_P7_4, "P7_4"},
    {{&machine_pin_type}, PIN_P7_5, "P7_5"},
    {{&machine_pin_type}, PIN_P7_6, "P7_6"},
    {{&machine_pin_type}, PIN_P7_7, "P7_7"},

    {{&machine_pin_type}, PIN_P8_0, "P8_0"},
    {{&machine_pin_type}, PIN_P8_1, "P8_1"},
    {{&machine_pin_type}, PIN_P8_2, "P8_2"},
    {{&machine_pin_type}, PIN_P8_3, "P8_3"},
    {{&machine_pin_type}, PIN_P8_4, "P8_4"},
    {{&machine_pin_type}, PIN_P8_5, "P8_5"},
    {{&machine_pin_type}, PIN_P8_6, "P8_6"},
    {{&machine_pin_type}, PIN_P8_7, "P8_7"},

    {{&machine_pin_type}, PIN_P9_0, "P9_0"},
    {{&machine_pin_type}, PIN_P9_1, "P9_1"},
    {{&machine_pin_type}, PIN_P9_2, "P9_2"},
    {{&machine_pin_type}, PIN_P9_3, "P9_3"},
    {{&machine_pin_type}, PIN_P9_4, "P9_4"},
    {{&machine_pin_type}, PIN_P9_5, "P9_5"},
    {{&machine_pin_type}, PIN_P9_6, "P9_6"},
    {{&machine_pin_type}, PIN_P9_7, "P9_7"},

    {{&machine_pin_type}, PIN_P10_0, "P10_0"},
    {{&machine_pin_type}, PIN_P10_1, "P10_1"},
    {{&machine_pin_type}, PIN_P10_2, "P10_2"},
    {{&machine_pin_type}, PIN_P10_3, "P10_3"},
    {{&machine_pin_type}, PIN_P10_4, "P10_4"},
    {{&machine_pin_type}, PIN_P10_5, "P10_5"},
    {{&machine_pin_type}, PIN_P10_6, "P10_6"},
    {{&machine_pin_type}, PIN_P10_7, "P10_7"},

    {{&machine_pin_type}, PIN_P11_0, "P11_0"},
    {{&machine_pin_type}, PIN_P11_1, "P11_1"},
    {{&machine_pin_type}, PIN_P11_2, "P11_2"},
    {{&machine_pin_type}, PIN_P11_3, "P11_3"},
    {{&machine_pin_type}, PIN_P11_4, "P11_4"},
    {{&machine_pin_type}, PIN_P11_5, "P11_5"},
    {{&machine_pin_type}, PIN_P11_6, "P11_6"},
    {{&machine_pin_type}, PIN_P11_7, "P11_7"},

    {{&machine_pin_type}, PIN_P12_0, "P12_0"},
    {{&machine_pin_type}, PIN_P12_1, "P12_1"},
    {{&machine_pin_type}, PIN_P12_2, "P12_2"},
    {{&machine_pin_type}, PIN_P12_3, "P12_3"},
    {{&machine_pin_type}, PIN_P12_4, "P12_4"},
    {{&machine_pin_type}, PIN_P12_5, "P12_5"},
    {{&machine_pin_type}, PIN_P12_6, "P12_6"},
    {{&machine_pin_type}, PIN_P12_7, "P12_7"},

    {{&machine_pin_type}, PIN_P13_0, "P13_0"},
    {{&machine_pin_type}, PIN_P13_1, "P13_1"},
    {{&machine_pin_type}, PIN_P13_2, "P13_2"},
    {{&machine_pin_type}, PIN_P13_3, "P13_3"},
    {{&machine_pin_type}, PIN_P13_4, "P13_4"},
    {{&machine_pin_type}, PIN_P13_5, "P13_5"},
    {{&machine_pin_type}, PIN_P13_6, "P13_6"},
    {{&machine_pin_type}, PIN_P13_7, "P13_7"},

    {{&machine_pin_type}, PIN_USBDP, "USBDP"},
    {{&machine_pin_type}, PIN_USBDM, "USBDM"},
};

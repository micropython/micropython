#ifndef MICROPY_INCLUDED_MACHINE_PIN_PHY_H
#define MICROPY_INCLUDED_MACHINE_PIN_PHY_H

#include <stdint.h>

typedef enum {
    PIN_PHY_FUNC_NONE = 0x00,
    PIN_PHY_FUNC_DIO = 0x01,
    PIN_PHY_FUNC_ADC = 0x02,
    PIN_PHY_FUNC_I2C = 0x04,
    PIN_PHY_FUNC_PWM = 0x08,
    PIN_PHY_FUNC_SPI = 0x10
} machine_pin_phy_func_t;

typedef struct _machine_pin_phy_obj_t {
    uint32_t addr;
    char *name;
    machine_pin_phy_func_t allocated_func;
} machine_pin_phy_obj_t;

#include "genhdr/pins.h"

extern machine_pin_phy_obj_t machine_pin_phy_obj[];

extern const uint8_t machine_pin_num_of_cpu_pins;

bool pin_phy_is_alloc(machine_pin_phy_obj_t *obj);
machine_pin_phy_obj_t *pin_phy_alloc(mp_obj_t pin_name, machine_pin_phy_func_t func);
machine_pin_phy_obj_t *pin_phy_realloc(mp_obj_t pin_name, machine_pin_phy_func_t func);
void pin_phy_free(machine_pin_phy_obj_t *obj);

void mod_pin_phy_deinit(void);

// Function Prototypes to support interaction between c<->py
int pin_find(mp_obj_t obj);
mp_obj_t pin_name_by_addr(mp_obj_t pin);
int pin_addr_by_name(mp_obj_t obj);

#endif // MICROPY_INCLUDED_MACHINE_PIN_PHY_H

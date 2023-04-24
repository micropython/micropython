#ifndef MICROPY_INCLUDED_PSOC6_PINS_H
#define MICROPY_INCLUDED_PSOC6_PINS_H


// port-specific includes
#include "modmachine.h"


// cy includes
#include "cyhal.h"


// mpy include


/* Include Pin definitions to make it visible at py side*/
// Generated manually w.r.t cyhal_gpio_psoc6_02_124_bga_t enum for the package present in the CYPROTO-062-4343 target
// TODO: automate this generation using a python script, same as other ports.
#define PIN_P0_0  CYHAL_GET_GPIO(CYHAL_PORT_0, 0)// !< Port 0 Pin 0
#define PIN_P0_1  CYHAL_GET_GPIO(CYHAL_PORT_0, 1)// !< Port 0 Pin 1
#define PIN_P0_2  CYHAL_GET_GPIO(CYHAL_PORT_0, 2)// !< Port 0 Pin 2
#define PIN_P0_3  CYHAL_GET_GPIO(CYHAL_PORT_0, 3)// !< Port 0 Pin 3
#define PIN_P0_4  CYHAL_GET_GPIO(CYHAL_PORT_0, 4)// !< Port 0 Pin 4
#define PIN_P0_5  CYHAL_GET_GPIO(CYHAL_PORT_0, 5)// !< Port 0 Pin 5

#define PIN_P1_0  CYHAL_GET_GPIO(CYHAL_PORT_1, 0)// !< Port 1 Pin 0
#define PIN_P1_1  CYHAL_GET_GPIO(CYHAL_PORT_1, 1)// !< Port 1 Pin 1
#define PIN_P1_2  CYHAL_GET_GPIO(CYHAL_PORT_1, 2)// !< Port 1 Pin 2
#define PIN_P1_3  CYHAL_GET_GPIO(CYHAL_PORT_1, 3)// !< Port 1 Pin 3
#define PIN_P1_4  CYHAL_GET_GPIO(CYHAL_PORT_1, 4)// !< Port 1 Pin 4
#define PIN_P1_5  CYHAL_GET_GPIO(CYHAL_PORT_1, 5)// !< Port 1 Pin 5

#define PIN_P2_0  CYHAL_GET_GPIO(CYHAL_PORT_2, 0)// !< Port 2 Pin 0
#define PIN_P2_1  CYHAL_GET_GPIO(CYHAL_PORT_2, 1)// !< Port 2 Pin 1
#define PIN_P2_2  CYHAL_GET_GPIO(CYHAL_PORT_2, 2)// !< Port 2 Pin 2
#define PIN_P2_3  CYHAL_GET_GPIO(CYHAL_PORT_2, 3)// !< Port 2 Pin 3
#define PIN_P2_4  CYHAL_GET_GPIO(CYHAL_PORT_2, 4)// !< Port 2 Pin 4
#define PIN_P2_5  CYHAL_GET_GPIO(CYHAL_PORT_2, 5)// !< Port 2 Pin 5
#define PIN_P2_6  CYHAL_GET_GPIO(CYHAL_PORT_2, 6)// !< Port 2 Pin 6
#define PIN_P2_7  CYHAL_GET_GPIO(CYHAL_PORT_2, 7)// !< Port 2 Pin 7

#define PIN_P3_0  CYHAL_GET_GPIO(CYHAL_PORT_3, 0)// !< Port 3 Pin 0
#define PIN_P3_1  CYHAL_GET_GPIO(CYHAL_PORT_3, 1)// !< Port 3 Pin 1
#define PIN_P3_2  CYHAL_GET_GPIO(CYHAL_PORT_3, 2)// !< Port 3 Pin 2
#define PIN_P3_3  CYHAL_GET_GPIO(CYHAL_PORT_3, 3)// !< Port 3 Pin 3
#define PIN_P3_4  CYHAL_GET_GPIO(CYHAL_PORT_3, 4)// !< Port 3 Pin 4
#define PIN_P3_5  CYHAL_GET_GPIO(CYHAL_PORT_3, 5)// !< Port 3 Pin 5

#define PIN_P4_0  CYHAL_GET_GPIO(CYHAL_PORT_4, 0)// !< Port 4 Pin 0
#define PIN_P4_1  CYHAL_GET_GPIO(CYHAL_PORT_4, 1)// !< Port 4 Pin 1

#define PIN_P5_0  CYHAL_GET_GPIO(CYHAL_PORT_5, 0)// !< Port 5 Pin 0
#define PIN_P5_1  CYHAL_GET_GPIO(CYHAL_PORT_5, 1)// !< Port 5 Pin 1
#define PIN_P5_2  CYHAL_GET_GPIO(CYHAL_PORT_5, 2)// !< Port 5 Pin 2
#define PIN_P5_3  CYHAL_GET_GPIO(CYHAL_PORT_5, 3)// !< Port 5 Pin 3
#define PIN_P5_4  CYHAL_GET_GPIO(CYHAL_PORT_5, 4)// !< Port 5 Pin 4
#define PIN_P5_5  CYHAL_GET_GPIO(CYHAL_PORT_5, 5)// !< Port 5 Pin 5
#define PIN_P5_6  CYHAL_GET_GPIO(CYHAL_PORT_5, 6)// !< Port 5 Pin 6
#define PIN_P5_7  CYHAL_GET_GPIO(CYHAL_PORT_5, 7)// !< Port 5 Pin 7

#define PIN_P6_0  CYHAL_GET_GPIO(CYHAL_PORT_6, 0)// !< Port 6 Pin 0
#define PIN_P6_1  CYHAL_GET_GPIO(CYHAL_PORT_6, 1)// !< Port 6 Pin 1
#define PIN_P6_2  CYHAL_GET_GPIO(CYHAL_PORT_6, 2)// !< Port 6 Pin 2
#define PIN_P6_3  CYHAL_GET_GPIO(CYHAL_PORT_6, 3)// !< Port 6 Pin 3
#define PIN_P6_4  CYHAL_GET_GPIO(CYHAL_PORT_6, 4)// !< Port 6 Pin 4
#define PIN_P6_5  CYHAL_GET_GPIO(CYHAL_PORT_6, 5)// !< Port 6 Pin 5
#define PIN_P6_6  CYHAL_GET_GPIO(CYHAL_PORT_6, 6)// !< Port 6 Pin 6
#define PIN_P6_7  CYHAL_GET_GPIO(CYHAL_PORT_6, 7)// !< Port 6 Pin 7

#define PIN_P7_0  CYHAL_GET_GPIO(CYHAL_PORT_7, 0)// !< Port 7 Pin 0
#define PIN_P7_1  CYHAL_GET_GPIO(CYHAL_PORT_7, 1)// !< Port 7 Pin 1
#define PIN_P7_2  CYHAL_GET_GPIO(CYHAL_PORT_7, 2)// !< Port 7 Pin 2
#define PIN_P7_3  CYHAL_GET_GPIO(CYHAL_PORT_7, 3)// !< Port 7 Pin 3
#define PIN_P7_4  CYHAL_GET_GPIO(CYHAL_PORT_7, 4)// !< Port 7 Pin 4
#define PIN_P7_5  CYHAL_GET_GPIO(CYHAL_PORT_7, 5)// !< Port 7 Pin 5
#define PIN_P7_6  CYHAL_GET_GPIO(CYHAL_PORT_7, 6)// !< Port 7 Pin 6
#define PIN_P7_7  CYHAL_GET_GPIO(CYHAL_PORT_7, 7)// !< Port 7 Pin 7

#define PIN_P8_0  CYHAL_GET_GPIO(CYHAL_PORT_8, 0)// !< Port 8 Pin 0
#define PIN_P8_1  CYHAL_GET_GPIO(CYHAL_PORT_8, 1)// !< Port 8 Pin 1
#define PIN_P8_2  CYHAL_GET_GPIO(CYHAL_PORT_8, 2)// !< Port 8 Pin 2
#define PIN_P8_3  CYHAL_GET_GPIO(CYHAL_PORT_8, 3)// !< Port 8 Pin 3
#define PIN_P8_4  CYHAL_GET_GPIO(CYHAL_PORT_8, 4)// !< Port 8 Pin 4
#define PIN_P8_5  CYHAL_GET_GPIO(CYHAL_PORT_8, 5)// !< Port 8 Pin 5
#define PIN_P8_6  CYHAL_GET_GPIO(CYHAL_PORT_8, 6)// !< Port 8 Pin 6
#define PIN_P8_7  CYHAL_GET_GPIO(CYHAL_PORT_8, 7)// !< Port 8 Pin 7

#define PIN_P9_0  CYHAL_GET_GPIO(CYHAL_PORT_9, 0)// !< Port 9 Pin 0
#define PIN_P9_1  CYHAL_GET_GPIO(CYHAL_PORT_9, 1)// !< Port 9 Pin 1
#define PIN_P9_2  CYHAL_GET_GPIO(CYHAL_PORT_9, 2)// !< Port 9 Pin 2
#define PIN_P9_3  CYHAL_GET_GPIO(CYHAL_PORT_9, 3)// !< Port 9 Pin 3
#define PIN_P9_4  CYHAL_GET_GPIO(CYHAL_PORT_9, 4)// !< Port 9 Pin 4
#define PIN_P9_5  CYHAL_GET_GPIO(CYHAL_PORT_9, 5)// !< Port 9 Pin 5
#define PIN_P9_6  CYHAL_GET_GPIO(CYHAL_PORT_9, 6)// !< Port 9 Pin 6
#define PIN_P9_7  CYHAL_GET_GPIO(CYHAL_PORT_9, 7)// !< Port 9 Pin 7

#define PIN_P10_0  CYHAL_GET_GPIO(CYHAL_PORT_10, 0)// !< Port 10 Pin 0
#define PIN_P10_1  CYHAL_GET_GPIO(CYHAL_PORT_10, 1)// !< Port 10 Pin 1
#define PIN_P10_2  CYHAL_GET_GPIO(CYHAL_PORT_10, 2)// !< Port 10 Pin 2
#define PIN_P10_3  CYHAL_GET_GPIO(CYHAL_PORT_10, 3)// !< Port 10 Pin 3
#define PIN_P10_4  CYHAL_GET_GPIO(CYHAL_PORT_10, 4)// !< Port 10 Pin 4
#define PIN_P10_5  CYHAL_GET_GPIO(CYHAL_PORT_10, 5)// !< Port 10 Pin 5
#define PIN_P10_6  CYHAL_GET_GPIO(CYHAL_PORT_10, 6)// !< Port 10 Pin 6
#define PIN_P10_7  CYHAL_GET_GPIO(CYHAL_PORT_10, 7)// !< Port 10 Pin 7

#define PIN_P11_0  CYHAL_GET_GPIO(CYHAL_PORT_11, 0)// !< Port 11 Pin 0
#define PIN_P11_1  CYHAL_GET_GPIO(CYHAL_PORT_11, 1)// !< Port 11 Pin 1
#define PIN_P11_2  CYHAL_GET_GPIO(CYHAL_PORT_11, 2)// !< Port 11 Pin 2
#define PIN_P11_3  CYHAL_GET_GPIO(CYHAL_PORT_11, 3)// !< Port 11 Pin 3
#define PIN_P11_4  CYHAL_GET_GPIO(CYHAL_PORT_11, 4)// !< Port 11 Pin 4
#define PIN_P11_5  CYHAL_GET_GPIO(CYHAL_PORT_11, 5)// !< Port 11 Pin 5
#define PIN_P11_6  CYHAL_GET_GPIO(CYHAL_PORT_11, 6)// !< Port 11 Pin 6
#define PIN_P11_7  CYHAL_GET_GPIO(CYHAL_PORT_11, 7)// !< Port 11 Pin 7

#define PIN_P12_0  CYHAL_GET_GPIO(CYHAL_PORT_12, 0)// !< Port 12 Pin 0
#define PIN_P12_1  CYHAL_GET_GPIO(CYHAL_PORT_12, 1)// !< Port 12 Pin 1
#define PIN_P12_2  CYHAL_GET_GPIO(CYHAL_PORT_12, 2)// !< Port 12 Pin 2
#define PIN_P12_3  CYHAL_GET_GPIO(CYHAL_PORT_12, 3)// !< Port 12 Pin 3
#define PIN_P12_4  CYHAL_GET_GPIO(CYHAL_PORT_12, 4)// !< Port 12 Pin 4
#define PIN_P12_5  CYHAL_GET_GPIO(CYHAL_PORT_12, 5)// !< Port 12 Pin 5
#define PIN_P12_6  CYHAL_GET_GPIO(CYHAL_PORT_12, 6)// !< Port 12 Pin 6
#define PIN_P12_7  CYHAL_GET_GPIO(CYHAL_PORT_12, 7)// !< Port 12 Pin 7

#define PIN_P13_0  CYHAL_GET_GPIO(CYHAL_PORT_13, 0)// !< Port 13 Pin 0
#define PIN_P13_1  CYHAL_GET_GPIO(CYHAL_PORT_13, 1)// !< Port 13 Pin 1
#define PIN_P13_2  CYHAL_GET_GPIO(CYHAL_PORT_13, 2)// !< Port 13 Pin 2
#define PIN_P13_3  CYHAL_GET_GPIO(CYHAL_PORT_13, 3)// !< Port 13 Pin 3
#define PIN_P13_4  CYHAL_GET_GPIO(CYHAL_PORT_13, 4)// !< Port 13 Pin 4
#define PIN_P13_5  CYHAL_GET_GPIO(CYHAL_PORT_13, 5)// !< Port 13 Pin 5
#define PIN_P13_6  CYHAL_GET_GPIO(CYHAL_PORT_13, 6)// !< Port 13 Pin 6
#define PIN_P13_7  CYHAL_GET_GPIO(CYHAL_PORT_13, 7)// !< Port 13 Pin 7

#define PIN_USBDP  CYHAL_GET_GPIO(CYHAL_PORT_14, 0)// !< Port 14 Pin 0
#define PIN_USBDM  CYHAL_GET_GPIO(CYHAL_PORT_14, 1)// !< Port 14 Pin 1

// Add all machine pin objects - GPIO , I2C, ADC etc.
typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint32_t pin_addr;
    char *pin_name;
} machine_pin_obj_t;

// Function Prototypes to support interaction between c<->py
int pin_find(mp_obj_t obj);
int pin_addr_by_name(mp_obj_t obj);

/* TODO: to auto-generate the pin instances in the pin_obj array
and thereby capture the obj count. It cannot be declared without a defined size,
since there are sizeof() and other functions in machine_pin which act on this array
and need a deterministic size of the array to compile. */
extern const machine_pin_obj_t machine_pin_obj[102];


#endif // MICROPY_INCLUDED_PSOC6_PINS_H

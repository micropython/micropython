#!/usr/bin/python3


def defines(name, suffix):
    print(f"mcu_pin_function_t {name} [] = {{")
    for instance in (0, 1):
        for function in "HI":
            for port in "ABCD":
                for idx in range(32):
                    pin = f"P{port}{idx:02d}"
                    pinmux = f"PINMUX_{pin}{function}_CAN{instance}_{suffix}"
                    print(
                        f"""\
#if defined({pinmux}) && ! defined(IGNORE_PIN_{pin})
    {{&pin_{pin}, {instance}, PIN_{pin}, {pinmux} & 0xffff}},
#endif"""
                    )
    print(f"{{NULL, 0, 0}}")
    print(f"}};")
    print()


print(
    """\
#include <stdint.h>
#include "py/obj.h"
#include "sam.h"
#include "samd/pins.h"
#include "mpconfigport.h"
#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"
#include "common-hal/microcontroller/Pin.h"
"""
)

defines("can_rx", "RX")
defines("can_tx", "TX")

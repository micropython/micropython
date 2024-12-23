#!/usr/bin/env python

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen


# Pins start at zero, and the highest pin index on any ESP32* chip is 48.
NUM_GPIOS = 49


class Esp32Pin(boardgen.Pin):
    # Required by NumericPinGenerator.
    def index(self):
        return int(self._cpu_pin_name[4:])

    # The IDF provides `GPIO_NUM_x = x` as an enum.
    def index_name(self):
        return "GPIO_NUM_{:d}".format(self.index())

    # Emit the combined struct which contains both the pin and irq instances.
    def definition(self):
        return "{ .base = { .type = &machine_pin_type }, .irq = { .base = { .type = &machine_pin_irq_type } } }"

    # This script isn't family-aware, so we always emit the maximum number of
    # pins and rely on the `MICROPY_HW_ENABLE_GPIOn` macros defined in
    # machine_pin.h to figure out which ones are actually available.
    def enable_macro(self):
        return "MICROPY_HW_ENABLE_{}".format(self._cpu_pin_name)

    # ESP32 cpu names must be "GPIOn".
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)

        if not cpu_pin_name.startswith("GPIO") or not cpu_pin_name[4:].isnumeric():
            raise boardgen.PinGeneratorError(
                "Invalid cpu pin name '{}', must be 'GPIOn'".format(cpu_pin_name)
            )

        if not (0 <= int(cpu_pin_name[4:]) < NUM_GPIOS):
            raise boardgen.PinGeneratorError("Unknown cpu pin '{}'".format(cpu_pin_name))


class Esp32PinGenerator(boardgen.NumericPinGenerator):
    def __init__(self):
        # Use custom pin type above.
        super().__init__(pin_type=Esp32Pin)

        # Pre-define the pins (i.e. don't require them to be listed in pins.csv).
        for i in range(NUM_GPIOS):
            self.add_cpu_pin("GPIO{}".format(i))

    # Only use pre-defined cpu pins (do not let board.csv create them).
    def find_pin_by_cpu_pin_name(self, cpu_pin_name, create=True):
        return super().find_pin_by_cpu_pin_name(cpu_pin_name, create=False)

    # This is provided by the IDF and is one more than the highest available
    # GPIO num.
    def cpu_table_size(self):
        return "GPIO_NUM_MAX"


if __name__ == "__main__":
    Esp32PinGenerator().main()

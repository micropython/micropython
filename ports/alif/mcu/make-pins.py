#!/usr/bin/env python

import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen

NUM_PORTS = 16
NUM_PINS_PER_PORT = 8

# Maps pad name to (adc12_periph, adc12_channel).
ADC12_ANA_MAP = {
    "GPIO0_0": (0, 0),
    "GPIO0_1": (0, 1),
    "GPIO0_2": (0, 2),
    "GPIO0_3": (0, 3),
    "GPIO0_4": (0, 4),
    "GPIO0_5": (0, 5),
    "GPIO0_6": (1, 0),
    "GPIO0_7": (1, 1),
    "GPIO1_0": (1, 2),
    "GPIO1_1": (1, 3),
    "GPIO1_2": (1, 4),
    "GPIO1_3": (1, 5),
    "GPIO1_4": (2, 0),
    "GPIO1_5": (2, 1),
    "GPIO1_6": (2, 2),
    "GPIO1_7": (2, 3),
    "GPIO2_0": (2, 4),
    "GPIO2_1": (2, 5),
}


class AlifPin(boardgen.Pin):
    # Emit the struct which contains the pin instance.
    def definition(self):
        port, pin = self.name()[4:].split("_")
        adc12_periph, adc12_channel = ADC12_ANA_MAP.get(self.name(), (3, 7))
        base = "LPGPIO_BASE" if port == "15" else "GPIO{}_BASE".format(port)
        return (
            "{{ "
            ".base = {{ .type = &machine_pin_type }}, "
            ".gpio = (GPIO_Type *){base}, "
            ".port = PORT_{port}, "
            ".pin = PIN_{pin}, "
            ".adc12_periph = {adc12_periph}, "
            ".adc12_channel = {adc12_channel}, "
            ".name = MP_QSTR_GPIO{port}_{pin} "
            "}}".format(
                port=port,
                pin=pin,
                base=base,
                adc12_periph=adc12_periph,
                adc12_channel=adc12_channel,
            )
        )

    # Alif cpu names must be "GPIOn_m".
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)

        if not (m := re.match("GPIO([0-9]){1,2}_([0-9])", cpu_pin_name)):
            raise boardgen.PinGeneratorError(
                "Invalid cpu pin name '{}', must be 'GPIOn_m'".format(cpu_pin_name)
            )

        port = int(m.group(1))
        pin = int(m.group(2))
        if not (0 <= port < NUM_PORTS and 0 <= pin < NUM_PINS_PER_PORT):
            raise boardgen.PinGeneratorError("Unknown cpu pin '{}'".format(cpu_pin_name))


class AlifPinGenerator(boardgen.PinGenerator):
    def __init__(self):
        # Use custom pin type above.
        super().__init__(pin_type=AlifPin)

        # Pre-define the pins (i.e. don't require them to be listed in pins.csv).
        for i in range(NUM_PORTS):
            for j in range(NUM_PINS_PER_PORT):
                self.add_cpu_pin("GPIO{}_{}".format(i, j))

    # Only use pre-defined cpu pins (do not let board.csv create them).
    def find_pin_by_cpu_pin_name(self, cpu_pin_name, create=True):
        return super().find_pin_by_cpu_pin_name(cpu_pin_name, create=False)

    def cpu_table_size(self):
        return "{} * {}".format(NUM_PORTS, NUM_PINS_PER_PORT)


if __name__ == "__main__":
    AlifPinGenerator().main()

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
    "P0_0": (0, 0),
    "P0_1": (0, 1),
    "P0_2": (0, 2),
    "P0_3": (0, 3),
    "P0_4": (0, 4),
    "P0_5": (0, 5),
    "P0_6": (1, 0),
    "P0_7": (1, 1),
    "P1_0": (1, 2),
    "P1_1": (1, 3),
    "P1_2": (1, 4),
    "P1_3": (1, 5),
    "P1_4": (2, 0),
    "P1_5": (2, 1),
    "P1_6": (2, 2),
    "P1_7": (2, 3),
    "P2_0": (2, 4),
    "P2_1": (2, 5),
}


class AlifPin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)
        self._afs = [("NONE", -1)] * 8

    # Called for each AF defined in the csv file for this pin.
    def add_af(self, af_idx, af_name, af):
        if af == "GPIO":
            self._afs[af_idx] = "GPIO", -1
        elif af.startswith("ANA_S"):
            self._afs[af_idx] = "ANA", int(af[5:])
        else:
            m = re.match(r"([A-Z0-9]+[A-Z])(\d*)_([A-Z0-9_]+)$", af)
            periph, unit, line = m.groups()
            unit = -1 if unit == "" else int(unit)
            self._afs[af_idx] = f"{periph}_{line}", unit

    # Emit the struct which contains the pin instance.
    def definition(self):
        port, pin = self.name()[1:].split("_")
        adc12_periph, adc12_channel = ADC12_ANA_MAP.get(self.name(), (3, 7))
        base = "LPGPIO_BASE" if port == "15" else "GPIO{}_BASE".format(port)
        return (
            "{{\n"
            "    .name = MP_QSTR_P{port}_{pin},\n"
            "    .base = {{ .type = &machine_pin_type }},\n"
            "    .gpio = (GPIO_Type *){base},\n"
            "    .port = PORT_{port},\n"
            "    .pin = PIN_{pin},\n"
            "    .adc12_periph = {adc12_periph},\n"
            "    .adc12_channel = {adc12_channel},\n"
            "    .alt = {{{alt}}},\n"
            "}}".format(
                port=port,
                pin=pin,
                base=base,
                adc12_periph=adc12_periph,
                adc12_channel=adc12_channel,
                alt=", ".join([f"MP_HAL_PIN_ALT({func}, {unit})" for func, unit in self._afs]),
            )
        )

    # Alif cpu names must be "Pn_m".
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)

        if not (m := re.match("P([0-9]){1,2}_([0-9])", cpu_pin_name)):
            raise boardgen.PinGeneratorError(
                "Invalid cpu pin name '{}', must be 'Pn_m'".format(cpu_pin_name)
            )

        port = int(m.group(1))
        pin = int(m.group(2))
        if not (0 <= port < NUM_PORTS and 0 <= pin < NUM_PINS_PER_PORT):
            raise boardgen.PinGeneratorError("Unknown cpu pin '{}'".format(cpu_pin_name))


class AlifPinGenerator(boardgen.PinGenerator):
    def __init__(self):
        # Use custom pin type above.
        super().__init__(pin_type=AlifPin, enable_af=True)

        # Pre-define the pins (i.e. don't require them to be listed in pins.csv).
        for i in range(NUM_PORTS):
            for j in range(NUM_PINS_PER_PORT):
                self.add_cpu_pin("P{}_{}".format(i, j))

    # Only use pre-defined cpu pins (do not let board.csv create them).
    def find_pin_by_cpu_pin_name(self, cpu_pin_name, create=True):
        return super().find_pin_by_cpu_pin_name(cpu_pin_name, create=False)

    def cpu_table_size(self):
        return "{} * {}".format(NUM_PORTS, NUM_PINS_PER_PORT)


if __name__ == "__main__":
    AlifPinGenerator().main()

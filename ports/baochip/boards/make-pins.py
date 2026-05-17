#!/usr/bin/env python

import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen

NUM_PORTS = 6  # PA-PF
NUM_PINS_PER_PORT = 16


class BaochipPin(boardgen.Pin):
    # Emit the const struct initialiser for this pin.
    def definition(self):
        name = self.name()  # e.g. "PB14"
        port_letter = name[1]  # 'B'
        pin_num = int(name[2:])  # 14
        port_idx = ord(port_letter) - ord("A")  # 1
        return (
            "{{\n"
            "    .base = {{ .type = &machine_pin_type }},\n"
            "    .name = MP_QSTR_{name},\n"
            "    .port = {port},\n"
            "    .pin  = {pin},\n"
            "}}".format(name=name, port=port_idx, pin=pin_num)
        )

    # Match exactly 'PA0'..'PF15' -- reject leading-zero forms like 'PA00'
    # which would create a distinct name string aliasing the same pin.
    _CPU_NAME_RE = re.compile(r"^P([A-F])(0|[1-9]|1[0-5])$")

    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)
        if not BaochipPin._CPU_NAME_RE.match(cpu_pin_name):
            raise boardgen.PinGeneratorError(
                "Invalid cpu pin name '{}', must be 'PA0'..'PF15' (no leading zeros)".format(
                    cpu_pin_name
                )
            )


class BaochipPinGenerator(boardgen.PinGenerator):
    def __init__(self):
        super().__init__(pin_type=BaochipPin, enable_af=False)
        # Pre-register all 96 pins so unknown names in board CSV are rejected.
        for port in range(NUM_PORTS):
            for pin in range(NUM_PINS_PER_PORT):
                self.add_cpu_pin("P{}{}".format(chr(ord("A") + port), pin))

    # Never create pins on demand from board CSV; only use pre-registered ones.
    def find_pin_by_cpu_pin_name(self, cpu_pin_name, create=True):
        return super().find_pin_by_cpu_pin_name(cpu_pin_name, create=False)

    def cpu_table_size(self):
        return "{} * {}".format(NUM_PORTS, NUM_PINS_PER_PORT)


if __name__ == "__main__":
    BaochipPinGenerator().main()

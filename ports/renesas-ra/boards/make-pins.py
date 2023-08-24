#!/usr/bin/env python

import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen


class RenesasRaPin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)

        self._port = int(cpu_pin_name[1], 16)
        self._bit = int(cpu_pin_name[2:])

        self._adc_name = None
        self._adc_bits = 0
        self._adc_channel = 0

    def add_adc(self, adc):
        # The previous version of make-pins.py handled len==4 to mean
        # adc_bits=8, but this is not used in any of the current af.csv files,
        # so this needs to be validated.
        assert len(adc) == 5 and (adc.startswith("AN0") or adc.startswith("AN1"))
        self._adc_name = adc
        self._adc_bit = 12
        self._adc_channel = int(adc[2:])

    # Called for each AF defined in the csv file for this pin.
    def add_af(self, af_idx, af_name, af):
        if af_idx == 0 and af:
            assert af_name == "Analog"
            self.add_adc(af)

    # Use the PIN() macro defined in ra_pin_prefix.c for defining the pin
    # objects.
    def definition(self):
        # PIN(p_name, p_pin, p_adc)
        adc = "&pin_{}_adc_obj".format(self.name()) if self._adc_name else "NULL"
        return "PIN({:s}, {:d}, {:s})".format(self.name(), self._port * 16 + self._bit, adc)

    # This will be called at the start of the output (after the prefix). Use
    # it to emit the ADC objects (via the PIN_ADC() macro).
    def print_source(self, out_source):
        # PIN_ADC(p_name, p_pin, adc_bit, adc_channel)
        if self._adc_name:
            print(
                "const machine_pin_adc_obj_t pin_{}_adc_obj = PIN_ADC({:s}, {:d}, {:d}, {:d});".format(
                    self.name(),
                    self._adc_name,
                    self._port * 16 + self._bit,
                    self._adc_bits,
                    self._adc_channel,
                ),
                file=out_source,
            )

    # RA cpu names must be "PXNN", where X is hexadecimal port, NN is decimal bit offset.
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)

        if not re.match("^P[0-9A-F][0-9][0-9]$", cpu_pin_name):
            raise boardgen.PinGeneratorError("Invalid cpu pin name '{}'".format(cpu_pin_name))


class RenesasRaPinGenerator(boardgen.PinGenerator):
    def __init__(self):
        # Use custom pin type above, and also enable the --af-csv argument so
        # that add_af gets called on each pin.
        super().__init__(
            pin_type=RenesasRaPin,
            enable_af=True,
        )

    # Override the default implementation just to change the default arguments.
    def parse_af_csv(self, filename):
        return super().parse_af_csv(filename, header_rows=1, pin_col=0, af_col=3)

    # Renesas-specific behavior, we use pin_PNNN for the cpu names, but
    # pyb_pin_FOO for the board names.
    def board_name_define_prefix(self):
        return "pyb_"


if __name__ == "__main__":
    RenesasRaPinGenerator().main()

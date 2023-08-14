#!/usr/bin/env python


from collections import defaultdict, namedtuple
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen


# These are the columns of the af.csv as well as the arguments to the PIN()
# macro in samd_prefix.c.
AFS = {
    "SAMD21": ["eic", "adc0", "sercom1", "sercom2", "tcc1", "tcc2"],
    "SAMD51": ["eic", "adc0", "adc1", "sercom1", "sercom2", "tc", "tcc1", "tcc2"],
}


class SamdPin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)

        # P<port><num> (already verified by validate_cpu_pin_name).
        self._port = cpu_pin_name[1]
        self._pin = int(cpu_pin_name[2:])

        # List of uint8 values from the af.csv. Default to 0xff if not
        # present.
        self._afs = defaultdict(lambda: 0xFF)

    # Called for each AF defined in the csv file for this pin.
    def add_af(self, af_idx, af_name, af):
        # samd-specific: Any pin in the af.csv is implicitly part of the board
        # pins.csv and will therefore be in the Pin.cpu dict. This is
        # equivalent to adding `,Pxy` to the board.csv for every pin in the
        # af.csv.
        self._available = True

        name = AFS[self._generator.args.mcu][af_idx]
        assert name == af_name.lower()
        if name == "eic" or name.startswith("adc"):
            # eic and adc columns are decimal.
            v = int(af)
        else:
            # hex, high nibble is device number, low nibble is pad number.
            v = int(af, 16)
        self._afs[AFS[self._generator.args.mcu][af_idx]] = v

    # Use the PIN() macro defined in samd_prefix.c for defining the pin
    # objects.
    def definition(self):
        # SAMD21: PIN(p_name, p_eic, p_adc0, p_sercom1, p_sercom2, p_tcc1, p_tcc2)
        # SAMD51: PIN(p_name, p_eic, p_adc0, p_adc1, p_sercom1, p_sercom2, p_tc, p_tcc1, p_tcc2)
        return "PIN({:s}, {})".format(
            self.name(),
            ", ".join("0x{:02x}".format(self._afs[x]) for x in AFS[self._generator.args.mcu]),
        )

    # Wrap all definitions to ensure that the ASF defines this pin for this
    # particular MCU.
    def enable_macro(self):
        return "defined(PIN_{})".format(self.name())

    # SAMD cpu names must be "P<port><num>", with zero-prefixed two-digit <num>.
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)

        if not re.match("P[A-D][0-9][0-9]$", cpu_pin_name):
            raise boardgen.PinGeneratorError("Invalid cpu pin name '{}'".format(cpu_pin_name))


class SamdPinGenerator(boardgen.PinGenerator):
    def __init__(self):
        # Use custom pin type above, and also enable the --af-csv argument so
        # that add_af gets called on each pin.
        super().__init__(
            pin_type=SamdPin,
            enable_af=True,
        )

    # Override the default implementation just to change the default arguments
    # (extra header row, skip first column).
    def parse_af_csv(self, filename):
        return super().parse_af_csv(filename, header_rows=1, pin_col=0, af_col=1)

    # We need to know the mcu to emit the correct AF list.
    def extra_args(self, parser):
        parser.add_argument("--mcu")


if __name__ == "__main__":
    SamdPinGenerator().main()

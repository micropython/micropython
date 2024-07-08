#!/usr/bin/env python
"""Creates the pin file for the RP2."""

import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen

# This is NUM_BANK0_GPIOS. Pin indices are 0 to 29 (inclusive).
NUM_GPIOS = None
# Up to 32 additional extended pins (e.g. via the wifi chip or io expanders).
NUM_EXT_GPIOS = None


class Rp2Pin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)
        self._afs = []

        if self.name().startswith("EXT_"):
            self._index = None
            self._ext_index = int(self.name()[8:])  # "EXT_GPIOn"
        else:
            self._index = int(self.name()[4:])  # "GPIOn"
            self._ext_index = None

    # Required by NumericPinGenerator.
    def index(self):
        return self._index

    # Use the PIN() macro defined in rp2_prefix.c for defining the pin
    # objects.
    def definition(self):
        if self._index is not None:
            return "PIN({:d}, GPIO{:d}, 0, {:d}, pin_GPIO{:d}_af)".format(
                self._index, self._index, len(self._afs), self.index()
            )
        else:
            return "PIN({:d}, EXT_GPIO{:d}, 1, 0, NULL)".format(self._ext_index, self._ext_index)

    # External pins need to be mutable (because they track the output state).
    def is_const(self):
        return self._index is not None

    # Add conditional macros only around the external pins based on how many
    # are enabled.
    def enable_macro(self):
        if self._ext_index is not None:
            return "(MICROPY_HW_PIN_EXT_COUNT > {:d})".format(self._ext_index)

    def add_af(self, af_idx, _af_name, af):
        if self._index is None:
            raise boardgen.PinGeneratorError(
                "Cannot add AF for ext pin '{:s}'".format(self.name())
            )

        # <af><unit>_<pin>
        m = re.match("([A-Z][A-Z0-9][A-Z]+)(([0-9]+)(_.*)?)?", af)
        af_fn = m.group(1)
        af_unit = int(m.group(3)) if m.group(3) is not None else 0
        if af_fn == "PIO":
            # Pins can be either PIO unit (unlike, say, I2C where a
            # pin can only be I2C0 _or_ I2C1, both sharing the same AF
            # index), so each PIO unit has a distinct AF index.
            af_fn = "{:s}{:d}".format(af_fn, af_unit)
        self._afs.append((af_idx + 1, af_fn, af_unit, af))

    # This will be called at the start of the output (after the prefix). Use
    # it to emit the af objects (via the AF() macro in rp2_prefix.c).
    def print_source(self, out_source):
        if self._index is not None:
            print(
                "const machine_pin_af_obj_t pin_GPIO{:d}_af[] = {{".format(self.index()),
                file=out_source,
            )
            for af_idx, af_fn, af_unit, af in self._afs:
                print(
                    "    AF({:d}, {:4s}, {:d}), // {:s}".format(af_idx, af_fn, af_unit, af),
                    file=out_source,
                )
            print("};", file=out_source)
            print(file=out_source)

    # rp2 cpu names must be "GPIOn" or "EXT_GPIOn".
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)

        if cpu_pin_name.startswith("GPIO") and cpu_pin_name[4:].isnumeric():
            if not (0 <= int(cpu_pin_name[4:]) < NUM_GPIOS):
                raise boardgen.PinGeneratorError("Unknown cpu pin '{}'".format(cpu_pin_name))
        elif cpu_pin_name.startswith("EXT_GPIO") and cpu_pin_name[8:].isnumeric():
            if not (0 <= int(cpu_pin_name[8:]) < NUM_EXT_GPIOS):
                raise boardgen.PinGeneratorError("Unknown ext pin '{}'".format(cpu_pin_name))
        else:
            raise boardgen.PinGeneratorError(
                "Invalid cpu pin name '{}', must be 'GPIOn' or 'EXT_GPIOn'".format(cpu_pin_name)
            )


class Rp2PinGenerator(boardgen.NumericPinGenerator):
    def __init__(self):
        # Use custom pin type above, and also enable the --af-csv argument.
        super().__init__(
            pin_type=Rp2Pin,
            enable_af=True,
        )

    # Provided by pico-sdk.
    def cpu_table_size(self):
        return "NUM_BANK0_GPIOS"

    # Only use pre-defined cpu pins (do not let board.csv create them).
    def find_pin_by_cpu_pin_name(self, cpu_pin_name, create=True):
        return super().find_pin_by_cpu_pin_name(cpu_pin_name, create=False)

    # NumericPinGenerator doesn't include the cpu dict by default (only the
    # board dict), so add that to the output for rp2.
    def print_source(self, out_source):
        super().print_source(out_source)
        self.print_cpu_locals_dict(out_source)

    def extra_args(self, parser):
        parser.add_argument("--num-gpios", type=int)
        parser.add_argument("--num-ext-gpios", type=int)

    def load_inputs(self, out_source):
        global NUM_GPIOS
        global NUM_EXT_GPIOS

        # Needed by validate_cpu_pin_name
        NUM_GPIOS = self.args.num_gpios
        NUM_EXT_GPIOS = self.args.num_ext_gpios

        if NUM_GPIOS is None:
            raise boardgen.PinGeneratorError("Please pass num-gpios")

        if NUM_EXT_GPIOS is None:
            NUM_EXT_GPIOS = 0
        # Pre-define the pins (i.e. don't require them to be listed in pins.csv).
        for i in range(NUM_GPIOS):
            self.add_cpu_pin("GPIO{}".format(i))
        for i in range(NUM_EXT_GPIOS):
            self.add_cpu_pin("EXT_GPIO{}".format(i))

        super().load_inputs(out_source)


if __name__ == "__main__":
    Rp2PinGenerator().main()

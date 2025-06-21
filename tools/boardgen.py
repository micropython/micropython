# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2023 Jim Mussared
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# This script contains common functionality to assist a port in implementing
# make-pins.py, which is used to emit compile-time definitions of pin, AF, and
# ADC objects based on inputs from the vendor HAL/SDK and the board
# definition's pins.csv.

# The pins.csv file can contain empty lines, comments (a line beginning with "#")
# or pin definition lines.  Pin definition lines must be of the form:
#
#     board,cpu
#
# Where "board" is the user-facing name of the pin as specified by the particular
# board layout and markings, and "cpu" is the corresponding name of the CPU/MCU
# pin.
#
# The "board" entry may be absent if the CPU pin has no additional name, and both
# entries may start with "-" to hide them from the corresponding Python dict of
# pins, and hence hide them from the user (but they are still accessible in C).
#
# For example, take the following pins.csv file:
#
#     X1,PA0
#     -X2,PA1
#     X3,-PA2
#     -X4,-PA3
#     ,PA4
#     ,-PA5
#
# The first row here configures:
# - The CPU pin PA0 is labelled X1.
# - The Python user can access both by the names Pin("X1") and Pin("A0").
# - The Python user can access both by the members Pin.board.X1 and Pin.cpu.A0.
# - In C code they are available as pyb_pin_X1 and pin_A0.
#
# Prefixing the names with "-" hides them from the user.  The following table
# summarises the various possibilities:
#
#     pins.csv entry | board name | cpu name | C board name | C cpu name
#     ---------------+------------+----------+--------------+-----------
#     X1,PA0           "X1"         "A0"       pyb_pin_X1     pin_A0
#     -X2,PA1          -            "A1"       pyb_pin_X2     pin_A1
#     X3,-PA2          "X3"         -          pyb_pin_X3     pin_A2
#     -X4,-PA3         -            -          pyb_pin_X4     pin_A3
#     ,PA4             -            "A4"       -              pin_A4
#     ,-PA5            -            -          -              pin_A5

import argparse
import csv
import os
import sys


class PinGeneratorError(Exception):
    pass


# A port should define a subclass of Pin that knows how to validate cpu/board
# names and emits the required structures.
class Pin:
    def __init__(self, cpu_pin_name):
        self._cpu_pin_name = cpu_pin_name
        # Optional aliases for the board from pins.csv. Each entry is a tuple
        # of (name, hidden). Hidden board pins are in pins.csv with with a "-"
        # prefix and will available to C but not Python.
        self._board_pin_names = set()
        # An unavailable pin is one that is not explicitly mentioned at all in
        # pins.csv (or added explicitly with PinGenerator.add_cpu_pin).
        self._available = False
        # A hidden pin is one that is in pins.csv with a "-" prefix and will
        # be still available to C but not Python.
        self._hidden = False
        # Reference to the PinGenerator instance.
        self._generator = None

    # The name of the pin to use in MP_QSTR_{} or pin_{}. Defaults to the cpu name.
    def name(self):
        return self._cpu_pin_name

    # Add a board alias (e.g. from pins.csv).
    def add_board_pin_name(self, board_pin_name, hidden=False):
        self._board_pin_names.add(
            (
                board_pin_name,
                hidden,
            )
        )

    # Iterate over board pin names in consistent sorted order.
    def board_pin_names(self):
        return sorted(self._board_pin_names, key=lambda x: x[0])

    # Override this to handle an af specified in af.csv.
    def add_af(self, af_idx, af_name, af):
        raise NotImplementedError

    # Override this to verify that naming matches the MCU standard (e.g. "GPIOn" or "PXn").
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        if not cpu_pin_name.strip():
            raise PinGeneratorError("Missing cpu pin name")

    # Override this to provide additional validation of board names.
    @staticmethod
    def validate_board_pin_name(board_pin_name):
        # TODO: ensure this is a valid Python/C identifier and can be used as MP_QSTR_foo.
        pass

    # Must be implemented when using NumericPinGenerator.
    # Returns the integer index, or None to exclude this pin from the table.
    def index(self):
        raise NotImplementedError

    # Can be overridden when using NumericPinGenerator.
    # Returns a string which is a C expression that evaluates to the index
    # e.g. `GPIO_NUM_7`.
    # This is used whenever the index is emitted in source code and defaults
    # to just returning the pin index as a literal.
    # Return None to exclude this pin from the table.
    def index_name(self):
        i = self.index()
        return str(i) if i is not None else None

    # Returns an expression that defines the pin. e.g. `{ .base { ... }, .x }`.
    # This is used as the RHS of the `const machine_pin_obj_t
    # pin_EXT_GPIO0_obj =` statements for named pins, and elements of
    # `machine_pin_obj_table` for numeric pins.
    # This will typically might be implemented as an invocation of a macro
    # defined in the port-specific prefix.
    def definition(self):
        raise NotImplementedError

    # Whether the pin object should be declared as "const". This should be True
    # for most pins, but some special cases (e.g. external pins on rp2) might
    # need mutable pin objects (e.g. to track current pin state).
    def is_const(self):
        return True

    # Optionally return a preprocessor expression that can be used to suppress
    # this pin (e.g. `MICROPY_HW_ENABLE_GPIOn`).
    def enable_macro(self):
        return None

    # Override this to output any additional per-pin definitions or other
    # content that should appear at the start of the source output.
    # This could be used to define additional objects such as IRQs or AFs.
    def print_source(self, out_source):
        pass


# A port should define a subclass of PinGenerator (or NumericPinGenerator).
class PinGenerator:
    def __init__(self, pin_type, enable_af=False):
        self._pins = []
        self._pin_type = pin_type
        self._enable_af = enable_af
        self._pin_cpu_num_entries = 0
        self._pin_board_num_entries = 0

    # Allows a port to define a known cpu pin (without relying on it being in the
    # csv file).
    def add_cpu_pin(self, cpu_pin_name, available=True):
        pin = self._pin_type(cpu_pin_name)
        pin._available = available
        self._pins.append(pin)
        pin._generator = self
        return pin

    # Iterate just the available pins (i.e. ones in pins.csv).
    def available_pins(self, exclude_hidden=False):
        for pin in self._pins:
            if not pin._available:
                continue
            if exclude_hidden and pin._hidden:
                continue
            yield pin

    # Allows a port to add additional command-line arguments to be handled.
    def extra_args(self, parser):
        pass

    # Load board->cpu mapping from csv.
    def parse_board_csv(self, filename):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for linenum, row in enumerate(rows):
                try:
                    # Skip empty lines, and lines starting with "#".
                    if len(row) == 0 or row[0].startswith("#"):
                        continue

                    # Lines must be pairs of names.
                    if len(row) != 2:
                        raise PinGeneratorError("Expecting two entries in each row")
                    board_pin_name, cpu_pin_name = (x.strip() for x in row)

                    # All rows must include a cpu name.
                    cpu_hidden = False
                    if cpu_pin_name.startswith("-"):
                        cpu_hidden = True
                        cpu_pin_name = cpu_pin_name[1:]
                    self._pin_type.validate_cpu_pin_name(cpu_pin_name)
                    pin = self.find_pin_by_cpu_pin_name(cpu_pin_name, create=True)
                    pin._available = True  # It's in pins.csv so must be available.
                    pin._hidden = cpu_hidden  # Optionally don't make available to Python.

                    # Rows can optionally alias to a board name.
                    if board_pin_name:
                        board_hidden = False
                        if board_pin_name.startswith("-"):
                            board_hidden = True
                            board_pin_name = board_pin_name[1:]
                        self._pin_type.validate_board_pin_name(board_pin_name)
                        pin.add_board_pin_name(board_pin_name, board_hidden)

                # Inject "file:line: " into the exception.
                except PinGeneratorError as er:
                    raise PinGeneratorError("{}:{}: {}".format(filename, linenum, er))

    def parse_af_csv(self, filename, header_rows=1, pin_col=0, af_col=1):
        headings = {}
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for linenum, row in enumerate(rows):
                try:
                    # Skip empty lines, and lines starting with "#".
                    if len(row) == 0 or row[0].startswith("#"):
                        continue

                    # Consume `header_rows` non-blank/comment rows at the start.
                    if header_rows:
                        if not headings:
                            # If this is the first header row then initialise
                            # the headings dict.
                            for af_idx, header in enumerate(row[af_col:]):
                                headings[af_idx] = header.strip()
                        header_rows -= 1
                        continue

                    # Lines must be pairs of names.
                    if len(row) <= max(pin_col, af_col):
                        raise PinGeneratorError(
                            "Expecting {} entries in each row".format(max(pin_col, af_col))
                        )

                    cpu_pin_name = row[pin_col].strip()
                    if cpu_pin_name == "-":
                        continue
                    self._pin_type.validate_cpu_pin_name(cpu_pin_name)
                    pin = self.find_pin_by_cpu_pin_name(cpu_pin_name, create=True)

                    for af_idx, af in enumerate(row[af_col:]):
                        af = af.strip()
                        if not af:
                            continue
                        pin.add_af(af_idx, headings.get(af_idx, ""), af)

                # Inject "file:line: " into the exception.
                except PinGeneratorError as er:
                    raise PinGeneratorError("{}:{}: {}".format(filename, linenum, er))

    # Find an existing pin.
    def find_pin_by_cpu_pin_name(self, cpu_pin_name, create=True):
        for pin in self._pins:
            if pin._cpu_pin_name == cpu_pin_name:
                return pin
        if create:
            return self.add_cpu_pin(cpu_pin_name, available=False)
        else:
            raise PinGeneratorError("Unknown cpu pin {}".format(cpu_pin_name))

    # Print the locals dict for Pin.board.
    def print_board_locals_dict(self, out_source):
        print(file=out_source)
        print(
            "static const mp_rom_map_elem_t machine_pin_board_pins_locals_dict_table[] = {",
            file=out_source,
        )
        for pin in self.available_pins():
            for board_pin_name, board_hidden in pin.board_pin_names():
                if board_hidden:
                    # Don't include hidden pins in Pins.board.
                    continue

                # Keep track of the total number of Pin.board entries.
                self._pin_board_num_entries += 1

                # We don't use the enable macro for board pins, because they
                # shouldn't be referenced in pins.csv unless they're
                # available.
                print(
                    "    {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(pin_{:s}) }},".format(
                        board_pin_name,
                        pin.name(),
                    ),
                    file=out_source,
                )
        print("};", file=out_source)
        print(
            "MP_DEFINE_CONST_DICT(machine_pin_board_pins_locals_dict, machine_pin_board_pins_locals_dict_table);",
            file=out_source,
        )

    # Print the locals dict for Pin.cpu.
    def print_cpu_locals_dict(self, out_source):
        print(file=out_source)
        print(
            "static const mp_rom_map_elem_t machine_pin_cpu_pins_locals_dict_table[] = {",
            file=out_source,
        )
        for pin in self.available_pins(exclude_hidden=True):
            # Keep track of the total number of Pin.cpu entries.
            self._pin_cpu_num_entries += 1

            m = pin.enable_macro()
            if m:
                print("    #if {}".format(m), file=out_source)
            print(
                "    {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(pin_{:s}) }},".format(
                    pin.name(),
                    pin.name(),
                ),
                file=out_source,
            )
            if m:
                print("    #endif", file=out_source)
        print("};", file=out_source)
        print(
            "MP_DEFINE_CONST_DICT(machine_pin_cpu_pins_locals_dict, machine_pin_cpu_pins_locals_dict_table);",
            file=out_source,
        )

    # NumericPinGenerator can override this to use an entry in machine_pin_obj_table.
    def _cpu_pin_pointer(self, pin):
        return "&pin_{:s}_obj".format(pin.name())

    # Allow a port to prefix the board pin macro names with something.
    # e.g. STM32 does pyb_pin_NAME whereas other ports using pin_NAME.
    def board_name_define_prefix(self):
        return ""

    # Print the pin_CPUNAME and pin_BOARDNAME macros.
    def print_defines(self, out_header, cpu=True, board=True):
        # Provide #defines for the number of cpu and board pins.
        print(
            "#define MICROPY_PY_MACHINE_PIN_CPU_NUM_ENTRIES ({})".format(
                self._pin_cpu_num_entries
            ),
            file=out_header,
        )
        print(
            "#define MICROPY_PY_MACHINE_PIN_BOARD_NUM_ENTRIES ({})".format(
                self._pin_board_num_entries
            ),
            file=out_header,
        )

        # Provide #defines for each cpu pin.
        for pin in self.available_pins():
            print(file=out_header)
            m = pin.enable_macro()
            if m:
                print("#if {}".format(m), file=out_header)

            # #define pin_CPUNAME (...)
            if cpu:
                print(
                    "#define pin_{:s} ({:s})".format(pin.name(), self._cpu_pin_pointer(pin)),
                    file=out_header,
                )

            # #define pin_BOARDNAME (pin_CPUNAME)
            if board:
                for board_pin_name, _board_hidden in pin.board_pin_names():
                    # Note: Hidden board pins are still available to C via the macro.
                    # Note: The RHS isn't wrapped in (), which is necessary to make the
                    # STATIC_AF_ macro work on STM32.
                    print(
                        "#define {:s}pin_{:s} pin_{:s}".format(
                            self.board_name_define_prefix(),
                            board_pin_name,
                            pin.name(),
                        ),
                        file=out_header,
                    )

            if m:
                print("#endif", file=out_header)

    def print_pin_objects(self, out_source):
        print(file=out_source)
        for pin in self.available_pins():
            m = pin.enable_macro()
            if m:
                print("#if {}".format(m), file=out_source)
            print(
                "{:s}machine_pin_obj_t pin_{:s}_obj = {:s};".format(
                    "const " if pin.is_const() else "",
                    pin.name(),
                    pin.definition(),
                ),
                file=out_source,
            )
            if m:
                print("#endif", file=out_source)

    def print_pin_object_externs(self, out_header):
        print(file=out_header)
        for pin in self.available_pins():
            m = pin.enable_macro()
            if m:
                print("#if {}".format(m), file=out_header)
            print(
                "extern {:s}machine_pin_obj_t pin_{:s}_obj;".format(
                    "const " if pin.is_const() else "",
                    pin.name(),
                ),
                file=out_header,
            )
            if m:
                print("#endif", file=out_header)

    def print_source(self, out_source):
        self.print_pin_objects(out_source)
        self.print_cpu_locals_dict(out_source)
        self.print_board_locals_dict(out_source)

    def print_header(self, out_header):
        self.print_pin_object_externs(out_header)
        self.print_defines(out_header)

    # A port can override this if it has extra input files (e.g. af.csv) to load.
    def load_inputs(self, out_source):
        # Optionally load pins.csv to get cpu->board name mappings.
        if self._enable_af and self.args.af_csv:
            print("// --af-csv {:s}".format(self.args.af_csv), file=out_source)
            self.parse_af_csv(self.args.af_csv)

        # Optionally load pins.csv to get cpu->board name mappings.
        if self.args.board_csv:
            print("// --board-csv {:s}".format(self.args.board_csv), file=out_source)
            self.parse_board_csv(self.args.board_csv)

        # Prepend the prefix file to the start of the output.
        if self.args.prefix:
            print("// --prefix {:s}".format(self.args.prefix), file=out_source)
            print(file=out_source)
            with open(self.args.prefix, "r") as prefix_file:
                print(prefix_file.read(), end="", file=out_source)

    # A port can override this to do extra work after the main source+header
    # have been written, such as generating additional header files.
    def generate_extra_files(self):
        pass

    def main(self):
        parser = argparse.ArgumentParser(description="Generate board specific pin file")
        parser.add_argument("--board-csv")
        if self._enable_af:
            parser.add_argument("--af-csv")
        parser.add_argument("--prefix")
        parser.add_argument("--output-source")
        parser.add_argument("--output-header")
        self.extra_args(parser)
        self.args = parser.parse_args()

        try:
            with open(self.args.output_source, "w") as out_source:
                print("// This file was automatically generated by make-pins.py", file=out_source)
                print("//", file=out_source)

                # Load additional files (including port-specific ones).
                self.load_inputs(out_source)

                # Allow a port to print arbitrary per-pin content.
                for pin in self.available_pins():
                    pin.print_source(out_source)

                # Print the tables and dictionaries.
                self.print_source(out_source)

            with open(self.args.output_header, "w") as out_header:
                self.print_header(out_header)

            self.generate_extra_files()
        except PinGeneratorError as er:
            print(er)
            sys.exit(1)


# For ports that use numeric pin identifiers (e.g. ESP32, rp2).
# This emits the machine_pin_obj_t instances as an array (machine_pin_obj_table).
class NumericPinGenerator(PinGenerator):
    # This should return a const expression that is the number of GPIO pins
    # for this board.
    def cpu_table_size(self):
        raise NotImplementedError

    def print_cpu_table(self, out_source):
        # Print machine_pin_obj_table, where each element is `[n] = {obj}`.
        print(file=out_source)
        print(
            "const machine_pin_obj_t machine_pin_obj_table[{}] = {{".format(self.cpu_table_size()),
            file=out_source,
        )
        for pin in self.available_pins():
            n = pin.index_name()
            if n is None:
                continue

            m = pin.enable_macro()
            if m:
                print("    #if {}".format(m), file=out_source)
            print(
                "    [{:s}] = {:s},".format(
                    pin.index_name(),
                    pin.definition(),
                ),
                file=out_source,
            )
            if m:
                print("    #endif", file=out_source)
        print("};", file=out_source)

        # For pins that do not have an index, print them out in the same style as PinGenerator.
        print(file=out_source)
        for pin in self.available_pins():
            n = pin.index_name()
            if n is not None:
                continue

            m = pin.enable_macro()
            if m:
                print("#if {}".format(m), file=out_source)
            print(
                "{:s}machine_pin_obj_t pin_{:s}_obj = {:s};".format(
                    "const " if pin.is_const() else "",
                    pin.name(),
                    pin.definition(),
                ),
                file=out_source,
            )
            if m:
                print("#endif", file=out_source)

    # Replace PinGenerator's implementation to print the numeric table.
    def print_source(self, out_source):
        self.print_cpu_table(out_source)
        self.print_board_locals_dict(out_source)

    # Replace PinGenerator's implementation to only print the defines.
    def print_header(self, out_header):
        self.print_defines(out_header)

    def _cpu_pin_pointer(self, pin):
        n = pin.index_name()
        if n is not None:
            return "&machine_pin_obj_table[{:s}]".format(pin.index_name())
        else:
            return super()._cpu_pin_pointer(pin)

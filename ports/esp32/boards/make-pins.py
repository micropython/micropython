#!/usr/bin/env python

import argparse
import sys
import csv
import re

MAX_CPU_PINS = 49


def parse_pin(name_str):
    """Parses a string and returns a pin number."""
    if len(name_str) < 2:
        raise ValueError("Expecting pin name to be at least 2 characters.")
    if not name_str.startswith("GPIO"):
        raise ValueError("Expecting pin name to start with GPIO")
    return int(re.findall(r"\d+$", name_str)[0])


class Pin:
    def __init__(self, pin):
        self.pin = pin
        self.is_board = False

    def cpu_pin_name(self):
        return "GPIO{:d}".format(self.pin)

    def is_board_pin(self):
        return self.is_board

    def set_is_board_pin(self):
        self.is_board = True


class NamedPin:
    def __init__(self, name, pin):
        self._name = name
        self._pin = pin

    def pin(self):
        return self._pin

    def name(self):
        return self._name


class Pins:
    def __init__(self):
        self.cpu_pins = []  # list of NamedPin objects
        self.board_pins = []  # list of NamedPin objects

    def find_pin(self, pin_name):
        for pin in self.cpu_pins:
            if pin.name() == pin_name:
                return pin.pin()

    def create_pins(self):
        for pin_num in range(MAX_CPU_PINS):
            pin = Pin(pin_num)
            self.cpu_pins.append(NamedPin(pin.cpu_pin_name(), pin))

    def parse_board_file(self, filename):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                if len(row) == 0 or row[0].startswith("#"):
                    # Skip empty lines, and lines starting with "#"
                    continue
                if len(row) != 2:
                    raise ValueError("Expecting two entries in a row")

                cpu_pin_name = row[1]
                parse_pin(cpu_pin_name)
                pin = self.find_pin(cpu_pin_name)
                if not pin:
                    raise ValueError("Unknown pin {}".format(cpu_pin_name))
                pin.set_is_board_pin()
                if row[0]:  # Only add board pins that have a name
                    self.board_pins.append(NamedPin(row[0], pin))

    def print_table(self, label, named_pins, out_source):
        print("", file=out_source)
        print(
            "const machine_{}_obj_t machine_{}_obj_table[GPIO_NUM_MAX] = {{".format(label, label),
            file=out_source,
        )
        for pin in named_pins:
            print("    #if MICROPY_HW_ENABLE_{}".format(pin.name()), file=out_source)
            print(
                "    [GPIO_NUM_{}] = {{ .base = {{ .type = &machine_{}_type }} }},".format(
                    pin.pin().pin, label
                ),
                file=out_source,
            )
            print("    #endif", file=out_source)
        print("};", file=out_source)

    def print_named(self, label, named_pins, out_source):
        print("", file=out_source)
        print(
            "STATIC const mp_rom_map_elem_t machine_pin_{:s}_pins_locals_dict_table[] = {{".format(
                label
            ),
            file=out_source,
        )
        for named_pin in named_pins:
            pin = named_pin.pin()
            print(
                "  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(&pin_{:s}) }},".format(
                    named_pin.name(), pin.cpu_pin_name()
                ),
                file=out_source,
            )

        print("};", file=out_source)
        print(
            "MP_DEFINE_CONST_DICT(machine_pin_{:s}_pins_locals_dict, machine_pin_{:s}_pins_locals_dict_table);".format(
                label, label
            ),
            file=out_source,
        )

    def print_tables(self, out_source):
        self.print_table("pin", self.cpu_pins, out_source)
        self.print_table("pin_irq", self.cpu_pins, out_source)
        self.print_named("board", self.board_pins, out_source)

    def print_header(self, out_header):
        # Provide #defines for each cpu pin.
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            n = pin.cpu_pin_name()
            print("#if MICROPY_HW_ENABLE_{}".format(n), file=out_header)
            print(
                "#define pin_{:s} (machine_pin_obj_table[{}])".format(n, pin.pin),
                file=out_header,
            )
            print("#endif", file=out_header)

        # Provide #define's mapping board to cpu name.
        for named_pin in self.board_pins:
            if named_pin.pin().is_board_pin():
                print(
                    "#define pin_{:s} pin_{:s}".format(
                        named_pin.name(), named_pin.pin().cpu_pin_name()
                    ),
                    file=out_header,
                )


def main():
    parser = argparse.ArgumentParser(description="Generate board specific pin file")
    parser.add_argument("--board-csv")
    parser.add_argument("--prefix")
    parser.add_argument("--output-source")
    parser.add_argument("--output-header")
    args = parser.parse_args(sys.argv[1:])

    pins = Pins()
    pins.create_pins()

    if args.board_csv:
        pins.parse_board_file(args.board_csv)

    with open(args.output_source, "w") as out_source:
        print("// This file was automatically generated by make-pins.py", file=out_source)
        print("//", file=out_source)

        if args.board_csv:
            print("// --board-csv {:s}".format(args.board_csv), file=out_source)

        if args.prefix:
            print("// --prefix {:s}".format(args.prefix), file=out_source)
            print("", file=out_source)
            with open(args.prefix, "r") as prefix_file:
                print(prefix_file.read(), end="", file=out_source)

        pins.print_tables(out_source)

    with open(args.output_header, "w") as out_header:
        pins.print_header(out_header)


if __name__ == "__main__":
    main()

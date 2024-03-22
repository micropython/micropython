#!/usr/bin/env python
"""Generates the pins file for the CC3200."""

# Do not use this as a reference for new ports. See tools/boardgen.py and e.g.
# ports/stm32/boards/make-pins.py.

from __future__ import print_function

import argparse
import sys
import csv


SUPPORTED_AFS = {
    "UART": ("TX", "RX", "RTS", "CTS"),
    "SPI": ("CLK", "MOSI", "MISO", "CS0"),
    #'I2S': ('CLK', 'FS', 'DAT0', 'DAT1'),
    "I2C": ("SDA", "SCL"),
    "TIM": ("PWM"),
    "SD": ("CLK", "CMD", "DAT0"),
    "ADC": ("CH0", "CH1", "CH2", "CH3"),
}


def parse_port_pin(name_str):
    """Parses a string and returns a (port, gpio_bit) tuple."""
    if len(name_str) < 3:
        raise ValueError("Expecting pin name to be at least 3 characters")
    if name_str[:2] != "GP":
        raise ValueError("Expecting pin name to start with GP")
    if not name_str[2:].isdigit():
        raise ValueError("Expecting numeric GPIO number")
    port = int(int(name_str[2:]) / 8)
    gpio_bit = 1 << int(int(name_str[2:]) % 8)
    return (port, gpio_bit)


class AF:
    """Holds the description of an alternate function"""

    def __init__(self, name, idx, fn, unit, type):
        self.name = name
        self.idx = idx
        if self.idx > 15:
            self.idx = -1
        self.fn = fn
        self.unit = unit
        self.type = type

    def print(self, out_source):
        print(
            "    AF({:16s}, {:4d}, {:8s}, {:4d}, {:8s}),    // {}".format(
                self.name, self.idx, self.fn, self.unit, self.type, self.name
            ),
            file=out_source,
        )


class Pin:
    """Holds the information associated with a pin."""

    def __init__(self, name, port, gpio_bit, pin_num):
        self.name = name
        self.port = port
        self.gpio_bit = gpio_bit
        self.pin_num = pin_num
        self.board_pin = False
        self.afs = []

    def add_af(self, af):
        self.afs.append(af)

    def print(self, out_source):
        print("// {}".format(self.name), file=out_source)
        if len(self.afs):
            print("const pin_af_t pin_{}_af[] = {{".format(self.name), file=out_source)
            for af in self.afs:
                af.print(out_source)
            print("};", file=out_source)
            print(
                "pin_obj_t pin_{:4s} = PIN({:6s}, {:1d}, {:3d}, {:2d}, pin_{}_af, {});\n".format(
                    self.name,
                    self.name,
                    self.port,
                    self.gpio_bit,
                    self.pin_num,
                    self.name,
                    len(self.afs),
                ),
                file=out_source,
            )
        else:
            print(
                "pin_obj_t pin_{:4s} = PIN({:6s}, {:1d}, {:3d}, {:2d}, NULL, 0);\n".format(
                    self.name, self.name, self.port, self.gpio_bit, self.pin_num
                ),
                file=out_source,
            )

    def print_header(self, out_header):
        print("extern pin_obj_t pin_{:s};".format(self.name), file=out_header)


class Pins:
    def __init__(self):
        self.board_pins = []  # list of pin objects

    def find_pin(self, port, gpio_bit):
        for pin in self.board_pins:
            if pin.port == port and pin.gpio_bit == gpio_bit:
                return pin

    def find_pin_by_num(self, pin_num):
        for pin in self.board_pins:
            if pin.pin_num == pin_num:
                return pin

    def find_pin_by_name(self, name):
        for pin in self.board_pins:
            if pin.name == name:
                return pin

    def parse_af_file(self, filename, pin_col, pinname_col, af_start_col):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    (port_num, gpio_bit) = parse_port_pin(row[pinname_col])
                except:
                    continue
                if not row[pin_col].isdigit():
                    raise ValueError(
                        "Invalid pin number {:s} in row {:s}".format(row[pin_col], row)
                    )
                # Pin numbers must start from 0 when used with the TI API
                pin_num = int(row[pin_col]) - 1
                pin = Pin(row[pinname_col], port_num, gpio_bit, pin_num)
                self.board_pins.append(pin)
                af_idx = 0
                for af in row[af_start_col:]:
                    af_splitted = af.split("_")
                    fn_name = af_splitted[0].rstrip("0123456789")
                    if fn_name in SUPPORTED_AFS:
                        type_name = af_splitted[1]
                        if type_name in SUPPORTED_AFS[fn_name]:
                            unit_idx = af_splitted[0][-1]
                            pin.add_af(AF(af, af_idx, fn_name, int(unit_idx), type_name))
                    af_idx += 1

    def parse_board_file(self, filename, cpu_pin_col):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                # Pin numbers must start from 0 when used with the TI API
                if row[cpu_pin_col].isdigit():
                    pin = self.find_pin_by_num(int(row[cpu_pin_col]) - 1)
                else:
                    pin = self.find_pin_by_name(row[cpu_pin_col])
                if pin:
                    pin.board_pin = True

    def print_named(self, label, pins, out_source):
        print("", file=out_source)
        print(
            "static const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label),
            file=out_source,
        )
        for pin in pins:
            if pin.board_pin:
                print(
                    "    {{ MP_ROM_QSTR(MP_QSTR_{:6s}), MP_ROM_PTR(&pin_{:6s}) }},".format(
                        pin.name, pin.name
                    ),
                    file=out_source,
                )
        print("};", file=out_source)
        print(
            "MP_DEFINE_CONST_DICT(pin_{:s}_pins_locals_dict, pin_{:s}_pins_locals_dict_table);".format(
                label, label
            ),
            file=out_source,
        )

    def print(self, out_source):
        for pin in self.board_pins:
            if pin.board_pin:
                pin.print(out_source)
        self.print_named("board", self.board_pins, out_source)
        print("", file=out_source)

    def print_header(self, out_header):
        for pin in self.board_pins:
            if pin.board_pin:
                pin.print_header(out_header)


def main():
    parser = argparse.ArgumentParser(description="Generate board specific pin file")
    parser.add_argument("--board-csv")
    parser.add_argument("--af-csv")
    parser.add_argument("--prefix")
    parser.add_argument("--output-source")
    parser.add_argument("--output-header")
    args = parser.parse_args()

    pins = Pins()

    with open(args.output_source, "w") as out_source:
        print("// This file was automatically generated by make-pins.py", file=out_source)
        print("//", file=out_source)
        if args.af_csv:
            print("// --af {:s}".format(args.af_csv), file=out_source)
            pins.parse_af_file(args.af_csv, 0, 1, 3)

        if args.board_csv:
            print("// --board {:s}".format(args.board_csv), file=out_source)
            pins.parse_board_file(args.board_csv, 1)

        if args.prefix:
            print("// --prefix {:s}".format(args.prefix), file=out_source)
            print("", file=out_source)
            with open(args.prefix, "r") as prefix_file:
                print(prefix_file.read(), file=out_source)
        pins.print(out_source)

    with open(args.output_header, "w") as out_header:
        pins.print_header(out_header)


if __name__ == "__main__":
    main()

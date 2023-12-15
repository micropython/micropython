from __future__ import print_function
import argparse
import sys
import csv
import re
import os

prefix_content = '#include <stdio.h> \n \
#include "py/obj.h" \n \
#include "py/mphal.h" \n \
#include "machine_pin_phy.h"\n '


class NamedPin(object):
    def __init__(self, name, pin):
        self._name = name
        self._pin = pin

    def pin(self):
        return self._pin

    def name(self):
        return self._name


class Pin(object):
    def __init__(self, name, pin_addr, pin_exp):
        self._name = name
        self._pin_addr = pin_addr
        self._pin_exp = pin_exp
        self._pin_func = "PIN_PHY_FUNC_NONE"
        self._board_pin = False

    def cpu_pin_name(self):
        return self._name

    def is_board_pin(self):
        return self._board_pin

    def set_is_board_pin(self):
        self._board_pin = True

    def set_board_index(self, index):
        self.board_index = index

    def print(self):
        """print(
            "const machine_pin_phy_obj_t pin_{:s}_obj = PIN({:s}, {:s});".format(
                self._name,
                self._name,
                self._pin_addr,
            )
        )"""

    def print_header(self, num_pins, hdr_file):
        hdr_file.write("#define MAX_IO_PINS {:d} \n".format(num_pins))

    def qstr_list(self):
        return [self._name]

    def print_const_table_entry(self):
        print('{{{:s}, "{:s}", {:s}}},'.format(self._pin_exp, self._name, self._pin_func))


class Pins(object):
    def __init__(self):
        self.cpu_pins = []  # list of NamedPin objects
        self.board_pins = []  # list of NamedPin objects
        self.board_pin_csv_path = ""
        self.num_cpu_pins = 0

    def update_num_cpu_pins(self):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.set_board_index(self.num_cpu_pins)
                self.num_cpu_pins += 1

    def get_num_cpu_pins(self):
        return self.num_cpu_pins

    def find_pin(self, cpu_pin_name):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.cpu_pin_name() == cpu_pin_name:
                return pin

    def print_const_table(self):
        print("")
        print(prefix_content)
        print("const uint8_t machine_pin_num_of_cpu_pins = {:d};".format(self.get_num_cpu_pins()))
        print("")
        print(
            "machine_pin_phy_obj_t machine_pin_phy_obj[{:d}] = {{".format(self.get_num_cpu_pins())
        )
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.print_const_table_entry()
        print("};")

    # ToDo: Complete for alternate functions
    def parse_af_file(self):
        with open("./pins_af.csv", "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    pin_name = row[0]
                    pin_addr = row[1]
                    pin_exp = row[2].strip('"')
                except:
                    continue
                pin = Pin(pin_name, pin_addr, pin_exp)
                self.cpu_pins.append(NamedPin(pin_name, pin))

    def parse_board_file(self):
        # Assuming the same path has required board files
        with open("./pins.csv", "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    board_pin_name = row[0]
                    cpu_pin_name = row[1]
                except:
                    continue
                pin = self.find_pin(cpu_pin_name)
                if pin:
                    pin.set_is_board_pin()
                    self.board_pins.append(NamedPin(board_pin_name, pin))

    def print_named(self, label, named_pins):
        print(
            "STATIC const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label)
        )
        num_cpu_pins = 0
        for named_pin in named_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                print(
                    "  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(&machine_pin_phy_obj[{:d}]) }},".format(
                        named_pin.name(), num_cpu_pins
                    )
                )
            num_cpu_pins += 1
        print("};")
        print(
            "MP_DEFINE_CONST_DICT(pin_{:s}_pins_locals_dict, pin_{:s}_pins_locals_dict_table);".format(
                label, label
            )
        )

    def print(self):
        self.print_named("cpu", self.cpu_pins)
        print("")

    def print_header(self, hdr_filename):
        with open(hdr_filename, "wt") as hdr_file:
            hdr_file.write("#define MAX_IO_PINS {:d} \n".format(self.get_num_cpu_pins()))

    def print_qstr(self, qstr_filename):
        with open(qstr_filename, "wt") as qstr_file:
            qstr_set = set([])
            for named_pin in self.cpu_pins:
                pin = named_pin.pin()
                if pin.is_board_pin():
                    qstr_set |= set(pin.qstr_list())
                    qstr_set |= set([named_pin.name()])
            for named_pin in self.board_pins:
                qstr_set |= set([named_pin.name()])
            for qstr in sorted(qstr_set):
                # cond_var = None
                print("Q({})".format(qstr), file=qstr_file)


def main():
    parser = argparse.ArgumentParser(
        prog="make-pins.py",
        usage="%(prog)s [options] [command]",
        description="Generate board specific pin file",
    )

    parser.add_argument(
        "-q",
        "--qstr",
        dest="qstr_filename",
        help="Specifies name of generated qstr header file",
    )

    parser.add_argument(
        "-r",
        "--hdr",
        dest="hdr_filename",
        help="Specifies name of generated pin header file",
    )

    args = parser.parse_args(sys.argv[1:])

    pins = Pins()

    pins.parse_af_file()

    if args.hdr_filename and args.qstr_filename:
        pins.parse_board_file()
        pins.update_num_cpu_pins()
        pins.print_const_table()
        pins.print()
        pins.print_header(args.hdr_filename)
        pins.print_qstr(args.qstr_filename)


if __name__ == "__main__":
    main()

from __future__ import print_function
from itertools import islice
import argparse
import sys
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


class PinsTable(object):
    def __init__(self):
        self.pins = []

    def add_pin(self, pin_name, pin_addr):
        self.pins.append([pin_name, pin_addr])

    def get_pins(self):
        return self.pins


class PinsDetailsTable(object):
    def __init__(self):
        self.pins_details = []

    def add_pin_details(self, pin_name, pin_addr, pin_exp):
        self.pins_details.append([pin_name, pin_addr, pin_exp])

    def get_pins_details(self):
        return self.pins_details


class Pin(object):
    def __init__(self, name, pin_addr, pin_exp):
        self._name = name
        self._pin_addr = pin_addr
        self._pin_exp = pin_exp
        self._board_pin = False

    def cpu_pin_name(self):
        return self._name

    def is_board_pin(self):
        return self._board_pin

    def set_is_board_pin(self):
        self._board_pin = True

    def set_board_index(self, index):
        self.board_index = index

    def print_header(self, num_pins, hdr_file):
        hdr_file.write("#define MAX_IO_PINS {:d} \n".format(num_pins))

    def qstr_list(self):
        return [self._name]

    def print_const_table_entry(self):
        print('{{{:s}, "{:s}"}},'.format(self._pin_exp, self._name))


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

    def parse_pin_details_table(self, pins_details_table):
        for pin in pins_details_table.get_pins_details():
            pin_name = pin[0]
            pin_addr = pin[1]
            pin_exp = pin[2].strip('"')
            pin = Pin(pin_name, pin_addr, pin_exp)
            self.cpu_pins.append(NamedPin(pin_name, pin))

    def parse_board_table(self, pins_table):
        for pin in pins_table.get_pins():
            board_pin_name = pin[0]
            cpu_pin_name = pin[1]
            pin = self.find_pin(cpu_pin_name)
            if pin:
                pin.set_is_board_pin()
                self.board_pins.append(NamedPin(board_pin_name, pin))

    def print_named(self, label, named_pins):
        print(
            "static const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label)
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
                print("Q({})".format(qstr), file=qstr_file)

    def get_pin_addr_helper(self, pin_def):
        pattern = r"CYHAL_PORT_(\d+),\s*(\d+)"
        match = re.search(pattern, pin_def)
        port_number = match.group(1)
        pin_number = match.group(2)
        return (int(port_number) << 3) + int(pin_number)

    def get_pin_package_path(self, filename):
        root_dir = "./mtb_shared/mtb-hal-cat1"
        mid_dir = "COMPONENT_CAT1A/include/pin_packages"
        for dirpath, dirnames, filenames in os.walk(root_dir):
            for dirname in dirnames:
                if dirname.startswith("release-"):
                    release_version = dirname
                    file_path = os.path.join(root_dir, release_version, mid_dir, filename)
                    if os.path.isfile(file_path):
                        return file_path
            return None

    def generate_pins_table(self, pin_pacakge_file_name):
        file_path = self.get_pin_package_path(pin_pacakge_file_name)

        if file_path is None:
            sys.exit(1)

        with open(file_path, "r") as file:
            content = file.read()

        enum_start = content.find("typedef enum {")
        enum_end = content.find("}")

        if enum_start != -1 and enum_end != -1:
            enum_content = content[enum_start:enum_end]
            enum_values = re.findall(r"\b(\w+)\s*=", enum_content)
            pin_name = re.findall(r"\b(?!NC\b)(\w+)\s*=", enum_content)
            pin_defs = re.findall(r"=\s*(.*?\))", enum_content)

            pins_table = PinsTable()
            pins_details_table = PinsDetailsTable()

            for value in enum_values:
                if value.startswith("P") or value.startswith("N") or value.startswith("U"):
                    pins_table.add_pin(value, value)

            pins_details_table.add_pin_details("NC", 255, "CYHAL_GET_GPIO(CYHAL_PORT_31, 7)")
            for pname, pdef in zip(pin_name, pin_defs):
                val = self.get_pin_addr_helper(pdef)
                pins_details_table.add_pin_details(pname, val, pdef.strip('"'))
            return pins_table, pins_details_table
        else:
            print("// Error: pins table and pins details table generation failed")


def main():
    parser = argparse.ArgumentParser(
        prog="make-pins.py",
        usage="%(prog)s [options] [command]",
        description="Generate board specific pin details",
    )

    parser.add_argument(
        "-g",
        "--gen-pin-for",
        dest="pin_package_filename",
        help="Specifies the pin package file from mtb assets to generate pin details",
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

    if args.pin_package_filename:
        print("// Generating pins table")
        print("// - --gen-pin-for {:s}".format(args.pin_package_filename))
        pins_table, pins_details_table = pins.generate_pins_table(args.pin_package_filename)

    pins.parse_pin_details_table(pins_details_table)

    if args.hdr_filename and args.qstr_filename:
        pins.parse_board_table(pins_table)
        pins.update_num_cpu_pins()
        pins.print_const_table()
        pins.print()
        pins.print_header(args.hdr_filename)
        pins.print_qstr(args.qstr_filename)


if __name__ == "__main__":
    main()

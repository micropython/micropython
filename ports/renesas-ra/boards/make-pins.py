#!/usr/bin/env python
"""Creates the pin file for the RAxxx."""

from __future__ import print_function

import argparse
import sys
import csv


class PinAD(object):
    def __init__(self, name, cpu_pin_name, pin_idx, bit, channel):
        self._name = name
        self._cpu_pin_name = cpu_pin_name
        self._pin_idx = pin_idx
        self._bit = bit
        self._channel = channel

    def cpu_pin_name(self):
        return self._cpu_pin_name

    def name(self):
        return self._name

    def bit(self):
        return self._bit

    def channel(self):
        return self._channel

    def print(self):
        print(
            "const pin_ad_obj_t pin_{:s}_ad_obj = PIN_AD({:s}, {:d}, {:d}, {:d});".format(
                self._cpu_pin_name, self._name, self._pin_idx, self._bit, self._channel
            )
        )
        print("")

    def print_header(self, hdr_file):
        n = self.cpu_pin_name()
        hdr_file.write("extern const pin_ad_obj_t pin_{:s}_ad_obj;\n".format(n))
        hdr_file.write("#define pin_{:s}_ad (&pin_{:s}_ad_obj)\n".format(n, n))

    def qstr_list(self):
        return [self._name]


class Pin(object):
    def __init__(self, name, port, bit):
        self._name = name
        self._pin_idx = port * 16 + bit
        self._pin_ad = []
        self._board_pin = False
        # print('// pin_{:s}_obj = PIN({:s}, {:d});'.format(self.name, self.name, self.pin))

    def cpu_pin_name(self):
        return self._name

    def pin_ad(self):
        return self._pin_ad

    def is_board_pin(self):
        return self._board_pin

    def set_is_board_pin(self):
        self._board_pin = True

    def parse_ad(self, ad_str):
        ad_bit = 0
        ad_channel = 0
        if (len(ad_str) == 5) and (ad_str[:3] == "AN0"):
            ad_bit = 12
            ad_channel = int(ad_str[2:])
            self._pin_ad.append(PinAD(ad_str, self._name, self._pin_idx, ad_bit, ad_channel))
        elif (len(ad_str) == 5) and (ad_str[:3] == "AN1"):
            ad_bit = 12
            ad_channel = int(ad_str[2:])
            self._pin_ad.append(PinAD(ad_str, self._name, self._pin_idx, ad_bit, ad_channel))
        elif ad_str[:2] == "AN":
            ad_bit = 8
            ad_channel = int(ad_str[2:4])
            self._pin_ad.append(PinAD(ad_str, self._name, self._pin_idx, ad_bit, ad_channel))

    def print(self):
        pin_ad_name = "NULL"
        for pin_ad in self._pin_ad:
            pin_ad.print()
            pin_ad_name = "pin_{:s}_ad".format(pin_ad.cpu_pin_name())
        print(
            "const machine_pin_obj_t pin_{:s}_obj = PIN({:s}, {:d}, {:s});".format(
                self._name, self._name, self._pin_idx, pin_ad_name
            )
        )
        print("")

    def print_header(self, hdr_file):
        n = self.cpu_pin_name()
        hdr_file.write("extern const machine_pin_obj_t pin_{:s}_obj;\n".format(n))
        hdr_file.write("#define pin_{:s} (&pin_{:s}_obj)\n".format(n, n))

    def qstr_list(self):
        result = []
        for pin_ad in self._pin_ad:
            result += pin_ad.qstr_list()
        # for alt_fn in self.alt_fn:
        #    if alt_fn.is_supported():
        #        result += alt_fn.qstr_list()
        return result


class NamedPin(object):
    def __init__(self, name, pin):
        self._name = name
        self._pin = pin
        # print('// NamedPin {:s}'.format(self._name))

    def pin(self):
        return self._pin

    def name(self):
        return self._name


class Pins(object):
    def __init__(self):
        self.cpu_pins = []  # list of NamedPin objects
        self.board_pins = []  # list of NamedPin objects

    def find_pin(self, cpu_pin_name):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.cpu_pin_name() == cpu_pin_name:
                return pin

    # rx63n_al.csv
    # cpu_pin_name, cpu_pin_port, cpu_pin_bit
    def parse_af_file(self, filename):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    cpu_pin_name = row[0]
                    cpu_pin_port = int(row[1])
                    cpu_pin_bit = int(row[2])
                except:
                    continue
                pin = Pin(cpu_pin_name, cpu_pin_port, cpu_pin_bit)
                self.cpu_pins.append(NamedPin(cpu_pin_name, pin))
                pin.parse_ad(row[3])

    # pins.csv
    # named_pin, cpu_pin_name
    def parse_board_file(self, filename):
        with open(filename, "r") as csvfile:
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
        for named_pin in named_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                print(
                    "  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(&pin_{:s}_obj) }},".format(
                        named_pin.name(), pin.cpu_pin_name()
                    )
                )
        print("};")
        print(
            "MP_DEFINE_CONST_DICT(pin_{:s}_pins_locals_dict, pin_{:s}_pins_locals_dict_table);".format(
                label, label
            )
        )

    def print(self):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.print()
        self.print_named("cpu", self.cpu_pins)
        print("")
        self.print_named("board", self.board_pins)

    def print_header(self, hdr_filename):
        with open(hdr_filename, "wt") as hdr_file:
            for named_pin in self.cpu_pins:
                pin = named_pin.pin()
                if pin.is_board_pin():
                    pin.print_header(hdr_file)
                pin_ads = pin.pin_ad()
                for pin_ad in pin_ads:
                    pin_ad.print_header(hdr_file)
            # provide #define's mapping board to cpu name
            for named_pin in self.board_pins:
                hdr_file.write(
                    "#define pyb_pin_{:s} pin_{:s}\n".format(
                        named_pin.name(), named_pin.pin().cpu_pin_name()
                    )
                )

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
                cond_var = None
                if qstr.startswith("AF"):
                    af_words = qstr.split("_")
                    cond_var = conditional_var(af_words[1])
                    print_conditional_if(cond_var, file=qstr_file)
                print("Q({})".format(qstr), file=qstr_file)
                # print_conditional_endif(cond_var, file=qstr_file)

    def print_ad_hdr(self, ad_const_filename):
        with open(ad_const_filename, "wt") as ad_const_file:
            for named_pin in self.cpu_pins:
                pin = named_pin.pin()
                if pin.is_board_pin():
                    pin_ads = pin.pin_ad()
                    for pin_ad in pin_ads:
                        ad_const_file.write(
                            "  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_INT(GPIO_{:s}) }}, \n".format(
                                pin_ad.name(), pin_ad.name()
                            )
                        )


def main():
    parser = argparse.ArgumentParser(
        prog="make-pins.py",
        usage="%(prog)s [options] [command]",
        description="Generate board specific pin file",
    )
    parser.add_argument(
        "-a",
        "--af",
        dest="af_filename",
        help="Specifies the alternate function file for the chip",
        default="ra4m1_af.csv",
    )
    parser.add_argument(
        "-b", "--board", dest="board_filename", help="Specifies the board file", default="pins.csv"
    )
    parser.add_argument(
        "-p",
        "--prefix",
        dest="prefix_filename",
        help="Specifies beginning portion of generated pins file",
        default="ra4m1_prefix.c",
    )
    parser.add_argument(
        "--ad-const",
        dest="ad_const_filename",
        help="Specifies header file for AD function constants.",
        default="build/pins_ad_const.h",
    )
    parser.add_argument(
        "--af-const",
        dest="af_const_filename",
        help="Specifies header file for alternate function constants.",
        default="build/pins_af_const.h",
    )
    parser.add_argument(
        "--af-py",
        dest="af_py_filename",
        help="Specifies the filename for the python alternate function mappings.",
        default="build/pins_af.py",
    )
    parser.add_argument(
        "--af-defs",
        dest="af_defs_filename",
        help="Specifies the filename for the alternate function defines.",
        default="build/pins_af_defs.h",
    )
    parser.add_argument(
        "-q",
        "--qstr",
        dest="qstr_filename",
        help="Specifies name of generated qstr header file",
        default="build/pins_qstr.h",
    )
    parser.add_argument(
        "-r",
        "--hdr",
        dest="hdr_filename",
        help="Specifies name of generated pin header file",
        default="build/pins.h",
    )
    args = parser.parse_args(sys.argv[1:])

    pins = Pins()

    print("// This file was automatically generated by make-pins.py")
    print("//")
    if args.af_filename:
        print("// --af {:s}".format(args.af_filename))
        pins.parse_af_file(args.af_filename)

    if args.board_filename:
        print("// --board {:s}".format(args.board_filename))
        pins.parse_board_file(args.board_filename)

    if args.prefix_filename:
        print("// --prefix {:s}".format(args.prefix_filename))
        print("")
        with open(args.prefix_filename, "r") as prefix_file:
            print(prefix_file.read())

    pins.print()
    pins.print_header(args.hdr_filename)
    pins.print_qstr(args.qstr_filename)
    pins.print_ad_hdr(args.ad_const_filename)


if __name__ == "__main__":
    main()

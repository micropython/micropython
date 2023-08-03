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

    def print(self, out_source):
        print(
            "const pin_ad_obj_t pin_{:s}_ad_obj = PIN_AD({:s}, {:d}, {:d}, {:d});".format(
                self._cpu_pin_name, self._name, self._pin_idx, self._bit, self._channel
            ),
            file=out_source,
        )
        print("", file=out_source)

    def print_header(self, out_header):
        n = self.cpu_pin_name()
        print("extern const pin_ad_obj_t pin_{:s}_ad_obj;".format(n), file=out_header)
        print("#define pin_{:s}_ad (&pin_{:s}_ad_obj)".format(n, n), file=out_header)


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

    def print(self, out_source):
        pin_ad_name = "NULL"
        for pin_ad in self._pin_ad:
            pin_ad.print(out_source)
            pin_ad_name = "pin_{:s}_ad".format(pin_ad.cpu_pin_name())
        print(
            "const machine_pin_obj_t pin_{:s}_obj = PIN({:s}, {:d}, {:s});".format(
                self._name, self._name, self._pin_idx, pin_ad_name
            ),
            file=out_source,
        )
        print("", file=out_source)

    def print_header(self, out_header):
        n = self.cpu_pin_name()
        print("extern const machine_pin_obj_t pin_{:s}_obj;".format(n), file=out_header)
        print("#define pin_{:s} (&pin_{:s}_obj)".format(n, n), file=out_header)


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
                    cpu_pin_port = int(row[1], 16)
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

    def print_named(self, label, named_pins, out_source):
        print(
            "STATIC const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label),
            file=out_source,
        )
        for named_pin in named_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                print(
                    "  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(&pin_{:s}_obj) }},".format(
                        named_pin.name(), pin.cpu_pin_name()
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
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.print(out_source)
        self.print_named("cpu", self.cpu_pins, out_source)
        print("", file=out_source)
        self.print_named("board", self.board_pins, out_source)

    def print_header(self, out_header):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.print_header(out_header)
            pin_ads = pin.pin_ad()
            for pin_ad in pin_ads:
                pin_ad.print_header(out_header)
        # provide #define's mapping board to cpu name
        for named_pin in self.board_pins:
            print(
                "#define pyb_pin_{:s} pin_{:s}".format(
                    named_pin.name(), named_pin.pin().cpu_pin_name()
                ),
                file=out_header,
            )

    def print_ad_hdr(self, out_ad_const):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin_ads = pin.pin_ad()
                for pin_ad in pin_ads:
                    print(
                        "  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_INT(GPIO_{:s}) }}, \n".format(
                            pin_ad.name(), pin_ad.name()
                        ),
                        file=out_ad_const,
                    )


def main():
    parser = argparse.ArgumentParser(description="Generate board specific pin file")
    parser.add_argument("--board-csv")
    parser.add_argument("--af-csv")
    parser.add_argument("--prefix")
    parser.add_argument("--output-source")
    parser.add_argument("--output-header")
    parser.add_argument("--output-ad-const")
    args = parser.parse_args()

    pins = Pins()

    with open(args.output_source, "w") as out_source:
        print("// This file was automatically generated by make-pins.py", file=out_source)
        print("//", file=out_source)
        if args.af_csv:
            print("// --af {:s}".format(args.af_csv), file=out_source)
            pins.parse_af_file(args.af_csv)

        if args.board_csv:
            print("// --board {:s}".format(args.board_csv), file=out_source)
            pins.parse_board_file(args.board_csv)

        if args.prefix:
            print("// --prefix {:s}".format(args.prefix), file=out_source)
            print("", file=out_source)
            with open(args.prefix, "r") as prefix_file:
                print(prefix_file.read(), file=out_source)

        pins.print(out_source)

    with open(args.output_header, "w") as out_header:
        pins.print_header(out_header)

    with open(args.output_ad_const, "w") as out_ad_const:
        pins.print_ad_hdr(out_ad_const)


if __name__ == "__main__":
    main()

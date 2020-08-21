#!/usr/bin/env python
"""Creates the pin file for the MIMXRT10xx."""

from __future__ import print_function

import argparse
import sys
import csv

SUPPORTED_AFS = {"GPIO", "FLEXSPI"}

def parse_pad(pad_str):
    """Parses a string and returns a (port, gpio_bit) tuple."""
    if len(pad_str) < 4:
        raise ValueError("Expecting pad name to be at least 4 characters")
    if pad_str[:4] != "GPIO":
        raise ValueError("Expecting pad name to start with GPIO")
    return pad_str


class Pin(object):
    """Holds the information associated with a pin."""

    def __init__(self, name, gpio, pin):
        self.gpio = gpio
        self.pin = pin
        self.alt_fn = []
        self.alt_fn_count = 0
        self.adc_channel = 0
        self.board_pin = False

    def set_is_board_pin(self):
        self.board_pin = True

    def is_board_pin(self):
        return self.board_pin

    def parse_adc(self, adc_str):
        pass

    def parse_af(self, af_idx, af_strs_in):
        pass

    def print(self):
        print("machine_pin_obj_t {0} = PIN({0}, {1}, {2}, {3});".format(self.pin, self.gpio, 5, self.pin + "_af"))

    def print_header(self, hdr_file):
        pass


class AlternateFunction(object):
    """Holds the information associated with a pins alternate function."""

    def __init__(self, idx, af_str):
        self.idx = idx
        # Special case. We change I2S2ext_SD into I2S2_EXTSD so that it parses
        # the same way the other peripherals do.
        af_str = af_str.replace("ext_", "_EXT")

        self.af_str = af_str

        self.func = ""
        self.fn_num = None
        self.pin_type = ""
        self.supported = False

        af_words = af_str.split("_", 1)
        self.func, self.fn_num = split_name_num(af_words[0])
        if len(af_words) > 1:
            self.pin_type = af_words[1]
        if self.func in SUPPORTED_FN:
            pin_types = SUPPORTED_FN[self.func]
            if self.pin_type in pin_types:
                self.supported = True

    def is_supported(self):
        return self.supported

    def ptr(self):
        """Returns the numbered function (i.e. USART6) for this AF."""
        if self.fn_num is None:
            return self.func
        return "{:s}{:d}".format(self.func, self.fn_num)

    def mux_name(self):
        return "AF{:d}_{:s}".format(self.idx, self.ptr())

    def print(self):
        """Prints the C representation of this AF."""
        cond_var = None
        if self.supported:
            cond_var = conditional_var("{}{}".format(self.func, self.fn_num))
            print_conditional_if(cond_var)
            print("  AF", end="")
        else:
            print("  //", end="")
        fn_num = self.fn_num
        if fn_num is None:
            fn_num = 0
        print(
            "({:2d}, {:8s}, {:2d}, {:10s}, {:8s}), // {:s}".format(
                self.idx, self.func, fn_num, self.pin_type, self.ptr(), self.af_str
            )
        )
        print_conditional_endif(cond_var)

    def qstr_list(self):
        return [self.mux_name()]



class Pins(object):
    def __init__(self):
        self.board_pins = []

    def find_pin_by_num(self, pin_num):
        for pin in self.board_pins:
            if pin.pin_num == pin_num:
                return pin

    def find_pin_by_name(self, name):
        for pin in self.board_pins:
            if pin.name == name:
                return pin

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

    def parse_af_file(self, filename, pad_col, af_start_col):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                pad = parse_pad(row[pad_col])
                # Parse alternate functions
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
                
                # Pin numbers must start from 0 when used with the TI API
                pin_num = int(row[pin_col]) - 1
                pin = Pin(pad, port_num, gpio_bit, pin_num)
                self.board_pins.append(pin)


    def print_named(self, label, pins):
        print("")
        print(
            "STATIC const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label)
        )
        for pin in pins:
            if pin.board_pin:
                print(
                    "    {{ MP_ROM_QSTR(MP_QSTR_{:6s}), MP_ROM_PTR(&pin_{:6s}) }},".format(
                        pin.name, pin.name
                    )
                )
        print("};")
        print(
            "MP_DEFINE_CONST_DICT(pin_{:s}_pins_locals_dict, pin_{:s}_pins_locals_dict_table);".format(
                label, label
            )
        )

    def print(self):
        for pin in self.board_pins:
            if pin.board_pin:
                pin.print()
        self.print_named("board", self.board_pins)
        print("")


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
        default="mimxrt1021.csv",
    )    
    parser.add_argument(
        "-b", "--board", dest="board_filename", help="Specifies the board file",
    )

    pins = Pins()

    # test code
    args = parser.parse_args(['--board', r'/home/philipp/Projects/micropython/micropython/ports/mimxrt/boards/MIMXRT1020_EVK/pins.csv',
                                "--af", r'/home/philipp/Projects/micropython/micropython/ports/mimxrt/boards/MIMXRT1020_EVK/mimixrt1021_af.csv'])
    #

    if args.af_filename:
        print("// --af {:s}".format(args.af_filename))
        pins.parse_af_file(args.af_filename, 0, 0, 1, 3)

    if args.board_filename:
        print("// --board {:s}".format(args.board_filename))
        pins.parse_board_file(args.board_filename, 1)

    pins.print()


if __name__ == "__main__":
    main()
#!/usr/bin/env python
"""Creates the pin file for the MIMXRT10xx."""

from __future__ import print_function

import argparse
import sys
import csv
import re

SUPPORTED_AFS = {"GPIO", "USDHC", "FLEXPWM", "TMR"}
MAX_AF = 10  # AF0 .. AF9
ADC_COL = 11


regexes = [
    r"IOMUXC_(?P<pin>GPIO_SD_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_AD_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_EMC_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_AD_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_SD_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
]


def parse_pad(pad_str):
    """Parses a string and returns a (port, gpio_bit) tuple."""
    if len(pad_str) < 4:
        raise ValueError("Expecting pad name to be at least 4 characters")
    if pad_str[:4] != "GPIO":
        raise ValueError("Expecting pad name to start with GPIO")
    return pad_str


def af_supported(af_str):
    for supported_af in SUPPORTED_AFS:
        if af_str.startswith(supported_af):
            return True
    else:
        return False


class Pin(object):
    """Holds the information associated with a pin."""

    def __init__(self, pad, gpio, pin, idx=0):
        self.idx = idx
        self.name = pad
        self.pad = pad
        self.gpio = gpio
        self.pin = pin
        self.alt_fn = []
        self.adc_fns = []
        self.board_pin = False

    def set_is_board_pin(self):
        self.board_pin = True

    def is_board_pin(self):
        return self.board_pin

    def parse_adc(self, adc_str):
        adc_regex = r"ADC(?P<instance>\d*)_IN(?P<channel>\d*)"

        matches = re.finditer(adc_regex, adc_str, re.MULTILINE)
        for match in matches:
            self.adc_fns.append(
                AdcFunction(instance=match.group("instance"), channel=match.group("channel"))
            )

    def parse_af(self, af_idx, af_strs_in):
        pass

    def add_af(self, af):
        self.alt_fn.append(af)

    def print_pin_af(self):
        if self.alt_fn:
            print(
                "static const machine_pin_af_obj_t pin_{0}_af[{1}] = {{".format(
                    self.name, len(self.alt_fn)
                )
            )
            for af in self.alt_fn:
                af.print()
            print("};")
        else:
            raise ValueError("Pin '{}' has no alternative functions".format(self.name))

    def print_pin_adc(self):
        if self.adc_fns:
            print(
                "static const machine_pin_adc_obj_t pin_{0}_adc[{1}] = {{".format(
                    self.name, len(self.adc_fns)
                )
            )
            for adc_fn in self.adc_fns:
                adc_fn.print()
            print("};")

    def print(self):
        if self.alt_fn:
            self.print_pin_af()
            self.print_pin_adc()

            if self.adc_fns:
                print(
                    "const machine_pin_obj_t pin_{0} = PIN({0}, {1}, {2}, pin_{0}_af, {3}, pin_{0}_adc);\n".format(
                        self.name, self.gpio, int(self.pin), len(self.adc_fns)
                    )
                )
            else:
                print(
                    "const machine_pin_obj_t pin_{0} = PIN({0}, {1}, {2}, pin_{0}_af, {3}, NULL);\n".format(
                        self.name, self.gpio, int(self.pin), len(self.adc_fns)
                    )
                )
        else:
            raise ValueError("Pin '{}' has no alternative functions".format(self.name))

    def print_header(self, hdr_file):
        pass


class AdcFunction(object):
    """Holds the information associated with a pins ADC function."""

    def __init__(self, instance, channel):
        self.instance = instance
        self.channel = channel

    def print(self):
        """Prints the C representation of this AF."""
        print(f"    PIN_ADC(ADC{self.instance}, {self.channel}),")


class AlternateFunction(object):
    """Holds the information associated with a pins alternate function."""

    def __init__(self, idx, input_reg, input_daisy, af_str):
        self.idx = idx
        self.af_str = af_str
        self.input_reg = input_reg
        self.input_daisy = input_daisy
        self.instance = self.af_str.split("_")[0]

    def print(self):
        """Prints the C representation of this AF."""
        print(
            "    PIN_AF({0}, PIN_AF_MODE_ALT{1}, {2}, {3}, {4}, {5}),".format(
                self.af_str, self.idx, self.input_daisy, self.instance, self.input_reg, "0x10B0U"
            )
        )


class NamedPin(object):
    def __init__(self, name, pad, idx):
        self.name = name
        self.pad = pad
        self.idx = idx


class Pins(object):
    def __init__(self):
        self.cpu_pins = []
        self.board_pins = []

    def find_pin_by_num(self, pin_num):
        for pin in self.cpu_pins:
            if pin.pin_num == pin_num:
                return pin

    def find_pin_by_name(self, pad):
        for pin in self.cpu_pins:
            if pin.pad == pad:
                return pin

    def parse_board_file(self, filename):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                if len(row) == 0 or row[0].startswith("#"):
                    # Skip empty lines, and lines starting with "#"
                    continue
                if len(row) != 2:
                    raise ValueError("Expecting two entries in a row")

                pin = self.find_pin_by_name(row[1])
                if pin and row[0]:  # Only add board pins that have a name
                    self.board_pins.append(NamedPin(row[0], pin.pad, pin.idx))

    def parse_af_file(self, filename, iomux_filename, pad_col, af_start_col):
        af_end_col = af_start_col + MAX_AF

        iomux_pin_config = dict()

        with open(iomux_filename, "r") as ipt:
            input_str = ipt.read()
            for regex in regexes:
                matches = re.finditer(regex, input_str, re.MULTILINE)

                for match in matches:
                    if match.group("pin") not in iomux_pin_config:
                        iomux_pin_config[match.group("pin")] = {
                            int((match.groupdict()["muxMode"].strip("U")), 16): match.groupdict()
                        }
                    else:
                        iomux_pin_config[match.group("pin")][
                            int((match.groupdict()["muxMode"].strip("U")), 16)
                        ] = match.groupdict()

        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            header = next(rows)
            for idx, row in enumerate(rows):
                pad = row[pad_col]
                gpio, pin = row[6].split("_")
                pin_number = pin.lstrip("IO")

                pin = Pin(pad, gpio, pin_number, idx=idx)

                # Parse alternate functions
                af_idx = 0
                for af_idx, af in enumerate(row[af_start_col:af_end_col]):
                    if af and af_supported(af):
                        pin.add_af(
                            AlternateFunction(
                                af_idx,
                                iomux_pin_config[pin.name][af_idx]["inputRegister"].strip("U"),
                                int(
                                    iomux_pin_config[pin.name][af_idx]["inputDaisy"].strip("U"), 16
                                ),
                                af,
                            )
                        )

                pin.parse_adc(row[ADC_COL])

                self.cpu_pins.append(pin)

    @staticmethod
    def print_named(label, pins):
        print("")
        print(
            "STATIC const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label)
        )
        for pin in pins:
            print(
                "    {{ MP_ROM_QSTR(MP_QSTR_{}), MP_ROM_PTR(&pin_{}) }},".format(pin.name, pin.pad)
            )
        print("};")
        print(
            "MP_DEFINE_CONST_DICT(machine_pin_{:s}_pins_locals_dict, pin_{:s}_pins_locals_dict_table);".format(
                label, label
            )
        )

    def print(self):
        # Print Pin Object declarations
        for pin in self.cpu_pins:
            pin.print()

        print("")
        print("const machine_pin_obj_t* machine_pin_board_pins [] = {")
        for pin in self.board_pins:
            print("    &pin_{},".format(pin.pad))
        print("};")
        print("const uint32_t num_board_pins = {:d};".format(len(self.board_pins)))
        # Print Pin mapping dictionaries
        self.print_named("cpu", self.cpu_pins)
        self.print_named("board", self.board_pins)
        print("")

    def print_header(self, hdr_filename):
        with open(hdr_filename, "w") as hdr_file:
            for pin in self.cpu_pins:
                hdr_file.write("extern const machine_pin_obj_t pin_{};\n".format(pin.name))
            hdr_file.write("extern const machine_pin_obj_t* machine_pin_board_pins[];\n")
            hdr_file.write("extern const uint32_t num_board_pins;\n")
            hdr_file.write("extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;\n")
            hdr_file.write("extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;\n")

            hdr_file.write("\n// Defines\n")
            module_instance_factory(self.cpu_pins, hdr_file, "USDHC")
            module_instance_factory(self.cpu_pins, hdr_file, "FLEXPWM")
            module_instance_factory(self.cpu_pins, hdr_file, "TMR")


def module_instance_factory(pins, output_file, name):
    module_pin = filter(lambda p: any([af for af in p.alt_fn if name in af.af_str]), pins)

    module_instances = dict()
    for pin in module_pin:
        for idx, alt_fn in enumerate(pin.alt_fn):
            if name in alt_fn.instance:
                format_string = "#define {0}_{1} &pin_{0}, {2}"
                if alt_fn.instance not in module_instances:
                    module_instances[alt_fn.instance] = [
                        format_string.format(pin.name, alt_fn.af_str, idx)
                    ]
                else:
                    module_instances[alt_fn.instance].append(
                        format_string.format(pin.name, alt_fn.af_str, idx)
                    )

    for k, v in module_instances.items():
        output_file.write(f"// {k}\n")
        output_file.write(f"#define {k}_AVAIL (1)\n")
        if name == "FLEXPWM":
            output_file.write(f"#define {k} {k[-4:]}\n")
        for i in v:
            output_file.write(i + "\n")


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
        default="mimxrt1021_af.csv",
    )
    parser.add_argument(
        "-i",
        "--iomux",
        dest="iomux_filename",
        help="Specifies the fsl_iomuxc.h file for the chip",
        default="fsl_iomuxc.h",
    )
    parser.add_argument(
        "-b",
        "--board",
        dest="board_filename",
        help="Specifies the board file",
        default="MIMXRT1020_EVK/pins.csv",
    )
    parser.add_argument(
        "-p",
        "--prefix",
        dest="prefix_filename",
        help="Specifies beginning portion of generated pins file",
        default="mimxrt_prefix.c",
    )
    parser.add_argument(
        "-r",
        "--hdr",
        dest="hdr_filename",
        help="Specifies name of generated pin header file",
        default="build/pins.h",
    )

    pins = Pins()

    # test code
    args = parser.parse_args()
    #

    if args.af_filename:
        print("// --af {:s}".format(args.af_filename))
        pins.parse_af_file(args.af_filename, args.iomux_filename, 0, 1)

    if args.board_filename:
        print("// --board {:s}".format(args.board_filename))
        pins.parse_board_file(args.board_filename)

    if args.hdr_filename:
        print("// --hdr {:s}".format(args.hdr_filename))

    if args.prefix_filename:
        print("// --prefix {:s}".format(args.prefix_filename))
        with open(args.prefix_filename, "r") as prefix_file:
            print(prefix_file.read())

    pins.print()
    pins.print_header(args.hdr_filename)


if __name__ == "__main__":
    main()

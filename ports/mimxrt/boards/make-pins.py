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
    r"IOMUXC_(?P<pin>GPIO_EMC_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_DISP_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_LPSR_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_[SNVS_]*(?P<pin>WAKEUP[_DIG]*)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_[SNVS_]*(?P<pin>PMIC_ON_REQ[_DIG]*)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_[SNVS_]*(?P<pin>PMIC_STBY_REQ[_DIG]*)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_SNVS_\d\d_DIG)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
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
        lpadc_regex = r"ADC(?P<instance>\d*)_CH(?P<channel>\d*)"  # LPADC for MIMXRT11xx chips

        matches = re.finditer(adc_regex, adc_str, re.MULTILINE)
        for match in matches:
            self.adc_fns.append(
                AdcFunction(instance=match.group("instance"), channel=match.group("channel"))
            )

        matches = re.finditer(lpadc_regex, adc_str, re.MULTILINE)
        for match in matches:
            self.adc_fns.append(
                AdcFunction(
                    peripheral="LPADC",
                    instance=match.group("instance"),
                    channel=match.group("channel"),
                )
            )

    def parse_af(self, af_idx, af_strs_in):
        pass

    def add_af(self, af):
        self.alt_fn.append(af)

    def print_pin_af(self, out_source):
        if self.alt_fn:
            print(
                "static const machine_pin_af_obj_t pin_{0}_af[{1}] = {{".format(
                    self.name, len(self.alt_fn)
                ),
                file=out_source,
            )
            for af in self.alt_fn:
                af.print(out_source)
            print("};", file=out_source)
        else:
            raise ValueError("Pin '{}' has no alternative functions".format(self.name))

    def print_pin_adc(self, out_source):
        if self.adc_fns:
            print(
                "static const machine_pin_adc_obj_t pin_{0}_adc[{1}] = {{".format(
                    self.name, len(self.adc_fns)
                ),
                file=out_source,
            )
            for adc_fn in self.adc_fns:
                adc_fn.print(out_source)
            print("};", file=out_source)

    def print(self, out_source):
        if self.alt_fn:
            self.print_pin_af(out_source)
            self.print_pin_adc(out_source)

            options = {
                "GPIO_LPSR_00": "PIN_LPSR",
                "GPIO_LPSR_01": "PIN_LPSR",
                "GPIO_LPSR_02": "PIN_LPSR",
                "GPIO_LPSR_03": "PIN_LPSR",
                "GPIO_LPSR_04": "PIN_LPSR",
                "GPIO_LPSR_05": "PIN_LPSR",
                "GPIO_LPSR_06": "PIN_LPSR",
                "GPIO_LPSR_07": "PIN_LPSR",
                "GPIO_LPSR_08": "PIN_LPSR",
                "GPIO_LPSR_09": "PIN_LPSR",
                "GPIO_LPSR_10": "PIN_LPSR",
                "GPIO_LPSR_11": "PIN_LPSR",
                "GPIO_LPSR_12": "PIN_LPSR",
                "GPIO_LPSR_13": "PIN_LPSR",
                "GPIO_LPSR_14": "PIN_LPSR",
                "GPIO_LPSR_15": "PIN_LPSR",
                "GPIO_SNVS_00_DIG": "PIN_SNVS",
                "GPIO_SNVS_01_DIG": "PIN_SNVS",
                "GPIO_SNVS_02_DIG": "PIN_SNVS",
                "GPIO_SNVS_03_DIG": "PIN_SNVS",
                "GPIO_SNVS_04_DIG": "PIN_SNVS",
                "GPIO_SNVS_05_DIG": "PIN_SNVS",
                "GPIO_SNVS_06_DIG": "PIN_SNVS",
                "GPIO_SNVS_07_DIG": "PIN_SNVS",
                "GPIO_SNVS_08_DIG": "PIN_SNVS",
                "GPIO_SNVS_09_DIG": "PIN_SNVS",
                "WAKEUP": "PIN_SNVS",
                "WAKEUP_DIG": "PIN_SNVS",
                "PMIC_ON_REQ": "PIN_SNVS",
                "PMIC_ON_REQ_DIG": "PIN_SNVS",
                "PMIC_STBY_REQ": "PIN_SNVS",
                "PMIC_STBY_REQ_DIG": "PIN_SNVS",
            }

            print(
                "const machine_pin_obj_t pin_{0} = {1}({0}, {2}, {3}, pin_{0}_af, {4}, {5});\n".format(
                    self.name,
                    options.get(self.name, "PIN"),
                    self.gpio,
                    int(self.pin),
                    len(self.adc_fns),
                    "pin_{}_adc".format(self.name) if self.adc_fns else "NULL",
                ),
                file=out_source,
            )
        else:
            raise ValueError("Pin '{}' has no alternative functions".format(self.name))

    def print_header(self, out_header):
        pass


class AdcFunction(object):
    """Holds the information associated with a pins ADC function."""

    def __init__(self, instance, channel, peripheral="ADC"):
        self.peripheral = peripheral
        self.instance = instance
        self.channel = channel

    def print(self, out_source):
        """Prints the C representation of this AF."""
        print(f"    PIN_ADC({self.peripheral}{self.instance}, {self.channel}),", file=out_source)


class AlternateFunction(object):
    """Holds the information associated with a pins alternate function."""

    def __init__(self, idx, input_reg, input_daisy, af_str):
        self.idx = idx
        self.af_str = af_str
        self.input_reg = input_reg
        self.input_daisy = input_daisy
        self.instance = self.af_str.split("_")[0]

    def print(self, out_source):
        """Prints the C representation of this AF."""
        print(
            "    PIN_AF({0}, PIN_AF_MODE_ALT{1}, {2}, {3}, {4}, {5}),".format(
                self.af_str, self.idx, self.input_daisy, self.instance, self.input_reg, "0x10B0U"
            ),
            file=out_source,
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

    def parse_af_file(self, filename, iomux_filename):
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
            # Extract indexes from header row
            pad_col = header.index("Pad")
            adc_col = header.index("ADC")
            #
            for idx, row in enumerate(rows):
                pad = row[pad_col]
                gpio, pin = row[6].split("_")
                pin_number = pin.lstrip("IO")
                pin = Pin(pad, gpio, pin_number, idx=idx)

                # Parse alternate functions
                af_idx = 0
                for af_idx, af in enumerate(row[(pad_col + 1) : adc_col]):
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

                pin.parse_adc(row[adc_col])

                self.cpu_pins.append(pin)

    @staticmethod
    def print_named(label, pins, out_source):
        print("", file=out_source)
        print(
            "STATIC const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label),
            file=out_source,
        )
        for pin in pins:
            (
                print(
                    "    {{ MP_ROM_QSTR(MP_QSTR_{}), MP_ROM_PTR(&pin_{}) }},".format(
                        pin.name, pin.pad
                    ),
                    file=out_source,
                ),
            )
        print("};", file=out_source)
        print(
            "MP_DEFINE_CONST_DICT(machine_pin_{:s}_pins_locals_dict, pin_{:s}_pins_locals_dict_table);".format(
                label, label
            ),
            file=out_source,
        )

    def print(self, out_source):
        # Print Pin Object declarations
        for pin in self.cpu_pins:
            pin.print(out_source)

        print("", file=out_source)
        print("const machine_pin_obj_t* machine_pin_board_pins [] = {", file=out_source)
        for pin in self.board_pins:
            print("    &pin_{},".format(pin.pad), file=out_source)
        print("};", file=out_source)
        print(
            "const uint32_t num_board_pins = {:d};".format(len(self.board_pins)), file=out_source
        )
        # Print Pin mapping dictionaries
        self.print_named("cpu", self.cpu_pins, out_source)
        self.print_named("board", self.board_pins, out_source)
        print("", file=out_source)

    def print_header(self, out_header):
        for pin in self.cpu_pins:
            print("extern const machine_pin_obj_t pin_{};".format(pin.name), file=out_header)
        print("extern const machine_pin_obj_t* machine_pin_board_pins[];", file=out_header)
        print("extern const uint32_t num_board_pins;", file=out_header)
        print("extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;", file=out_header)
        print("extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;", file=out_header)
        print("", file=out_header)
        print("// Defines", file=out_header)
        module_instance_factory(self.cpu_pins, out_header, "USDHC")
        module_instance_factory(self.cpu_pins, out_header, "FLEXPWM")
        module_instance_factory(self.cpu_pins, out_header, "TMR")


def module_instance_factory(pins, out_header, name):
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
        print(f"// {k}", file=out_header)
        print(f"#define {k}_AVAIL (1)", file=out_header)
        if name == "FLEXPWM":
            print(f"#define {k} {k[-4:]}", file=out_header)
        for i in v:
            print(i, file=out_header)


def main():
    parser = argparse.ArgumentParser(description="Generate board specific pin file")
    parser.add_argument("--board-csv")
    parser.add_argument("--af-csv")
    parser.add_argument("--prefix")
    parser.add_argument("--iomux-header")
    parser.add_argument("--output-source")
    parser.add_argument("--output-header")
    args = parser.parse_args()

    pins = Pins()

    with open(args.output_source, "w") as out_source:
        if args.af_csv:
            print("// --af {:s}".format(args.af_csv), file=out_source)
            pins.parse_af_file(args.af_csv, args.iomux_header)

        if args.board_csv:
            print("// --board {:s}".format(args.board_csv), file=out_source)
            pins.parse_board_file(args.board_csv)

        if args.output_header:
            print("// --hdr {:s}".format(args.output_header), file=out_source)

        if args.prefix:
            print("// --prefix {:s}".format(args.prefix), file=out_source)
            with open(args.prefix, "r") as prefix_file:
                print(prefix_file.read(), file=out_source)

        pins.print(out_source)

    with open(args.output_header, "w") as out_header:
        pins.print_header(out_header)


if __name__ == "__main__":
    main()

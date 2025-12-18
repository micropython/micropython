#!/usr/bin/env python
"""Creates the pin file for the nRF5."""

# Do not use this as a reference for new ports. See tools/boardgen.py and e.g.
# ports/stm32/boards/make-pins.py.

import argparse
import sys
import csv

SUPPORTED_FN = {"UART": ["RX", "TX", "CTS", "RTS"]}


def parse_pin(name_str):
    """Parses a string and returns a pin-num."""
    if len(name_str) < 1:
        raise ValueError("Expecting pin name to be at least 4 characters.")
    if name_str[0] != "P":
        raise ValueError("Expecting pin name to start with P")
    pin_str = name_str[1:].split("/")[0]
    if not pin_str.isdigit():
        raise ValueError("Expecting numeric pin number.")
    return int(pin_str)


def split_name_num(name_num):
    num = None
    for num_idx in range(len(name_num) - 1, -1, -1):
        if not name_num[num_idx].isdigit():
            name = name_num[0 : num_idx + 1]
            num_str = name_num[num_idx + 1 :]
            if len(num_str) > 0:
                num = int(num_str)
            break
    return name, num


class AlternateFunction(object):
    """Holds the information associated with a pins alternate function."""

    def __init__(self, idx, af_str):
        self.idx = idx
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

    def print(self, out_source):
        """Prints the C representation of this AF."""
        if self.supported:
            print("  AF", end="", file=out_source)
        else:
            print("  //", end="", file=out_source)
        fn_num = self.fn_num
        if fn_num is None:
            fn_num = 0
        print(
            "({:2d}, {:8s}, {:2d}, {:10s}, {:8s}), // {:s}".format(
                self.idx, self.func, fn_num, self.pin_type, self.ptr(), self.af_str
            ),
            file=out_source,
        )


class Pin(object):
    """Holds the information associated with a pin."""

    def __init__(self, pin):
        self.pin = pin
        self.alt_fn = []
        self.alt_fn_count = 0
        self.adc_num = 0
        self.adc_channel = 0
        self.board_pin = False
        self.board_index = None

    def cpu_pin_name(self):
        return "{:s}{:d}".format("P", self.pin)

    def is_board_pin(self):
        return self.board_pin

    def set_is_board_pin(self):
        self.board_pin = True

    def set_board_index(self, index):
        self.board_index = index

    def parse_adc(self, adc_str):
        if adc_str[:3] != "ADC":
            return
        (adc, channel) = adc_str.split("_")
        for idx in range(3, len(adc)):
            self.adc_num = int(adc[idx])
        self.adc_channel = int(channel[2:])

    def parse_af(self, af_idx, af_strs_in):
        if len(af_strs_in) == 0:
            return
        # If there is a slash, then the slash separates 2 aliases for the
        # same alternate function.
        af_strs = af_strs_in.split("/")
        for af_str in af_strs:
            alt_fn = AlternateFunction(af_idx, af_str)
            self.alt_fn.append(alt_fn)
            if alt_fn.is_supported():
                self.alt_fn_count += 1

    def alt_fn_name(self, null_if_0=False):
        if null_if_0 and self.alt_fn_count == 0:
            return "NULL"
        return "pin_{:s}_af".format(self.cpu_pin_name())

    def adc_num_str(self):
        str = ""
        for adc_num in range(1, 4):
            if self.adc_num & (1 << (adc_num - 1)):
                if len(str) > 0:
                    str += " | "
                str += "PIN_ADC"
                str += chr(ord("0") + adc_num)
        if len(str) == 0:
            str = "0"
        return str

    def print_const_table_entry(self, out_source):
        print(
            "  PIN({:d}, {:s}, {:s}, {:d}),".format(
                self.pin, self.alt_fn_name(null_if_0=True), self.adc_num_str(), self.adc_channel
            ),
            file=out_source,
        )

    def print(self, out_source):
        if self.alt_fn_count == 0:
            print("// ", end="", file=out_source)
        print("const pin_af_obj_t {:s}[] = {{".format(self.alt_fn_name()), file=out_source)
        for alt_fn in self.alt_fn:
            alt_fn.print(out_source)
        if self.alt_fn_count == 0:
            print("// ", end="", file=out_source)
        print("};", file=out_source)
        print("", file=out_source)
        print(
            "const pin_obj_t pin_{:s} = PIN({:d}, {:s}, {:s}, {:d});".format(
                self.cpu_pin_name(),
                self.pin,
                self.alt_fn_name(null_if_0=True),
                self.adc_num_str(),
                self.adc_channel,
            ),
            file=out_source,
        )
        print("", file=out_source)

    def print_header(self, out_header):
        print("extern const pin_obj_t pin_{:s};".format(self.cpu_pin_name()), file=out_header)
        if self.alt_fn_count > 0:
            print(
                "extern const pin_af_obj_t pin_{:s}_af[];\n".format(self.cpu_pin_name()),
                file=out_header,
            )


class NamedPin(object):
    def __init__(self, name, pin):
        self._name = name
        self._pin = pin

    def pin(self):
        return self._pin

    def name(self):
        return self._name


class Pins(object):
    def __init__(self):
        self.cpu_pins = []  # list of NamedPin objects
        self.board_pins = []  # list of NamedPin objects

    def find_pin(self, pin_num):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.pin == pin_num:
                return pin

    def parse_af_file(self, filename, pinname_col, af_col, af_col_end):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    pin_num = parse_pin(row[pinname_col])
                except:
                    continue
                pin = Pin(pin_num)
                for af_idx in range(af_col, len(row)):
                    if af_idx < af_col_end:
                        pin.parse_af(af_idx - af_col, row[af_idx])
                    elif af_idx == af_col_end:
                        pin.parse_adc(row[af_idx])
                self.cpu_pins.append(NamedPin(pin.cpu_pin_name(), pin))

    def parse_board_file(self, filename):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    pin_num = parse_pin(row[1])
                except:
                    continue
                pin = self.find_pin(pin_num)
                if pin:
                    pin.set_is_board_pin()
                    self.board_pins.append(NamedPin(row[0], pin))

    def print_named(self, label, named_pins, out_source):
        print(
            "static const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label),
            file=out_source,
        )
        for named_pin in named_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                print(
                    "  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(&machine_board_pin_obj[{:d}]) }},".format(
                        named_pin.name(), pin.board_index
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

    def print_const_table(self, out_source):
        num_board_pins = 0
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.set_board_index(num_board_pins)
                num_board_pins += 1
        print("", file=out_source)
        print(
            "const uint8_t machine_pin_num_of_board_pins = {:d};".format(num_board_pins),
            file=out_source,
        )
        print("", file=out_source)
        print(
            "const pin_obj_t machine_board_pin_obj[{:d}] = {{".format(num_board_pins),
            file=out_source,
        )
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.print_const_table_entry(out_source)
        print("};", file=out_source)

    def print(self, out_source):
        self.print_named("cpu", self.cpu_pins, out_source)
        print("", file=out_source)
        self.print_named("board", self.board_pins, out_source)

    def print_header(self, out_header):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.print_header(out_header)
        print("extern const pin_obj_t * const pin_adc1[];", file=out_header)
        print("extern const pin_obj_t * const pin_adc2[];", file=out_header)
        print("extern const pin_obj_t * const pin_adc3[];", file=out_header)

    def print_af_hdr(self, out_af_const):
        af_hdr_set = set([])
        mux_name_width = 0
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                for af in pin.alt_fn:
                    if af.is_supported():
                        mux_name = af.mux_name()
                        af_hdr_set |= set([mux_name])
                        if len(mux_name) > mux_name_width:
                            mux_name_width = len(mux_name)
        for mux_name in sorted(af_hdr_set):
            key = "MP_ROM_QSTR(MP_QSTR_{}),".format(mux_name)
            val = "MP_ROM_INT(GPIO_{})".format(mux_name)
            print("    { %-*s %s }," % (mux_name_width + 26, key, val), file=out_af_const)


def main():
    parser = argparse.ArgumentParser(description="Generate board specific pin file")
    parser.add_argument("--board-csv")
    parser.add_argument("--af-csv")
    parser.add_argument("--prefix")
    parser.add_argument("--output-source")
    parser.add_argument("--output-header")
    parser.add_argument("--output-af-const")
    args = parser.parse_args()

    pins = Pins()

    with open(args.output_source, "w") as out_source:
        print("// This file was automatically generated by make-pins.py", file=out_source)
        print("//", file=out_source)
        if args.af_csv:
            print("// --af {:s}".format(args.af_csv), file=out_source)
            pins.parse_af_file(args.af_csv, 1, 2, 2)

        if args.board_csv:
            print("// --board {:s}".format(args.board_csv), file=out_source)
            pins.parse_board_file(args.board_csv)

        if args.prefix:
            print("// --prefix {:s}".format(args.prefix), file=out_source)
            print("", file=out_source)
            with open(args.prefix, "r") as prefix_file:
                print(prefix_file.read(), file=out_source)

        pins.print_const_table(out_source)
        pins.print(out_source)

    with open(args.output_header, "w") as out_header:
        pins.print_header(out_header)

    with open(args.output_af_const, "w") as out_af_const:
        pins.print_af_hdr(out_af_const)


if __name__ == "__main__":
    main()

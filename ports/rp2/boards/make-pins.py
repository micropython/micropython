#!/usr/bin/env python
"""Creates the pin file for the RP2."""

from __future__ import print_function

import argparse
import sys
import csv
import re

SUPPORTED_FN = {
    "SPI": ["TX", "RX", "SCK", "CS"],
    "UART": ["TX", "RX", "CTS", "RTS"],
    "I2C": ["SCL", "SDA"],
    "PWM": ["A", "B"],
    "SIO": [""],
    "PIO0": [""],
    "PIO1": [""],
    "GPCK": ["GPIN0", "GPOUT0", "GPIN1", "GPOUT1", "GPOUT2", "GPOUT3"],
    "USB": ["OVCUR_DET", "VBUS_DET", "VBUS_EN"],
}


def parse_pin(name_str):
    """Parses a string and returns a pin number."""
    if len(name_str) < 2:
        raise ValueError("Expecting pin name to be at least 2 characters.")
    if not name_str.startswith("GPIO") and not name_str.startswith("EXT_GPIO"):
        raise ValueError("Expecting pin name to start with EXT_/GPIO")
    return int(re.findall(r"\d+$", name_str)[0])


def split_name_num(name_num):
    num = None
    for num_idx in range(len(name_num) - 1, -1, -1):
        if not name_num[num_idx].isdigit():
            name = name_num[0 : num_idx + 1]
            num_str = name_num[num_idx + 1 :]
            if len(num_str) > 0:
                num = int(num_str)
            break
    if name == "PIO":
        name += str(num)
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

    def print(self):
        """Prints the C representation of this AF."""
        if self.supported:
            print("  AF", end="")
        else:
            print("  //", end="")
        fn_num = self.fn_num
        if fn_num is None:
            fn_num = 0
        print("({:d}, {:4s}, {:d}), // {:s}".format(self.idx, self.func, fn_num, self.af_str))

    def qstr_list(self):
        return [self.mux_name()]


class Pin(object):
    """Holds the information associated with a pin."""

    def __init__(self, pin, is_ext=False):
        self.pin = pin
        self.alt_fn = []
        self.alt_fn_count = 0
        self.is_board = False
        self.is_ext = is_ext

    def cpu_pin_name(self):
        return "{:s}GPIO{:d}".format("EXT_" if self.is_ext else "", self.pin)

    def is_board_pin(self):
        return self.is_board

    def set_is_board_pin(self):
        self.is_board = True

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

    def print(self):
        if self.is_ext:
            print("#if (MICROPY_HW_PIN_EXT_COUNT > {:d})".format(self.pin))

        if self.alt_fn_count == 0:
            print("// ", end="")
        print("const machine_pin_af_obj_t {:s}[] = {{".format(self.alt_fn_name()))
        for alt_fn in self.alt_fn:
            alt_fn.print()
        if self.alt_fn_count == 0:
            print("// ", end="")
        print("};")
        print("")
        print(
            "{:s}machine_pin_obj_t pin_{:s} = PIN({:d}, {:s}, {:d}, {:d}, {:s});".format(
                "" if self.is_ext else "const ",
                self.cpu_pin_name(),
                self.pin,
                self.cpu_pin_name(),
                self.is_ext,
                self.alt_fn_count,
                self.alt_fn_name(null_if_0=True),
            )
        )
        if self.is_ext:
            print("#endif")
        print("")

    def print_header(self, hdr_file):
        n = self.cpu_pin_name()
        hdr_file.write(
            "extern{:s}machine_pin_obj_t pin_{:s};\n".format(" " if self.is_ext else " const ", n)
        )
        if self.alt_fn_count > 0:
            hdr_file.write("extern const machine_pin_af_obj_t pin_{:s}_af[];\n".format(n))

    def qstr_list(self):
        result = []
        for alt_fn in self.alt_fn:
            if alt_fn.is_supported():
                result += alt_fn.qstr_list()
        return result


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
        self.ext_pins = []  # list of NamedPin objects
        for i in range(0, 10):
            self.ext_pins.append(NamedPin("EXT_GPIO{:d}".format(i), Pin(i, True)))

    def find_pin(self, pin_name):
        for pin in self.cpu_pins:
            if pin.name() == pin_name:
                return pin.pin()

        for pin in self.ext_pins:
            if pin.name() == pin_name:
                return pin.pin()

    def parse_af_file(self, filename, pinname_col, af_col):
        with open(filename, "r") as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    pin_num = parse_pin(row[pinname_col])
                except Exception:
                    # import traceback; traceback.print_exc()
                    continue
                pin = Pin(pin_num)
                for af_idx in range(af_col, len(row)):
                    if af_idx >= af_col:
                        pin.parse_af(af_idx, row[af_idx])
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
                try:
                    parse_pin(cpu_pin_name)
                except:
                    # import traceback; traceback.print_exc()
                    continue
                pin = self.find_pin(cpu_pin_name)
                if pin:
                    pin.set_is_board_pin()
                    if row[0]:  # Only add board pins that have a name
                        self.board_pins.append(NamedPin(row[0], pin))

    def print_table(self, label, named_pins):
        print("")
        print("const machine_pin_obj_t *machine_pin_{:s}_pins[] = {{".format(label))
        for pin in named_pins:
            if not pin.pin().is_ext:
                print("    &pin_{},".format(pin.name()))
        print("};")
        print("")

    def print_named(self, label, named_pins):
        print("")
        print(
            "STATIC const mp_rom_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{".format(label)
        )
        for named_pin in named_pins:
            pin = named_pin.pin()
            if pin.is_ext:
                print("  #if (MICROPY_HW_PIN_EXT_COUNT > {:d})".format(pin.pin))
            print(
                "  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(&pin_{:s}) }},".format(
                    named_pin.name(), pin.cpu_pin_name()
                )
            )
            if pin.is_ext:
                print("  #endif")

        print("};")
        print(
            "MP_DEFINE_CONST_DICT(pin_{:s}_pins_locals_dict, pin_{:s}_pins_locals_dict_table);".format(
                label, label
            )
        )
        print("")

    def print(self):
        for pin in self.cpu_pins:
            pin.pin().print()

        for pin in self.ext_pins:
            if pin.pin().is_ext:
                pin.pin().print()

        self.print_table("cpu", self.cpu_pins)
        self.print_named("cpu", self.cpu_pins + self.ext_pins)
        self.print_named("board", self.board_pins)

    def print_header(self, hdr_filename, obj_decls):
        with open(hdr_filename, "wt") as hdr_file:
            if obj_decls:
                for named_pin in self.cpu_pins:
                    pin = named_pin.pin()
                    pin.print_header(hdr_file)
                for named_pin in self.board_pins:
                    pin = named_pin.pin()
                    if pin.is_ext:
                        pin.print_header(hdr_file)
            # provide #define's mapping board to cpu name
            for named_pin in self.board_pins:
                if named_pin.pin().is_board_pin():
                    hdr_file.write(
                        "#define pin_{:s} pin_{:s}\n".format(
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
                print("Q({})".format(qstr), file=qstr_file)

    def print_af_hdr(self, af_const_filename):
        with open(af_const_filename, "wt") as af_const_file:
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
                key = "MP_OBJ_NEW_QSTR(MP_QSTR_{}),".format(mux_name)
                val = "MP_OBJ_NEW_SMALL_INT(GPIO_{})".format(mux_name)
                print("    { %-*s %s }," % (mux_name_width + 26, key, val), file=af_const_file)

    def print_af_py(self, af_py_filename):
        with open(af_py_filename, "wt") as af_py_file:
            print("PINS_AF = (", file=af_py_file)
            for named_pin in self.board_pins:
                print("  ('%s', " % named_pin.name(), end="", file=af_py_file)
                for af in named_pin.pin().alt_fn:
                    if af.is_supported():
                        print("(%d, '%s'), " % (af.idx, af.af_str), end="", file=af_py_file)
                print("),", file=af_py_file)
            print(")", file=af_py_file)


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
        default="rp2_af.csv",
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
        "-b",
        "--board",
        dest="board_filename",
        help="Specifies the board file",
    )
    parser.add_argument(
        "-p",
        "--prefix",
        dest="prefix_filename",
        help="Specifies beginning portion of generated pins file",
        default="rp2_prefix.c",
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
        pins.parse_af_file(args.af_filename, 0, 1)

    if args.board_filename:
        print("// --board {:s}".format(args.board_filename))
        pins.parse_board_file(args.board_filename)

    if args.prefix_filename:
        print("// --prefix {:s}".format(args.prefix_filename))
        print("")
        with open(args.prefix_filename, "r") as prefix_file:
            print(prefix_file.read())
    pins.print()
    pins.print_header(args.hdr_filename, True)
    pins.print_qstr(args.qstr_filename)
    pins.print_af_hdr(args.af_const_filename)
    pins.print_af_py(args.af_py_filename)


if __name__ == "__main__":
    main()

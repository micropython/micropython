#!/usr/bin/env python
"""Creates the pin file for the nRF5."""

from __future__ import print_function

import argparse
import sys
import csv


def parse_port_pin(name_str):
    """Parses a string and returns a (port-num, pin-num) tuple."""
    if len(name_str) < 4:
        raise ValueError("Expecting pin name to be at least 5 charcters.")
    if name_str[0] != 'P':
        raise ValueError("Expecting pin name to start with P")
    if name_str[1] not in ('0', '1'):
        raise ValueError("Expecting pin port to be in 0 or 1")
    port = ord(name_str[1]) - ord('0')
    pin_str = name_str[3:]
    if not pin_str.isdigit():
        raise ValueError("Expecting numeric pin number.")
    return (port, int(pin_str))


class Pin(object):
    """Holds the information associated with a pin."""

    def __init__(self, port, pin):
        self.port = port
        self.pin = pin
        self.adc_channel = '0'
        self.board_pin = False

    def cpu_pin_name(self):
        return 'P{:d}_{:02d}'.format(self.port, self.pin)

    def is_board_pin(self):
        return self.board_pin

    def set_is_board_pin(self):
        self.board_pin = True

    def parse_adc(self, adc_str):
        if (adc_str[:3] != 'AIN'):
            return
        self.adc_channel = 'SAADC_CH_PSELP_PSELP_AnalogInput%d' % int(adc_str[3])

    def print(self):
        print('const pin_obj_t pin_{:s} = PIN({:s}, {:d}, {:d}, {:s});'.format(
            self.cpu_pin_name(), self.cpu_pin_name(),
            self.port, self.pin, self.adc_channel))

    def print_header(self, hdr_file):
        hdr_file.write('extern const pin_obj_t pin_{:s};\n'.
                       format(self.cpu_pin_name()))


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
        self.cpu_pins = []    # list of NamedPin objects
        self.board_pins = []  # list of NamedPin objects

    def find_pin(self, port_num, pin_num):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.port == port_num and pin.pin == pin_num:
                return pin

    def parse_af_file(self, filename):
        with open(filename, 'r') as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    (port_num, pin_num) = parse_port_pin(row[0])
                except:
                    continue
                pin = Pin(port_num, pin_num)
                if len(row) > 1:
                    pin.parse_adc(row[1])
                self.cpu_pins.append(NamedPin(pin.cpu_pin_name(), pin))

    def parse_board_file(self, filename):
        with open(filename, 'r') as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    (port_num, pin_num) = parse_port_pin(row[1])
                except:
                    continue
                pin = self.find_pin(port_num, pin_num)
                if pin:
                    pin.set_is_board_pin()
                    self.board_pins.append(NamedPin(row[0], pin))

    def print_named(self, label, named_pins):
        print('')
        print('STATIC const mp_rom_map_elem_t {:s}_table[] = {{'.format(label))
        for named_pin in named_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                print('  {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_PTR(&pin_{:s}) }},'.format(named_pin.name(),  pin.cpu_pin_name()))
        print('};')
        print('MP_DEFINE_CONST_DICT({:s}, {:s}_table);'.format(label, label))

    def print(self):
        for named_pin in self.cpu_pins:
            pin = named_pin.pin()
            if pin.is_board_pin():
                pin.print()
        self.print_named('mcu_pin_globals', self.cpu_pins)
        self.print_named('board_module_globals', self.board_pins)

    def print_header(self, hdr_filename):
        with open(hdr_filename, 'wt') as hdr_file:
            for named_pin in self.cpu_pins:
                pin = named_pin.pin()
                if pin.is_board_pin():
                    pin.print_header(hdr_file)

    def print_qstr(self, qstr_filename):
        with open(qstr_filename, 'wt') as qstr_file:
            qstr_set = set([])
            for named_pin in self.cpu_pins:
                pin = named_pin.pin()
                if pin.is_board_pin():
                    qstr_set |= set([named_pin.name()])
            for named_pin in self.board_pins:
                qstr_set |= set([named_pin.name()])
            for qstr in sorted(qstr_set):
                print('Q({})'.format(qstr), file=qstr_file)


def main():
    parser = argparse.ArgumentParser(
        prog="make-pins.py",
        usage="%(prog)s [options] [command]",
        description="Generate board specific pin file"
    )
    parser.add_argument(
        "-a", "--af",
        dest="af_filename",
        help="Specifies the alternate function file for the chip",
        default="nrf_af.csv"
    )
    parser.add_argument(
        "-b", "--board",
        dest="board_filename",
        help="Specifies the board file",
    )
    parser.add_argument(
        "-p", "--prefix",
        dest="prefix_filename",
        help="Specifies beginning portion of generated pins file",
        default="nrf52_prefix.c"
    )
    parser.add_argument(
        "-q", "--qstr",
        dest="qstr_filename",
        help="Specifies name of generated qstr header file",
        default="build/pins_qstr.h"
    )
    parser.add_argument(
        "-r", "--hdr",
        dest="hdr_filename",
        help="Specifies name of generated pin header file",
        default="build/pins.h"
    )
    args = parser.parse_args(sys.argv[1:])

    pins = Pins()

    print('// This file was automatically generated by make-pins.py')
    print('//')
    if args.af_filename:
        print('// --af {:s}'.format(args.af_filename))
        pins.parse_af_file(args.af_filename)

    if args.board_filename:
        print('// --board {:s}'.format(args.board_filename))
        pins.parse_board_file(args.board_filename)

    if args.prefix_filename:
        print('// --prefix {:s}'.format(args.prefix_filename))
        print('')
        with open(args.prefix_filename, 'r') as prefix_file:
            print(prefix_file.read())
    pins.print()
    pins.print_header(args.hdr_filename)
    pins.print_qstr(args.qstr_filename)


if __name__ == "__main__":
    main()

#!/usr/bin/env python
"""Generates the pins files for the CC3200."""

from __future__ import print_function

import argparse
import sys
import csv


def parse_port_pin(name_str):
    """Parses a string and returns a (port, gpio_bit) tuple."""
    if len(name_str) < 5:
        raise ValueError("Expecting pin name to be at least 5 characters")
    if name_str[:4] != 'GPIO':
        raise ValueError("Expecting pin name to start with GPIO")
    if not name_str[4:].isdigit():
        raise ValueError("Expecting numeric GPIO number")
    port = int(int(name_str[4:]) / 8)
    gpio_bit = 1 << int(int(name_str[4:]) % 8)
    return (port, gpio_bit)


class Pin(object):
    """Holds the information associated with a pin."""
    def __init__(self, name, port, gpio_bit, pin_num):
        self.name = name
        self.port = port
        self.gpio_bit = gpio_bit
        self.pin_num = pin_num
        self.board_pin = False

    def cpu_pin_name(self):
        return self.name  
        
    def is_board_pin(self):
        return self.board_pin

    def set_is_board_pin(self):
        self.board_pin = True
        
    def print(self):
        print('const pin_obj_t pin_{:6s} = PIN({:6s}, {:1d}, {:3d}, {:2d});'.format(
               self.name, self.name, self.port, self.gpio_bit, self.pin_num))

    def print_header(self, hdr_file):
        hdr_file.write('extern const pin_obj_t pin_{:s};\n'.
                       format(self.name))


class Pins(object):

    def __init__(self):
        self.cpu_pins = []   # list of pin objects

    def find_pin(self, port, gpio_bit):
        for pin in self.cpu_pins:
            if pin.port == port and pin.gpio_bit == gpio_bit:
                return pin
                
    def find_pin_by_num(self, pin_num):
        for pin in self.cpu_pins:
            if pin.pin_num == pin_num:
                return pin

    def parse_af_file(self, filename, pin_col, pinname_col):
        with open(filename, 'r') as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                try:
                    (port_num, gpio_bit) = parse_port_pin(row[pinname_col])
                except:
                    continue
                if not row[pin_col].isdigit():
                    raise ValueError("Invalid pin number:  {:s} in row {:s}".format(row[pin_col]), row)
                # Pin numbers must start from 0 when used with the TI API
                pin_num = int(row[pin_col]) - 1;        
                pin = Pin(row[pinname_col], port_num, gpio_bit, pin_num)
                self.cpu_pins.append(pin)
                
    def parse_board_file(self, filename, cpu_pin_num_col):
        with open(filename, 'r') as csvfile:
            rows = csv.reader(csvfile)
            for row in rows:
                # Pin numbers must start from 0 when used with the TI API
                pin = self.find_pin_by_num(int(row[cpu_pin_num_col]) - 1)
                if pin:
                    pin.set_is_board_pin()

    def print_named(self, label, pins):
        print('')
        print('STATIC const mp_map_elem_t pin_{:s}_pins_locals_dict_table[] = {{'.format(label))
        for pin in pins:
            if pin.is_board_pin():
                print('  {{ MP_OBJ_NEW_QSTR(MP_QSTR_{:6s}), (mp_obj_t)&pin_{:6s} }},'.format(pin.cpu_pin_name(),  pin.cpu_pin_name()))
        print('};')
        print('MP_DEFINE_CONST_DICT(pin_{:s}_pins_locals_dict, pin_{:s}_pins_locals_dict_table);'.format(label, label));

    def print(self):
        for pin in self.cpu_pins:
            if pin.is_board_pin():
                pin.print()
        self.print_named('cpu', self.cpu_pins)
        print('')

    def print_header(self, hdr_filename):
        with open(hdr_filename, 'wt') as hdr_file:
            for pin in self.cpu_pins:
                if pin.is_board_pin():
                    pin.print_header(hdr_file)

    def print_qstr(self, qstr_filename):
        with open(qstr_filename, 'wt') as qstr_file:
            qstr_set = set([])
            for pin in self.cpu_pins:
                if pin.is_board_pin():
                    qstr_set |= set([pin.cpu_pin_name()])
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
        default="cc3200_af.csv"
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
        default="cc3200_prefix.c"
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
        pins.parse_af_file(args.af_filename, 0, 1)

    if args.board_filename:
        print('// --board {:s}'.format(args.board_filename))
        pins.parse_board_file(args.board_filename, 1)    
        
    if args.prefix_filename:
        print('// --prefix {:s}'.format(args.prefix_filename))
        print('')
        with open(args.prefix_filename, 'r') as prefix_file:
            print(prefix_file.read())
    pins.print()
    pins.print_qstr(args.qstr_filename)
    pins.print_header(args.hdr_filename)


if __name__ == "__main__":
    main()

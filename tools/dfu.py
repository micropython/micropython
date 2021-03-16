#!/usr/bin/python

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

# Written by Antonio Galea - 2010/11/18
# Updated for DFU 1.1 by Sean Cross - 2020/03/31
# Distributed under Gnu LGPL 3.0
# see http://www.gnu.org/licenses/lgpl-3.0.txt

import sys, struct, zlib, os
from optparse import OptionParser

DEFAULT_DEVICE = "0x0483:0xdf11"


def named(tuple, names):
    return dict(zip(names.split(), tuple))


def consume(fmt, data, names):
    n = struct.calcsize(fmt)
    return named(struct.unpack(fmt, data[:n]), names), data[n:]


def cstring(string):
    return string.split("\0", 1)[0]


def compute_crc(data):
    return 0xFFFFFFFF & -zlib.crc32(data) - 1


def parse(file, dump_images=False):
    print('File: "%s"' % file)
    data = open(file, "rb").read()
    crc = compute_crc(data[:-4])
    data = data[len(data) - 16 :]
    suffix = named(struct.unpack("<4H3sBI", data[:16]), "device product vendor dfu ufd len crc")
    print(
        "usb: %(vendor)04x:%(product)04x, device: 0x%(device)04x, dfu: 0x%(dfu)04x, %(ufd)s, %(len)d, 0x%(crc)08x"
        % suffix
    )
    if crc != suffix["crc"]:
        print("CRC ERROR: computed crc32 is 0x%08x" % crc)
    data = data[16:]
    if data:
        print("PARSE ERROR")


def build(file, data, device=DEFAULT_DEVICE):
    # Parse the VID and PID from the `device` argument
    v, d = map(lambda x: int(x, 0) & 0xFFFF, device.split(":", 1))

    # Generate the DFU suffix, consisting of these fields:
    #  Field name     | Length  |  Description
    # ================+=========+================================
    #  bcdDevice      |    2    | The release number of this firmware (0xffff - don't care)
    #  idProduct      |    2    | PID of this device
    #  idVendor       |    2    | VID of this device
    #  bcdDFU         |    2    | Version of this DFU spec (0x01 0x00)
    #  ucDfuSignature |    3    | The characters 'DFU', printed in reverse order
    #  bLength        |    1    | The length of this suffix (16 bytes)
    #  dwCRC          |    4    | A CRC32 of the data, including this suffix
    data += struct.pack("<4H3sB", 0xFFFF, d, v, 0x0100, b"UFD", 16)
    crc = compute_crc(data)
    # Append the CRC32 of the entire block
    data += struct.pack("<I", crc)
    open(file, "wb").write(data)


if __name__ == "__main__":
    usage = """
%prog [-d|--dump] infile.dfu
%prog {-b|--build} file.bin [{-D|--device}=vendor:device] outfile.dfu"""
    parser = OptionParser(usage=usage)
    parser.add_option(
        "-b",
        "--build",
        action="store",
        dest="binfile",
        help="build a DFU file from given BINFILE",
        metavar="BINFILE",
    )
    parser.add_option(
        "-D",
        "--device",
        action="store",
        dest="device",
        help="build for DEVICE, defaults to %s" % DEFAULT_DEVICE,
        metavar="DEVICE",
    )
    parser.add_option(
        "-d",
        "--dump",
        action="store_true",
        dest="dump_images",
        default=False,
        help="dump contained images to current directory",
    )
    (options, args) = parser.parse_args()

    if options.binfile and len(args) == 1:
        binfile = options.binfile
        if not os.path.isfile(binfile):
            print("Unreadable file '%s'." % binfile)
            sys.exit(1)
        target = open(binfile, "rb").read()
        outfile = args[0]
        device = DEFAULT_DEVICE
        # If a device is specified, parse the pair into a VID:PID pair
        # in order to validate them.
        if options.device:
            device = options.device
        try:
            v, d = map(lambda x: int(x, 0) & 0xFFFF, device.split(":", 1))
        except:
            print("Invalid device '%s'." % device)
            sys.exit(1)
        build(outfile, target, device)
    elif len(args) == 1:
        infile = args[0]
        if not os.path.isfile(infile):
            print("Unreadable file '%s'." % infile)
            sys.exit(1)
        parse(infile, dump_images=options.dump_images)
    else:
        parser.print_help()
        sys.exit(1)

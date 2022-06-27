#!/usr/bin/python

# Written by Antonio Galea - 2010/11/18
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
    return string.split(b"\0", 1)[0]


def compute_crc(data):
    return 0xFFFFFFFF & -zlib.crc32(data) - 1


def parse(file, dump_images=False):
    print('File: "%s"' % file)
    data = open(file, "rb").read()
    crc = compute_crc(data[:-4])
    prefix, data = consume("<5sBIB", data, "signature version size targets")
    print("%(signature)s v%(version)d, image size: %(size)d, targets: %(targets)d" % prefix)
    for t in range(prefix["targets"]):
        tprefix, data = consume(
            "<6sBI255s2I", data, "signature altsetting named name size elements"
        )
        tprefix["num"] = t
        if tprefix["named"]:
            tprefix["name"] = cstring(tprefix["name"])
        else:
            tprefix["name"] = ""
        print(
            '%(signature)s %(num)d, alt setting: %(altsetting)s, name: "%(name)s", size: %(size)d, elements: %(elements)d'
            % tprefix
        )
        tsize = tprefix["size"]
        target, data = data[:tsize], data[tsize:]
        for e in range(tprefix["elements"]):
            eprefix, target = consume("<2I", target, "address size")
            eprefix["num"] = e
            print("  %(num)d, address: 0x%(address)08x, size: %(size)d" % eprefix)
            esize = eprefix["size"]
            image, target = target[:esize], target[esize:]
            if dump_images:
                out = "%s.target%d.image%d.bin" % (file, t, e)
                open(out, "wb").write(image)
                print('    DUMPED IMAGE TO "%s"' % out)
        if len(target):
            print("target %d: PARSE ERROR" % t)
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


def build(file, targets, device=DEFAULT_DEVICE):
    data = b""
    for t, target in enumerate(targets):
        tdata = b""
        for image in target:
            # pad image to 8 bytes (needed at least for L476)
            pad = (8 - len(image["data"]) % 8) % 8
            image["data"] = image["data"] + bytes(bytearray(8)[0:pad])
            #
            tdata += struct.pack("<2I", image["address"], len(image["data"])) + image["data"]
        tdata = (
            struct.pack("<6sBI255s2I", b"Target", 0, 1, b"ST...", len(tdata), len(target)) + tdata
        )
        data += tdata
    data = struct.pack("<5sBIB", b"DfuSe", 1, len(data) + 11, len(targets)) + data
    v, d = map(lambda x: int(x, 0) & 0xFFFF, device.split(":", 1))
    data += struct.pack("<4H3sB", 0, d, v, 0x011A, b"UFD", 16)
    crc = compute_crc(data)
    data += struct.pack("<I", crc)
    open(file, "wb").write(data)


if __name__ == "__main__":
    usage = """
%prog [-d|--dump] infile.dfu
%prog {-b|--build} address:file.bin [-b address:file.bin ...] [{-D|--device}=vendor:device] outfile.dfu"""
    parser = OptionParser(usage=usage)
    parser.add_option(
        "-b",
        "--build",
        action="append",
        dest="binfiles",
        help="build a DFU file from given BINFILES",
        metavar="BINFILES",
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

    if options.binfiles and len(args) == 1:
        target = []
        for arg in options.binfiles:
            try:
                address, binfile = arg.split(":", 1)
            except ValueError:
                print("Address:file couple '%s' invalid." % arg)
                sys.exit(1)
            try:
                address = int(address, 0) & 0xFFFFFFFF
            except ValueError:
                print("Address %s invalid." % address)
                sys.exit(1)
            if not os.path.isfile(binfile):
                print("Unreadable file '%s'." % binfile)
                sys.exit(1)
            target.append({"address": address, "data": open(binfile, "rb").read()})
        outfile = args[0]
        device = DEFAULT_DEVICE
        if options.device:
            device = options.device
        try:
            v, d = map(lambda x: int(x, 0) & 0xFFFF, device.split(":", 1))
        except:
            print("Invalid device '%s'." % device)
            sys.exit(1)
        build(outfile, [target], device)
    elif len(args) == 1:
        infile = args[0]
        if not os.path.isfile(infile):
            print("Unreadable file '%s'." % infile)
            sys.exit(1)
        parse(infile, dump_images=options.dump_images)
    else:
        parser.print_help()
        sys.exit(1)

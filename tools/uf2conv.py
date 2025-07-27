#!/usr/bin/env python3

# Microsoft UF2
#
# The MIT License (MIT)
#
# Copyright (c) Microsoft Corporation
#
# All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import sys
import struct
import subprocess
import re
import os
import os.path
import argparse
import json


UF2_MAGIC_START0 = 0x0A324655  # "UF2\n"
UF2_MAGIC_START1 = 0x9E5D5157  # Randomly selected
UF2_MAGIC_END = 0x0AB16F30  # Ditto

INFO_FILE = "/INFO_UF2.TXT"

appstartaddr = 0x2000
familyid = 0x0


def is_uf2(buf):
    w = struct.unpack("<II", buf[0:8])
    return w[0] == UF2_MAGIC_START0 and w[1] == UF2_MAGIC_START1


def is_hex(buf):
    try:
        w = buf[0:30].decode("utf-8")
    except UnicodeDecodeError:
        return False
    if w[0] == ":" and re.match(b"^[:0-9a-fA-F\r\n]+$", buf):
        return True
    return False


def convert_from_uf2(buf):
    global appstartaddr
    global familyid
    numblocks = len(buf) // 512
    curraddr = None
    currfamilyid = None
    families_found = {}
    prev_flag = None
    all_flags_same = True
    outp = []
    for blockno in range(numblocks):
        ptr = blockno * 512
        block = buf[ptr : ptr + 512]
        hd = struct.unpack(b"<IIIIIIII", block[0:32])
        if hd[0] != UF2_MAGIC_START0 or hd[1] != UF2_MAGIC_START1:
            print("Skipping block at " + ptr + "; bad magic")
            continue
        if hd[2] & 1:
            # NO-flash flag set; skip block
            continue
        datalen = hd[4]
        if datalen > 476:
            assert False, "Invalid UF2 data size at " + ptr
        newaddr = hd[3]
        if (hd[2] & 0x2000) and (currfamilyid is None):
            currfamilyid = hd[7]
        if curraddr is None or ((hd[2] & 0x2000) and hd[7] != currfamilyid):
            currfamilyid = hd[7]
            curraddr = newaddr
            if familyid == 0x0 or familyid == hd[7]:
                appstartaddr = newaddr
        padding = newaddr - curraddr
        if padding < 0:
            assert False, "Block out of order at " + ptr
        if padding > 10 * 1024 * 1024:
            assert False, "More than 10M of padding needed at " + ptr
        if padding % 4 != 0:
            assert False, "Non-word padding size at " + ptr
        while padding > 0:
            padding -= 4
            outp += b"\x00\x00\x00\x00"
        if familyid == 0x0 or ((hd[2] & 0x2000) and familyid == hd[7]):
            outp.append(block[32 : 32 + datalen])
        curraddr = newaddr + datalen
        if hd[2] & 0x2000:
            if hd[7] in families_found.keys():
                if families_found[hd[7]] > newaddr:
                    families_found[hd[7]] = newaddr
            else:
                families_found[hd[7]] = newaddr
        if prev_flag is None:
            prev_flag = hd[2]
        if prev_flag != hd[2]:
            all_flags_same = False
        if blockno == (numblocks - 1):
            print("--- UF2 File Header Info ---")
            families = load_families()
            for family_hex, target_address in families_found.items():
                family_short_name = ""
                for name, value in families.items():
                    if value == family_hex:
                        family_short_name = name
                print(
                    "Family ID is {:s}, hex value is 0x{:08x}".format(
                        family_short_name, family_hex
                    )
                )
                print("Target Address is 0x{:08x}".format(target_address))
            if all_flags_same:
                print("All block flag values consistent, 0x{:04x}".format(hd[2]))
            else:
                print("Flags were not all the same")
            print("----------------------------")
            if len(families_found) > 1 and familyid == 0x0:
                outp = []
                appstartaddr = 0x0
    return b"".join(outp)


def convert_to_carray(file_content):
    outp = "const unsigned long bindata_len = %d;\n" % len(file_content)
    outp += "const unsigned char bindata[] __attribute__((aligned(16))) = {"
    for i in range(len(file_content)):
        if i % 16 == 0:
            outp += "\n"
        outp += "0x%02x, " % file_content[i]
    outp += "\n};\n"
    return bytes(outp, "utf-8")


def convert_to_uf2(file_content):
    global familyid
    datapadding = b""
    while len(datapadding) < 512 - 256 - 32 - 4:
        datapadding += b"\x00\x00\x00\x00"
    numblocks = (len(file_content) + 255) // 256
    outp = []
    for blockno in range(numblocks):
        ptr = 256 * blockno
        chunk = file_content[ptr : ptr + 256]
        flags = 0x0
        if familyid:
            flags |= 0x2000
        hd = struct.pack(
            b"<IIIIIIII",
            UF2_MAGIC_START0,
            UF2_MAGIC_START1,
            flags,
            ptr + appstartaddr,
            256,
            blockno,
            numblocks,
            familyid,
        )
        while len(chunk) < 256:
            chunk += b"\x00"
        block = hd + chunk + datapadding + struct.pack(b"<I", UF2_MAGIC_END)
        assert len(block) == 512
        outp.append(block)
    return b"".join(outp)


class Block:
    def __init__(self, addr):
        self.addr = addr
        self.bytes = bytearray(256)

    def encode(self, blockno, numblocks):
        global familyid
        flags = 0x0
        if familyid:
            flags |= 0x2000
        hd = struct.pack(
            "<IIIIIIII",
            UF2_MAGIC_START0,
            UF2_MAGIC_START1,
            flags,
            self.addr,
            256,
            blockno,
            numblocks,
            familyid,
        )
        hd += self.bytes[0:256]
        while len(hd) < 512 - 4:
            hd += b"\x00"
        hd += struct.pack("<I", UF2_MAGIC_END)
        return hd


def convert_from_hex_to_uf2(buf):
    global appstartaddr
    appstartaddr = None
    upper = 0
    currblock = None
    blocks = []
    for line in buf.split("\n"):
        if line[0] != ":":
            continue
        i = 1
        rec = []
        while i < len(line) - 1:
            rec.append(int(line[i : i + 2], 16))
            i += 2
        tp = rec[3]
        if tp == 4:
            upper = ((rec[4] << 8) | rec[5]) << 16
        elif tp == 2:
            upper = ((rec[4] << 8) | rec[5]) << 4
        elif tp == 1:
            break
        elif tp == 0:
            addr = upper + ((rec[1] << 8) | rec[2])
            if appstartaddr is None:
                appstartaddr = addr
            i = 4
            while i < len(rec) - 1:
                if not currblock or currblock.addr & ~0xFF != addr & ~0xFF:
                    currblock = Block(addr & ~0xFF)
                    blocks.append(currblock)
                currblock.bytes[addr & 0xFF] = rec[i]
                addr += 1
                i += 1
    numblocks = len(blocks)
    resfile = b""
    for i in range(0, numblocks):
        resfile += blocks[i].encode(i, numblocks)
    return resfile


def to_str(b):
    return b.decode("utf-8")


def get_drives():
    drives = []
    if sys.platform == "win32":
        r = subprocess.check_output(
            [
                "wmic",
                "PATH",
                "Win32_LogicalDisk",
                "get",
                "DeviceID,",
                "VolumeName,",
                "FileSystem,",
                "DriveType",
            ]
        )
        for line in to_str(r).split("\n"):
            words = re.split(r"\s+", line)
            if len(words) >= 3 and words[1] == "2" and words[2] == "FAT":
                drives.append(words[0])
    else:
        rootpath = "/media"
        if sys.platform == "darwin":
            rootpath = "/Volumes"
        elif sys.platform == "linux":
            tmp = rootpath + "/" + os.environ["USER"]
            if os.path.isdir(tmp):
                rootpath = tmp
        drives.extend(os.path.join(rootpath, d) for d in os.listdir(rootpath))

    def has_info(d):
        try:
            return os.path.isfile(d + INFO_FILE)
        except:
            return False

    return list(filter(has_info, drives))


def board_id(path):
    with open(path + INFO_FILE, mode="r") as file:
        file_content = file.read()
    return re.search("Board-ID: ([^\r\n]*)", file_content).group(1)


def list_drives():
    for d in get_drives():
        print(d, board_id(d))


def write_file(name, buf):
    with open(name, "wb") as f:
        f.write(buf)
    print("Wrote %d bytes to %s" % (len(buf), name))


def load_families():
    # The expectation is that the `uf2families.json` file is in the same
    # directory as this script. Make a path that works using `__file__`
    # which contains the full path to this script.
    filename = "uf2families.json"
    pathname = os.path.join(os.path.dirname(os.path.abspath(__file__)), filename)
    with open(pathname) as f:
        raw_families = json.load(f)

    families = {}
    for family in raw_families:
        families[family["short_name"]] = int(family["id"], 0)

    return families


def main():
    global appstartaddr, familyid

    def error(msg):
        print(msg)
        sys.exit(1)

    parser = argparse.ArgumentParser(description="Convert to UF2 or flash directly.")
    parser.add_argument(
        "input", metavar="INPUT", type=str, nargs="?", help="input file (HEX, BIN or UF2)"
    )
    parser.add_argument(
        "-b",
        "--base",
        dest="base",
        type=str,
        default="0x2000",
        help="set base address of application for BIN format (default: 0x2000)",
    )
    parser.add_argument(
        "-o",
        "--output",
        metavar="FILE",
        dest="output",
        type=str,
        help='write output to named file; defaults to "flash.uf2" or "flash.bin" where sensible',
    )
    parser.add_argument("-d", "--device", dest="device_path", help="select a device path to flash")
    parser.add_argument("-l", "--list", action="store_true", help="list connected devices")
    parser.add_argument("-c", "--convert", action="store_true", help="do not flash, just convert")
    parser.add_argument("-D", "--deploy", action="store_true", help="just flash, do not convert")
    parser.add_argument(
        "-f",
        "--family",
        dest="family",
        type=str,
        default="0x0",
        help="specify familyID - number or name (default: 0x0)",
    )
    parser.add_argument(
        "-C", "--carray", action="store_true", help="convert binary file to a C array, not UF2"
    )
    parser.add_argument(
        "-i",
        "--info",
        action="store_true",
        help="display header information from UF2, do not convert",
    )
    args = parser.parse_args()
    appstartaddr = int(args.base, 0)

    families = load_families()

    if args.family.upper() in families:
        familyid = families[args.family.upper()]
    else:
        try:
            familyid = int(args.family, 0)
        except ValueError:
            error("Family ID needs to be a number or one of: " + ", ".join(families.keys()))

    if args.list:
        list_drives()
    else:
        if not args.input:
            error("Need input file")
        with open(args.input, mode="rb") as f:
            inpbuf = f.read()
        from_uf2 = is_uf2(inpbuf)
        ext = "uf2"
        if args.deploy:
            outbuf = inpbuf
        elif from_uf2 and not args.info:
            outbuf = convert_from_uf2(inpbuf)
            ext = "bin"
        elif from_uf2 and args.info:
            outbuf = ""
            convert_from_uf2(inpbuf)
        elif is_hex(inpbuf):
            outbuf = convert_from_hex_to_uf2(inpbuf.decode("utf-8"))
        elif args.carray:
            outbuf = convert_to_carray(inpbuf)
            ext = "h"
        else:
            outbuf = convert_to_uf2(inpbuf)
        if not args.deploy and not args.info:
            print(
                "Converted to %s, output size: %d, start address: 0x%x"
                % (ext, len(outbuf), appstartaddr)
            )
        if args.convert or ext != "uf2":
            drives = []
            if args.output is None:
                args.output = "flash." + ext
        else:
            drives = get_drives()

        if args.output:
            write_file(args.output, outbuf)
        else:
            if len(drives) == 0:
                error("No drive to deploy.")
        for d in drives:
            print("Flashing %s (%s)" % (d, board_id(d)))
            write_file(d + "/NEW.UF2", outbuf)


if __name__ == "__main__":
    main()

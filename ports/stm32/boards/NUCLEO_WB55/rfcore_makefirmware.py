#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2020 Jim Mussared
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# This script obfuscates the ST wireless binaries so they can be safely copied
# to the flash filesystem and not be accidentally discovered by the FUS during
# an update. See more information (and the corresponding de-obfuscation) in
# rfcore_firmware.py as well as instructions on how to use.

import os
import re
import struct
import sys
from binascii import crc32
from rfcore_firmware import validate_crc, _OBFUSCATION_KEY


_FIRMWARE_FILES = {
    "stm32wb5x_FUS_fw_1_0_2.bin": "fus_102.bin",
    "stm32wb5x_FUS_fw.bin": "fus_110.bin",
    "stm32wb5x_BLE_HCILayer_fw.bin": "ws_ble_hci.bin",
}
_RELEASE_NOTES = "Release_Notes.html"


def get_details(release_notes, filename):
    if not release_notes:
        return None
    file_details = re.findall(
        rb"%s,(((0x[\d\S]+?),)+[vV][\d\.]+)[<,]" % filename.encode(),
        release_notes,
        flags=re.DOTALL,
    )
    # The release note has all past version details also, but current is at top
    latest_details = file_details[0][0].split(b",")
    addr_1m, addr_640k, addr_512k, addr_256k, version = latest_details
    addr_1m = int(addr_1m, 0)
    addr_640k = int(addr_640k, 0)
    addr_512k = int(addr_512k, 0)
    addr_256k = int(addr_256k, 0)
    version = [int(v) for v in version.lower().lstrip(b"v").split(b".")]
    return addr_1m, addr_640k, addr_512k, addr_256k, version


def main(src_path, dest_path):
    # Load the release note to parse for important details
    with open(os.path.join(src_path, _RELEASE_NOTES), "rb") as f:
        release_notes = f.read()
    # remove some formatting
    release_notes = re.sub(rb"</?strong>", b"", release_notes)
    # flatten tables
    release_notes = re.sub(
        rb"</t[dh]>\W*\n*\W*",
        b",",
        release_notes.replace(b"<td>", b"").replace(b"<th>", b""),
    )
    if (
        b"Wireless Coprocessor Binary,STM32WB5xxG(1M),STM32WB5xxY(640k),STM32WB5xxE(512K),STM32WB5xxC(256K),Version,"
        not in release_notes
    ):
        raise SystemExit(
            "Cannot determine binary load address, please confirm Coprocessor folder / Release Notes format."
        )

    for src_filename, dest_file in _FIRMWARE_FILES.items():
        src_file = os.path.join(src_path, src_filename)
        dest_file = os.path.join(dest_path, dest_file)
        if not os.path.exists(src_file):
            print("Unable to find: {}".format(src_file))
            continue
        sz = 0
        with open(src_file, "rb") as src:
            crc = 0
            with open(dest_file, "wb") as dest:
                while True:
                    b = src.read(4)
                    if not b:
                        break
                    (v,) = struct.unpack("<I", b)
                    v ^= _OBFUSCATION_KEY
                    vs = struct.pack("<I", v)
                    dest.write(vs)
                    crc = crc32(vs, crc)
                    sz += 4

                addr_1m, addr_640k, addr_512k, addr_256k, version = get_details(
                    release_notes, src_filename
                )

                footer = struct.pack(
                    "<37sIIIIbbbI",
                    src_filename.encode(),
                    addr_1m,
                    addr_640k,
                    addr_512k,
                    addr_256k,
                    *version,
                    _OBFUSCATION_KEY,
                )
                assert len(footer) == 60
                dest.write(footer)
                crc = crc32(footer, crc)
                crc = 0xFFFFFFFF & -crc - 1
                dest.write(struct.pack("<I", crc))
                sz += 64

        print(
            f"Written {src_filename} v{version[0]}.{version[1]}.{version[2]} to {dest_file} ({sz} bytes)"
        )
        check_file_details(dest_file)


def check_file_details(filename):
    """Should match copy of function in rfcore_firmware.py to confirm operation"""
    with open(filename, "rb") as f:
        if not validate_crc(f):
            raise ValueError("file validation failed: incorrect crc")

        f.seek(-64, 2)
        footer = f.read()
        assert len(footer) == 64
        details = struct.unpack("<37sIIIIbbbII", footer)
        (
            src_filename,
            addr_1m,
            addr_640k,
            addr_512k,
            addr_256k,
            vers_major,
            vers_minor,
            vers_patch,
            KEY,
            crc,
        ) = details
        if KEY != _OBFUSCATION_KEY:
            raise ValueError("file validation failed: incorrect key")

    return (
        src_filename,
        addr_1m,
        addr_640k,
        addr_512k,
        addr_256k,
        (vers_major, vers_minor, vers_patch),
    )


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: {} src_path dest_path".format(sys.argv[0]))
        print()
        print(
            '"src_path" should be the location of the ST binaries from https://github.com/STMicroelectronics/STM32CubeWB/tree/master/Projects/STM32WB_Copro_Wireless_Binaries/STM32WB5x'
        )
        print(
            '"dest_path" will be where fus_102.bin, fus_110.bin, and ws_ble_hci.bin will be written to.'
        )
        sys.exit(1)

    main(sys.argv[1], sys.argv[2])

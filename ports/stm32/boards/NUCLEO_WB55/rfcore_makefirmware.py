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
import struct
import sys

# Must match rfcore_firmware.py.
_OBFUSCATION_KEY = 0x0573B55AA

_FIRMWARE_FILES = {
    "stm32wb5x_FUS_fw_1_0_2.bin": "fus_102.bin",
    "stm32wb5x_FUS_fw.bin": "fus_110.bin",
    "stm32wb5x_BLE_HCILayer_fw.bin": "ws_ble_hci.bin",
}


def main(src_path, dest_path):
    for src_file, dest_file in _FIRMWARE_FILES.items():
        src_file = os.path.join(src_path, src_file)
        dest_file = os.path.join(dest_path, dest_file)
        if not os.path.exists(src_file):
            print("Unable to find: {}".format(src_file))
            continue
        sz = 0
        with open(src_file, "rb") as src:
            with open(dest_file, "wb") as dest:
                while True:
                    b = src.read(4)
                    if not b:
                        break
                    (v,) = struct.unpack("<I", b)
                    v ^= _OBFUSCATION_KEY
                    dest.write(struct.pack("<I", v))
                    sz += 4
        print("Written {} ({} bytes)".format(dest_file, sz))


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

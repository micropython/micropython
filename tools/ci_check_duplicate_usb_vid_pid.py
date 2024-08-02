#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2020 Michael Schroeder
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

from collections import defaultdict
import argparse
import pathlib
import re
import sys

DEFAULT_CLUSTERLIST = {
    "0x04D8:0xEC44": ["pycubed", "pycubed_mram", "pycubed_mram_v05", "pycubed_v05"],
    "0x1B4F:0x8D24": ["sparkfun_qwiic_micro_no_flash", "sparkfun_qwiic_micro_with_flash"],
    "0x1D50:0x6153": [
        "jpconstantineau_pykey18",
        "jpconstantineau_pykey44",
        "jpconstantineau_pykey60",
        "jpconstantineau_pykey87",
    ],
    "0x239A:0x8019": [
        "circuitplayground_express",
        "circuitplayground_express_crickit",
        "circuitplayground_express_displayio",
    ],
    "0x239A:0x801F": ["trinket_m0_haxpress", "trinket_m0"],
    "0x239A:0x8021": ["metro_m4_express", "cp32-m4"],
    "0x239A:0x8023": ["feather_m0_express", "feather_m0_supersized"],
    "0x239A:0x80A6": ["espressif_esp32s2_devkitc_1_n4r2", "espressif_saola_1_wrover"],
    "0x239A:0x80AC": ["unexpectedmaker_feathers2", "unexpectedmaker_feathers2_prerelease"],
    "0x239A:0x80C8": ["espressif_kaluga_1", "espressif_kaluga_1.3"],
    "0x303A:0x7003": [
        "espressif_esp32s3_devkitc_1_n8",
        "espressif_esp32s3_devkitc_1_n8r2",
        "espressif_esp32s3_devkitc_1_n8r8",
        "espressif_esp32s3_devkitc_1_n32r8",
        "espressif_esp32s3_devkitc_1_n8r8_hacktablet",
    ],
    "0x303A:0x7009": [
        "espressif_esp32s2_devkitc_1_n4",
        "espressif_esp32s2_devkitc_1_n4r2",
        "espressif_esp32s2_devkitc_1_n8r2",
    ],
    "0x239A:0x102E": ["weact_studio_pico", "weact_studio_pico_16mb"],
    "0x303A:0x8166": ["yd_esp32_s3_n8r8", "yd_esp32_s3_n16r8"],
    "0x2341:0x056B": ["arduino_nano_esp32s3", "arduino_nano_esp32s3_inverted_statusled"],
    "0x2E8A:0x1020": ["waveshare_rp2040_plus_4mb", "waveshare_rp2040_plus_16mb"],
    "0x2341:0x805A": ["arduino_nano_33_ble", "arduino_nano_33_ble_rev2"],
}

cli_parser = argparse.ArgumentParser(
    description="USB VID/PID and Creator/Creation ID Duplicate Checker"
)


def configboard_files():
    """A pathlib glob search for all ports/*/boards/*/mpconfigboard.mk file
    paths.

    :returns: A ``pathlib.Path.glob()`` generator object
    """
    working_dir = pathlib.Path(__file__).resolve().parent.parent
    return working_dir.glob("ports/**/boards/**/mpconfigboard.mk")


VID_PATTERN = re.compile(r"^USB_VID\s*=\s*(.*)", flags=re.M)
PID_PATTERN = re.compile(r"^USB_PID\s*=\s*(.*)", flags=re.M)
CREATOR_PATTERN = re.compile(r"^CIRCUITPY_CREATOR_ID\s*=\s*(.*)", flags=re.M)
CREATION_PATTERN = re.compile(r"^CIRCUITPY_CREATION_ID\s*=\s*(.*)", flags=re.M)


def check_vid_pid(files, clusterlist):
    """Compiles a list of USB VID & PID values for all boards, and checks
    for duplicates. Exits with ``sys.exit()`` (non-zero exit code)
    if duplicates are found, and lists the duplicates.
    """

    usb_pattern = re.compile(
        r"^CIRCUITPY_USB_DEVICE\s*=\s*0$|^IDF_TARGET = (esp32|esp32c2|esp32c3|esp32c6|esp32h2)$|^MCU_SERIES = MG24$",
        flags=re.M,
    )

    usb_ids = defaultdict(set)
    for board_config in files:
        src_text = board_config.read_text()

        usb_vid = VID_PATTERN.search(src_text)
        usb_pid = PID_PATTERN.search(src_text)
        creator = CREATOR_PATTERN.search(src_text)
        creation = CREATION_PATTERN.search(src_text)
        non_usb = usb_pattern.search(src_text)
        board_name = board_config.parts[-2]
        port_name = board_config.parts[-4]

        if port_name == "renode":
            continue

        if usb_vid and usb_pid:
            id_group = f"0x{int(usb_vid.group(1), 16):04X}:0x{int(usb_pid.group(1), 16):04X}"
        elif non_usb:
            if creator is None or creation is None:
                print(f"{board_name=} {creator=} {creation=}", file=sys.stderr)
                continue
            id_group = f"0x{int(creator.group(1), 16):08X}:0x{int(creation.group(1), 16):08X}"
        else:
            raise SystemExit(f"Could not find expected settings in {board_config}")

        usb_ids[id_group].add(board_name)

    duplicates = []
    for key, boards in usb_ids.items():
        if len(boards) == 1:
            continue

        # It is a cluster
        cluster = set(clusterlist.get(key, []))
        if cluster != boards:
            if key == "":
                duplicates.append(f"- Non-USB:\n" f"  Boards: {', '.join(sorted(boards))}")
            else:
                duplicates.append(f"- VID/PID: {key}\n" f"  Boards: {', '.join(sorted(boards))}")

    if duplicates:
        duplicates = "\n".join(duplicates)
        duplicate_message = (
            f"Duplicate VID/PID usage found!\n{duplicates}\n"
            f"If you are open source maker, then you can request a PID from http://pid.codes\n"
            f"For boards without native USB, you can request a Creator ID from https://github.com/creationid/creators/\n"
            f"Otherwise, companies should pay the USB-IF for a vendor ID: https://www.usb.org/getting-vendor-id\n"
            f"FAQ: Why does CircuitPython require a unique VID:PID for every board definition? https://learn.adafruit.com/how-to-add-a-new-board-to-circuitpython/frequently-asked-questions#faq-3130480"
        )
        sys.exit(duplicate_message)

    else:
        print("No unexpected ID duplicates found.")


if __name__ == "__main__":
    arguments = cli_parser.parse_args()

    print("Running USB VID/PID and Creator/Creation ID Duplicate Checker...")

    board_files = configboard_files()
    check_vid_pid(board_files, DEFAULT_CLUSTERLIST)

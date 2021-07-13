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

import argparse
import pathlib
import re
import sys

DEFAULT_IGNORELIST = [
    "circuitplayground_express",
    "circuitplayground_express_crickit",
    "circuitplayground_express_displayio",
    "pycubed",
    "pycubed_mram",
    "pygamer",
    "pygamer_advance",
    "trinket_m0",
    "trinket_m0_haxpress",
    "sparkfun_qwiic_micro_with_flash",
    "sparkfun_qwiic_micro_no_flash",
    "feather_m0_express",
    "feather_m0_supersized",
    "cp32-m4",
    "metro_m4_express",
    "unexpectedmaker_feathers2",
    "unexpectedmaker_feathers2_prerelease",
    "espressif_kaluga_1",
    "espressif_kaluga_1.3",
]

cli_parser = argparse.ArgumentParser(description="USB VID/PID Duplicate Checker")
cli_parser.add_argument(
    "--ignorelist",
    dest="ignorelist",
    nargs="?",
    action="store",
    default=DEFAULT_IGNORELIST,
    help=(
        "Board names to ignore duplicate VID/PID combinations. Pass an empty "
        "string to disable all duplicate ignoring. Defaults are: "
        f"{', '.join(DEFAULT_IGNORELIST)}"
    ),
)


def configboard_files():
    """A pathlib glob search for all ports/*/boards/*/mpconfigboard.mk file
    paths.

    :returns: A ``pathlib.Path.glob()`` genarator object
    """
    working_dir = pathlib.Path().resolve()
    if not working_dir.name.startswith("circuitpython"):
        raise RuntimeError(
            "Please run USB VID/PID duplicate verification at the " "top-level directory."
        )
    return working_dir.glob("ports/**/boards/**/mpconfigboard.mk")


def check_vid_pid(files, ignorelist):
    """Compiles a list of USB VID & PID values for all boards, and checks
    for duplicates. Exits with ``sys.exit()`` (non-zero exit code)
    if duplicates are found, and lists the duplicates.
    """

    duplicates_found = False

    usb_ids = {}

    vid_pattern = re.compile(r"^USB_VID\s*\=\s*(.*)", flags=re.M)
    pid_pattern = re.compile(r"^USB_PID\s*\=\s*(.*)", flags=re.M)

    for board_config in files:
        src_text = board_config.read_text()

        usb_vid = vid_pattern.search(src_text)
        usb_pid = pid_pattern.search(src_text)

        board_name = board_config.parts[-2]

        board_ignorelisted = False
        if board_name in ignorelist:
            board_ignorelisted = True
            board_name += " (ignorelisted)"

        if usb_vid and usb_pid:
            id_group = f"{usb_vid.group(1)}:{usb_pid.group(1)}"
            if id_group not in usb_ids:
                usb_ids[id_group] = {"boards": [board_name], "duplicate": False}
            else:
                usb_ids[id_group]["boards"].append(board_name)
                if not board_ignorelisted:
                    usb_ids[id_group]["duplicate"] = True
                    duplicates_found = True

    if duplicates_found:
        duplicates = ""
        for key, value in usb_ids.items():
            if value["duplicate"]:
                duplicates += f"- VID/PID: {key}\n" f"  Boards: {', '.join(value['boards'])}\n"

        duplicate_message = (
            f"Duplicate VID/PID usage found!\n{duplicates}\n"
            f"If you are open source maker, then you can request a PID from http://pid.codes\n"
            f"Otherwise, companies should pay the USB-IF for a vendor ID: https://www.usb.org/getting-vendor-id"
        )
        sys.exit(duplicate_message)
    else:
        print("No USB PID duplicates found.")


if __name__ == "__main__":
    arguments = cli_parser.parse_args()

    print("Running USB VID/PID Duplicate Checker...")
    print(f"Ignoring the following boards: {', '.join(arguments.ignorelist)}", end="\n\n")

    board_files = configboard_files()
    check_vid_pid(board_files, arguments.ignorelist)

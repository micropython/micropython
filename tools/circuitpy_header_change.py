#!/usr/bin/env python3

# This file is part of the CircuitPython project: https://circuitpython.org

# SPDX-FileCopyrightText: Copyright (c) 2024 by Dan Halbert for Adafruit Industries
#
# SPDX-License-Identifier: MIT
#
# Casual tool to help convert old-style copyright/license headers to SPDX and label
# them as CircuitPython.
#
# Typical usages:
# header_change.py *.c *.h --change
# find boards common-hal/ peripherals/ supervisor/ -name '*.[ch]' |xargs header_change.py
# find boards common-hal/ peripherals/ supervisor/ -name '*.[ch]' |xargs header_change.py --change

import click
import os
import pathlib
import re
import sys

SPDX_COPYRIGHT_RE = re.compile(r"(SPDX-FileCopyrightText:.*$)")
ORIGINAL_COPYRIGHT_RE = re.compile(r"\* (.*Copyright .*[12].*$)", flags=re.IGNORECASE)
MIT_LICENSE_RE = re.compile(r"MIT License", flags=re.IGNORECASE)


def find_related_copyrights(filename):
    path = pathlib.Path(filename)
    copyrights = []

    if "boards" in path.parts:
        related_path = path.parent / "board.c"
        if related_path.is_file():
            with open(related_path, "r") as f:
                for line in f.readlines():
                    match = SPDX_COPYRIGHT_RE.search(line)
                    if match:
                        copyrights.append(f"// {match.group(1)}")
                        continue
                    match = ORIGINAL_COPYRIGHT_RE.search(line)
                    if match:
                        copyrights.append(f"// SPDX-FileCopyrightText: {match.group(1)}")

    return copyrights


def fix_file(filename, change):
    copyrights = []
    mit_license = False
    empty_file = False
    first_line = ""
    no_existing_header = False

    with open(filename, "r") as f:
        lines = f.readlines()
        if not lines:
            empty_file = True
            no_existing_header = True
            mit_license = True
            copyrights.append(
                f"// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC"
            )
        else:
            first_line = lines.pop(0)

            if first_line.startswith("// SPDX"):
                return "already SPDX"

            if first_line.startswith("// This file is part of the CircuitPython"):
                return "already converted to CircuitPython header"

            if not first_line == "/*\n":
                no_existing_header = True
                mit_license = True

                # Put back the line we just read, and add a blank line to separate the header as well.
                lines.insert(0, first_line)
                lines.insert(0, "\n")

        while lines and not no_existing_header:
            line = lines.pop(0)

            if not line and not no_existing_header:
                return "Could not find '*/'"

            # Check that it's MIT-licensed
            match = MIT_LICENSE_RE.search(line)
            if match:
                mit_license = True
                continue

            # If there's an SPDX copyright, just use it. There may be more than one
            match = SPDX_COPYRIGHT_RE.search(line)
            if match:
                copyrights.append(f"// {match.group(1)}")
                continue

            # If it's a non-SPDX copyright, use the copyright text and put it in an SPDX-FileCopyrightText.
            match = ORIGINAL_COPYRIGHT_RE.search(line)
            if match:
                copyrights.append(f"// SPDX-FileCopyrightText: {match.group(1)}")
                continue

            if line.strip() == "*/":
                break

        if not mit_license and not no_existing_header:
            return "No MIT license"
        if not copyrights:
            copyrights = find_related_copyrights(filename)
            if not copyrights:
                copyrights.append(
                    f"// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC"
                )

        if change:
            with open(filename, "w") as f:
                f.write(
                    "// This file is part of the CircuitPython project: https://circuitpython.org\n//\n"
                )
                for copyright in copyrights:
                    f.write(copyright)
                    f.write("\n")
                f.write("//\n// SPDX-License-Identifier: MIT\n")

                # Copy the rest of the file.
                for line in lines:
                    f.write(line)
        return None


@click.command()
@click.option("--change/--no-change", default=False, help="update the file, or only do a dry run")
@click.argument("files", type=click.Path(exists=True, dir_okay=False, writable=True), nargs=-1)
def main(change, files):
    if not change:
        print("Dry-run only. No changes being made. Use --change to change files")
    for filename in files:
        print(filename, end="")
        error = fix_file(filename, change)
        if error:
            print(" :", error)
        else:
            print()


if __name__ == "__main__":
    main()

#! /usr/bin/env python3

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import argparse

import os
import os.path
import sys
import re
from string import Template

parser = argparse.ArgumentParser(description="Apply #define values to .template.ld file.")
parser.add_argument(
    "template_files",
    metavar="TEMPLATE_FILE",
    type=argparse.FileType("r"),
    nargs="+",
    help="template filename: <something>.template.ld",
)
parser.add_argument("--defines", type=argparse.FileType("r"), required=True)
parser.add_argument("--out_dir", required=True)

args = parser.parse_args()

defines = {}

#
REMOVE_UL_RE = re.compile("([0-9]+)UL")


def remove_UL(s):
    return REMOVE_UL_RE.sub(r"\1", s)


# We skip all lines before
# // START_LD_DEFINES
# Then we look for lines like this:
# /*NAME_OF_VALUE=*/ NAME_OF_VALUE;
VALUE_LINE_RE = re.compile(r"^/\*\s*(\w+)\s*=\*/\s*(.*);\s*$")

start_processing = False
for line in args.defines:
    line = line.strip()
    if line == "// START_LD_DEFINES":
        start_processing = True
        continue
    if start_processing:
        match = VALUE_LINE_RE.match(line)
        if match:
            name = match.group(1)
            value = match.group(2).strip()
            defines[name] = remove_UL(value)

fail = False

for template_file in args.template_files:
    ld_template_basename = os.path.basename(template_file.name)
    ld_pathname = os.path.join(args.out_dir, ld_template_basename.replace(".template.ld", ".ld"))
    with open(ld_pathname, "w") as output:
        for k, v in defines.items():
            print("/*", k, "=", v, "*/", file=output)
        print(file=output)
        try:
            output.write(Template(template_file.read()).substitute(defines))
        except KeyError as e:
            print("ERROR: {}: No #define for '{}'".format(ld_pathname, e.args[0]), file=sys.stderr)
            fail = True

if fail:
    sys.exit(1)

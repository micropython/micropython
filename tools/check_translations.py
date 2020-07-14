# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

# Validate that all entries in the .pot are in every .po. Only the .pot is updated so we can detect
# if a translation was added to the source but isn't in a .po. This ensures translators can grab
# complete files to work on.

import sys
import polib


template_filename = sys.argv[1]
po_filenames = sys.argv[2:]

template = polib.pofile(template_filename)
all_ids = set([x.msgid for x in template])
for po_filename in po_filenames:
    print("Checking", po_filename)
    po_file = polib.pofile(po_filename)
    po_ids = set([x.msgid for x in po_file])

    missing = all_ids - po_ids
    if missing:
        print("Missing message id. Please run `make translate`")
        print(missing)
        sys.exit(-1)
    else:
        print("ok")

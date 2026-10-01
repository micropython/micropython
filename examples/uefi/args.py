# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# args.py — prints sys.argv, to show script-with-arguments launching.
#
# From the UEFI shell (not the REPL):
#     FS0:\EFI\BOOT\BOOTAA64.EFI FS0:\samples\args.py hello world
# runs this file and exits, just like `python args.py hello world`.

import sys

print("argv:", sys.argv)
print("got", len(sys.argv) - 1, "argument(s)")

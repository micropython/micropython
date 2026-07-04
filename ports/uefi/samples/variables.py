# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
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

# variables.py — read, write and enumerate UEFI NVRAM variables.
#   exec(open('/samples/variables.py').read())

import uefi
from uefi import variable as V

# Enumerate the well-known global namespace (BootOrder, PlatformLang, ...).
print("Global variables:")
for name, _g in V.enumerate(uefi.guid.GLOBAL_VARIABLE):
    print("  ", name)

# Read a couple of common ones.
for name in ("PlatformLang", "BootOrder"):
    try:
        print("%s = %r" % (name, V.get(name, uefi.guid.GLOBAL_VARIABLE)))
    except uefi.Error as e:
        print("%s : %s" % (name, e.name))

# Create / read / delete a *volatile* test variable (no flash wear, gone at reset).
NS = uefi.GUID("11112222-3333-4444-5555-666677778888")
V.set("hello", NS, b"world", V.BOOTSERVICE_ACCESS)
print("\nwrote hello =", V.get("hello", NS))
V.delete("hello", NS)
print("deleted, exists =", V.exists("hello", NS))

# Variable Policy is usually locked by the time an app runs — check gracefully.
print(
    "\nVariable Policy available:",
    V.VariablePolicy.available(),
    "enabled:",
    V.VariablePolicy.available() and V.VariablePolicy.is_enabled(),
)

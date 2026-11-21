# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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

# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# devices.py — walk the UEFI handle database, like the shell's `dh` / `drivers`.
#   exec(open('/samples/devices.py').read())

import uefi
from uefi import handle, guid

handles = handle.all_handles()
print("%d handles, %d distinct protocols\n" % (len(handles), len(handle.all_protocols())))

# Show the first handles and the (named) protocols on each. guid.name_for() gives a
# readable label without needing the firmware's Component Name protocols.
for h in handles[:15]:
    names = [guid.name_for(g) or str(g) for g in h.protocols()]
    print("%#x: %s" % (h.ptr, ", ".join(names)))

# Handles that expose a Driver Binding protocol are the loaded drivers.
drivers = handle.locate_handles(guid.DRIVER_BINDING_PROTOCOL)
print("\n%d drivers (handles with a Driver Binding protocol)" % len(drivers))

# Who is driving the block devices? open_info() shows BY_DRIVER / BY_CHILD usage.
print("\nBlock devices and their consumers:")
for h in handle.locate_handles(guid.BLOCK_IO_PROTOCOL):
    for info in h.open_info(guid.BLOCK_IO_PROTOCOL):
        if info.by_driver or info.by_child:
            kind = "driver" if info.by_driver else "child"
            print("  %#x  <-%s-  agent %r" % (h.ptr, kind, info.agent))

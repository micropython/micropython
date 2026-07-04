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

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

# hotplug.py — watch for a protocol being installed (device hot-plug).
#   exec(open('/samples/hotplug.py').read())
#
# RegisterProtocolNotify fires whenever a new instance of a protocol appears — the
# usual signal that a device of that class was plugged in. Here we self-trigger it
# by installing a dummy protocol, so the demo is self-contained. To watch real
# hot-plug, use e.g. uefi.guid.BLOCK_IO_PROTOCOL and attach a device in QEMU.

import uefi
from uefi import event, guid

TEST = uefi.GUID("aabbccdd-1122-3344-5566-77889900aabb")

watch = event.ProtocolWatch(TEST)
print("watching for", TEST)
print("present before:", watch.poll())

# Install the protocol on a fresh handle with a dummy interface pointer -> fires it.
iface = uefi.PoolBuffer(16)
st, h = uefi.raw.install_protocol_interface(0, TEST.bytes, iface.ptr)
print("installed on handle %#x (status %d)" % (h, st))

for hnd in watch.poll():
    names = [guid.name_for(g) or str(g) for g in hnd.protocols()]
    print("  arrived:", hnd, "->", names)

# Clean up.
uefi.raw.uninstall_protocol_interface(h, TEST.bytes, iface.ptr)
iface.close()
watch.close()
print("done")

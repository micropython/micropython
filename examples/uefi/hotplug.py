# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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

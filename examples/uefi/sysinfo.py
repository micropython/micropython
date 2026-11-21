# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# sysinfo.py — a quick tour of the running firmware and this app.
#   exec(open('/samples/sysinfo.py').read())

import uefi
import gc
from uefi import protocols as P, system

print("Firmware :", system.firmware_vendor(), "(rev 0x%08x)" % system.firmware_revision())

heap = gc.mem_alloc() + gc.mem_free()
print("MPY heap : %d MiB (%d free)" % (heap // (1024 * 1024), gc.mem_free()))

# Where did we boot from? A protocol opens on a bare EFI_HANDLE, and the volume
# handle it reports back is a bare EFI_HANDLE too — both go straight where needed.
li = P.LOADED_IMAGE.open(uefi.raw.image_handle())
volume = li.device_handle
li.close()
print("Boot vol :", uefi.Handle(volume).device_path().to_text())

# Handle-database size + display mode.
print(
    "Handles  :", len(uefi.handle.all_handles()), "  protocols:", len(uefi.handle.all_protocols())
)
try:
    gop = P.GOP.locate()
    info = gop.struct_at("mode", P.GOP_MODE).struct_at("info", P.GOP_INFO)
    print("Display  : %dx%d" % (info.horizontal_resolution, info.vertical_resolution))
except uefi.Error:
    print("Display  : no GOP (run 'make run-gfx' for graphics)")

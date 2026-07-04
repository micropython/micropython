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

# sysinfo.py — a quick tour of the running firmware and this app.
#   exec(open('/samples/sysinfo.py').read())

import uefi
import uctypes
import gc
from uefi import protocols as P

# Firmware vendor + revision live near the top of the EFI System Table.
st = uefi.raw.system_table_ptr()
hdr = uctypes.struct(
    st, {"vendor": uctypes.UINT64 | 24, "rev": uctypes.UINT32 | 32}, uctypes.LITTLE_ENDIAN
)
print("Firmware :", uefi.raw.wstr_at(hdr.vendor), "(rev 0x%08x)" % hdr.rev)

heap = gc.mem_alloc() + gc.mem_free()
print("MPY heap : %d MiB (%d free)" % (heap // (1024 * 1024), gc.mem_free()))

# Where did we boot from?
img = uefi.Handle(uefi.raw.image_handle())
li = P.LOADED_IMAGE.open(img)
dev = uefi.Handle(li.device_handle)
li.close()
print("Boot vol :", dev.device_path().to_text())

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

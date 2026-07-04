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

# paths.py — parse and format UEFI device paths.
#   exec(open('/samples/paths.py').read())

import uefi
from uefi import device_path as dp, protocols as P

img = uefi.Handle(uefi.raw.image_handle())
li = P.LOADED_IMAGE.open(img)

# This image's own file path (a Media/FilePath node -> a filename).
if li.file_path:
    print("This image :", dp.DevicePath.from_ptr(li.file_path).file_path())

dev = uefi.Handle(li.device_handle)
li.close()

# The boot volume's full device path.
path = dev.device_path()
print("Boot volume:", path.to_text())
print("Nodes:")
for node in path:
    print("  type=0x%02x subtype=0x%02x  (%d bytes)" % (node.type, node.subtype, len(node.data)))

pci = path.pci_node()
if pci:
    print("PCI device/function:", pci)

# Round-trip through the firmware's text converters.
text = path.to_text()
again = dp.DevicePath.from_text(text)
print("from_text -> to_text matches:", again.to_text() == text)

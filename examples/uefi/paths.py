# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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

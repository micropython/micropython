# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# paths.py — parse and format UEFI device paths.
#   exec(open('/samples/paths.py').read())

import uefi
from uefi import protocols as P

li = P.LOADED_IMAGE.open(uefi.raw.image_handle())

# This image's own file path. LoadedImage gives it as a raw pointer, which is one
# of the four device-path spellings as_device_path() accepts.
if li.file_path:
    print("This image :", uefi.as_device_path(li.file_path).file_path())

volume = li.device_handle
li.close()

# The boot volume's full device path.
path = uefi.Handle(volume).device_path()
print("Boot volume:", path.to_text())
print("Nodes:")
for node in path:
    print("  type=0x%02x subtype=0x%02x  (%d bytes)" % (node.type, node.subtype, len(node.data)))

pci = path.pci_node()
if pci:
    print("PCI device/function:", pci)

# A device path travels in four interchangeable spellings — the object, its packed
# bytes, its text, or a firmware pointer. as_device_path() takes any of them, so a
# path from one API drops straight into the next without conversion.
text = path.to_text()
packed = path.to_bytes()
for label, form in (("object", path), ("bytes", packed), ("text", text)):
    print("as_device_path(%-6s) -> %s" % (label, uefi.as_device_path(form).to_text() == text))

# And anywhere a device path is taken, those spellings are equally welcome.
print("LoadOption from text  :", uefi.LoadOption("Demo", text).device_path.to_bytes() == packed)

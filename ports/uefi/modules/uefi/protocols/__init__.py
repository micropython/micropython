# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.protocols — pre-baked ProtocolDescriptors for common UEFI protocols.
#
# Each submodule defines one (or more) ProtocolDescriptor with the interface's
# field/method layout. uctypes computes the offsets for the target; method calls
# route through the raw.call trampoline. Import the descriptor you need:
#
#     from uefi.protocols import LOADED_IMAGE, BLOCK_IO, BLOCK_IO_MEDIA, GOP
#     li = LOADED_IMAGE.open(some_handle)

from .loaded_image import LOADED_IMAGE
from .block_io import BLOCK_IO, BLOCK_IO_MEDIA
from .simple_fs import SIMPLE_FS
from .gop import GOP, GOP_MODE, GOP_INFO
from .pci_io import PCI_IO
from .serial_io import SERIAL_IO, SERIAL_IO_MODE
from .timestamp import TIMESTAMP
from .component_name import COMPONENT_NAME, COMPONENT_NAME2

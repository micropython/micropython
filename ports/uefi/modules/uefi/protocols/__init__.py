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

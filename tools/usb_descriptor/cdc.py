from . import core
import struct

class FunctionalDescriptor(core.Descriptor):
    bDescriptorType = 0x24

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fmt = "<BBB" + self.fmt[3:]

    def __bytes__(self):
        return struct.pack(self.fmt, self.bLength, self.bDescriptorType, self.bDescriptorSubtype, *self.data)


class Header(FunctionalDescriptor):
    fields = [('bcdCDC', "H", None)]
    bLength = 0x05
    bDescriptorSubtype = 0x0


class CallManagement(FunctionalDescriptor):
    fields = [('bmCapabilities', "b", None),
              ('bDataInterface', "b", None)]
    bLength = 0x05
    bDescriptorSubtype = 0x01


class AbstractControlManagement(FunctionalDescriptor):
    fields = [('bmCapabilities', "b", None)]
    bLength = 0x04
    bDescriptorSubtype = 0x02


class DirectLineManagement(FunctionalDescriptor):
    fields = [('bmCapabilities', "b", None)]
    bLength = 0x04
    bDescriptorSubtype = 0x03


class Union(FunctionalDescriptor):
    fields = [('bMasterInterface', "b", None)]
    bDescriptorSubtype = 0x06

    def __init__(self, *args, **kwargs):
        self.bSlaveInterface = kwargs["bSlaveInterface"]
        super().__init__(*args, **kwargs)

    def __bytes__(self):
        return super().__bytes__() + bytes(self.bSlaveInterface)

    @property
    def bLength(self):
        return 0x4 + len(self.bSlaveInterface)

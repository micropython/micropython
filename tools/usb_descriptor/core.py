import struct


def join_interfaces(*args):
    interfaces = []
    base_endpoint_number = 1
    for interface_set in args:
        base_interface_number = len(interfaces)
        for i, interface in enumerate(interface_set):
            interfaces.append(interface)
            interface.bInterfaceNumber = interfaces.index(interface)
            max_endpoint_address = base_endpoint_number
            for subdescriptor in interface.subdescriptors:
                if subdescriptor.bDescriptorType == EndpointDescriptor.bDescriptorType:
                    subdescriptor.bEndpointAddress += base_endpoint_number
                    max_endpoint_address = max(max_endpoint_address, subdescriptor.bEndpointAddress & 0xf)
            base_endpoint_number = max_endpoint_address + 1
    return interfaces


class Descriptor:
    def __init__(self, *args, **kwargs):
        self.fmt = ["<B", "B"]
        for field in self.fields:
            self.fmt.append(field[1])
        self.fmt = "".join(self.fmt)
        if len(args) == 1:
            self.data = struct.unpack(self.fmt, args[0])
            if self.data[1] != self.bDescriptorType:
                raise RuntimeError("Descriptor type doesn't match.")
            return
        elif len(args) > 1:
            raise TypeError("Only one arg or keyword args expected.")
        elif len(kwargs) == 0:
            raise TypeError("Only one arg or keyword args expected.")

        self.data = []
        for field, _, default in self.fields:
            if field in kwargs:
                self.data.append(kwargs[field])
            elif default is not None:
                self.data.append(default)
            else:
                raise ValueError("Missing {} argument.".format(field))

    def __bytes__(self):
        return struct.pack(self.fmt, self.bLength, self.bDescriptorType, *self.data)

    @property
    def bDescriptorType(self):
        return self._bDescriptorType

class EndpointDescriptor(Descriptor):
    fields = [('bEndpointAddress', "B", None),
              ('bmAttributes', "B", None),
              ('wMaxPacketSize', "H", 0x40),
              ('bInterval', "B", 0)]

    bLength = 0x07
    bDescriptorType = 0x5

    TYPE_CONTROL = 0b00
    TYPE_ISOCHRONOUS = 0b01
    TYPE_BULK = 0b10
    TYPE_INTERRUPT = 0b11

    DIRECTION_IN =  0x80
    DIRECTION_OUT = 0x00

    @property
    def bEndpointAddress(self):
        return self.data[0]

    @bEndpointAddress.setter
    def bEndpointAddress(self, value):
        self.data[0] = value


class InterfaceDescriptor(Descriptor):
    fields = [('bInterfaceNumber', "B", 0),
              ('bAlternateSetting', "B", 0),
              ('bNumEndpoints', "B", 0),
              ('bInterfaceClass', "B", None),
              ('bInterfaceSubClass', "B", 0),
              ('bInterfaceProtocol', "B", 0),
              ('iInterface', "B", 0)]

    bLength = 0x09
    bDescriptorType = 0x4

    def __init__(self, *args, **kwargs):
        self.subdescriptors = []
        if "subdescriptors" in kwargs:
            self.subdescriptors = kwargs["subdescriptors"]
        super().__init__(*args, **kwargs)

    def __bytes__(self):
        endpoint_count = 0
        subdescriptor_bytes = []
        for desc in self.subdescriptors:
            subdescriptor_bytes.append(bytes(desc))
            if desc.bDescriptorType == EndpointDescriptor.bDescriptorType:
                endpoint_count += 1
        subdescriptor_bytes = b"".join(subdescriptor_bytes)
        self.data[2] = endpoint_count
        return super().__bytes__() + subdescriptor_bytes

    @property
    def bInterfaceNumber(self):
        return self.data[0]

    @bInterfaceNumber.setter
    def bInterfaceNumber(self, value):
        self.data[0] = value

class InterfaceAssociationDescriptor(Descriptor):
    fields = [('bFirstInterface', "B", None),
              ('bInterfaceCount', "B", None),
              ('bFunctionClass', "B", None),
              ('bFunctionSubClass', "B", None),
              ('bFunctionProtocol', "B", None),
              ('iFunction', "B", 0)]

    bLength = 0x08
    bDescriptorType = 0xB


class ConfigurationDescriptor(Descriptor):
    fields = [('wTotalLength', "H", None),
              ('bNumInterfaces', "B", None),
              ('bConfigurationValue', "B", 0x1),
              ('iConfiguration', "B", 0),
              # bus powered (bit 6), no remote wakeup (bit 5), bit 7 is always 1 and 0-4 are always 0
              ('bmAttributes', "B", 0x80),
              # 100 mA by default
              ('bMaxPower', "B", 50)]

    bLength = 0x09
    bDescriptorType = 0x2

class DeviceDescriptor(Descriptor):
    fields = [('bcdUSB', "H", 0x200),
              ('bDeviceClass', "B", 0xef),
              ('bDeviceSubClass', "B", 0x02),
              ('bDeviceProtocol', "B", 0x01),
              ('bMaxPacketSize0', "B", 0x40),
              ('idVendor', "H", None),
              ('idProduct', "H", None),
              ('bcdDevice', "H", 0x100),
              ('iManufacturer', "B", None),
              ('iProduct', "B", None),
              ('iSerialNumber', "B", None),
              ('bNumConfigurations', "B", 1)]

    bLength = 0x12
    bDescriptorType = 0x1

class StringDescriptor:
    def __init__(self, value):
        if type(value) == str:
            self._bString = value.encode("utf-16-le")
            self._bLength = len(self._bString) + 2
        elif len(value) > 1:
            self._bLength = value[0]
            if value[1] != 3:
                raise ValueError("Sequence not a StringDescriptor")
            self._bString = value[2:2+self.bLength]

    def __bytes__(self):
        return struct.pack("BB{}s".format(len(self._bString)), self.bLength, self.bDescriptorType, self._bString)

    @property
    def bString(self):
        return self._bString.decode("utf-16-le")

    @bString.setter
    def bString(self, value):
        self._bString = value.encode("utf-16-le")
        self._bLength = len(self.encoded) + 2

    @property
    def bDescriptorType(self):
        return 3

    @property
    def bLength(self):
        return self._bLength

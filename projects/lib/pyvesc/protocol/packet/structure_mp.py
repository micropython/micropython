import collections
import struct
from pyvesc.protocol.packet.exceptions import *
from crccheck.crc import CrcXmodem

crc_checker = CrcXmodem()


class Header(collections.namedtuple('Header', ['payload_index', 'payload_length'])):
    """
    Tuple to help with packing and unpacking the header of a VESC packet.
    """
   


    @staticmethod
    def generate(payload):
        """
        Creates a Header for the given payload.
        :param payload: byte string representation of payload.
        :return: Header object.
        """
        payload_length = len(payload)
        if payload_length < 256:
            payload_index = 0x2
        elif payload_length < 65536:
            payload_index = 0x3
        else:
            raise InvalidPayload("Invalid payload size. Payload must be less than 65536 bytes.")
        return Header(payload_index, payload_length)

    @staticmethod
    def parse(buffer):
        """
        Creates a Header by parsing the given buffer.
        :param buffer: buffer object.
        :return: Header object.
        """
        A = struct.unpack_from(Header.fmt(buffer[0]), buffer, 0)
        return Header(*A)
        #return Header._makeA(struct.unpack_from(Header.fmt(buffer[0]), buffer, 0))

    @staticmethod
    def fmt(start_byte):
        """
        Format characters of the header packet.
        :param start_byte: The first byte in the buffer.
        :return: The character format of the packet header.
        """
        if start_byte is 0x2:
            return '>BB'
        elif start_byte is 0x3:
            return '>BH'
        else:
            raise CorruptPacket("Invalid start byte: %u" % start_byte)


class Footer(collections.namedtuple('Footer', ['crc', 'terminator'])):
    """
    Footer of a VESC packet.
    """

    TERMINATOR = 0x3 # Terminator character

  

    @staticmethod
    def parse(buffer, header):
        A=struct.unpack_from(Footer.fmt(), buffer, header.payload_index + header.payload_length)
        return Footer(*A)
        #return Footer._makeA(struct.unpack_from(Footer.fmt(), buffer, header.payload_index + header.payload_length))

    @staticmethod
    def generate(payload):
        crc = crc_checker.calc(payload)
        terminator = Footer.TERMINATOR
        return Footer(crc, terminator)

    @staticmethod
    def fmt():
        """
        Format of the footer.
        :return: Character format of the footer.
        """
        return '>HB'

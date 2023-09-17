import pyvesc.protocol.base_mp
import pyvesc.protocol.packet.codec


def decode_mp(my_msg,buffer):
    """
    Decodes the next valid VESC message in a buffer.

    :param buffer: The buffer to attempt to parse from.
    :type buffer: bytes

    :return: PyVESC message, number of bytes consumed in the buffer. If nothing
             was parsed returns (None, 0).
    :rtype: `tuple`: (PyVESC message, int)
    """
    msg_payload, consumed = pyvesc.protocol.packet.codec.unframe(buffer)
    if msg_payload:
        #return pyvesc.protocol.base_mp.VESCMessage.unpack(msg_payload), consumed
        return my_msg.unpack(msg_payload), consumed
    else:
        return None, consumed


def encode_mp(msg):
    """
    Encodes a PyVESC message to a packet. This packet is a valid VESC packet and
    can be sent to a VESC via your serial port.

    :param msg: Message to be encoded. All fields must be initialized.
    :type msg: PyVESC message

    :return: The packet.
    :rtype: bytes
    """
    msg_payload = pyvesc.protocol.base_mp.VESCMessage.pack(msg)
    packet = pyvesc.protocol.packet.codec.frame(msg_payload)
    return packet


def encode_request_mp(msg_cls):
    """
    Encodes a PyVESC message for requesting a getter message. This function
    should be called when you want to request a VESC to return a getter
    message.

    :param msg_cls: The message type which you are requesting.
    :type msg_cls: pyvesc.messages.getters.[requested getter]

    :return: The encoded PyVESC message which can be sent.
    :rtype: bytes
    """
   
    msg_payload = pyvesc.protocol.base_mp.VESCMessage.pack(msg_cls, header_only=True)
    packet = pyvesc.protocol.packet.codec.frame(msg_payload)
    return packet

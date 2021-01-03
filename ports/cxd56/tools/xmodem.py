'''
===============================
 XMODEM file transfer protocol
===============================

.. $Id$

This is a literal implementation of XMODEM.TXT_, XMODEM1K.TXT_ and
XMODMCRC.TXT_, support for YMODEM and ZMODEM is pending. YMODEM should
be fairly easy to implement as it is a hack on top of the XMODEM
protocol using sequence bytes ``0x00`` for sending file names (and some
meta data).

.. _XMODEM.TXT: doc/XMODEM.TXT
.. _XMODEM1K.TXT: doc/XMODEM1K.TXT
.. _XMODMCRC.TXT: doc/XMODMCRC.TXT

Data flow example including error recovery
==========================================

Here is a sample of the data flow, sending a 3-block message.
It includes the two most common line hits - a garbaged block,
and an ``ACK`` reply getting garbaged. ``CRC`` or ``CSUM`` represents
the checksum bytes.

XMODEM 128 byte blocks
----------------------

::

    SENDER                                      RECEIVER

                                            <-- NAK
    SOH 01 FE Data[128] CSUM                -->
                                            <-- ACK
    SOH 02 FD Data[128] CSUM                -->
                                            <-- ACK
    SOH 03 FC Data[128] CSUM                -->
                                            <-- ACK
    SOH 04 FB Data[128] CSUM                -->
                                            <-- ACK
    SOH 05 FA Data[100] CPMEOF[28] CSUM     -->
                                            <-- ACK
    EOT                                     -->
                                            <-- ACK

XMODEM-1k blocks, CRC mode
--------------------------

::

    SENDER                                      RECEIVER

                                            <-- C
    STX 01 FE Data[1024] CRC CRC            -->
                                            <-- ACK
    STX 02 FD Data[1024] CRC CRC            -->
                                            <-- ACK
    STX 03 FC Data[1000] CPMEOF[24] CRC CRC -->
                                            <-- ACK
    EOT                                     -->
                                            <-- ACK

Mixed 1024 and 128 byte Blocks
------------------------------

::

    SENDER                                      RECEIVER

                                            <-- C
    STX 01 FE Data[1024] CRC CRC            -->
                                            <-- ACK
    STX 02 FD Data[1024] CRC CRC            -->
                                            <-- ACK
    SOH 03 FC Data[128] CRC CRC             -->
                                            <-- ACK
    SOH 04 FB Data[100] CPMEOF[28] CRC CRC  -->
                                            <-- ACK
    EOT                                     -->
                                            <-- ACK

YMODEM Batch Transmission Session (1 file)
------------------------------------------

::

    SENDER                                      RECEIVER
                                            <-- C (command:rb)
    SOH 00 FF foo.c NUL[123] CRC CRC        -->
                                            <-- ACK
                                            <-- C
    SOH 01 FE Data[128] CRC CRC             -->
                                            <-- ACK
    SOH 02 FC Data[128] CRC CRC             -->
                                            <-- ACK
    SOH 03 FB Data[100] CPMEOF[28] CRC CRC  -->
                                            <-- ACK
    EOT                                     -->
                                            <-- NAK
    EOT                                     -->
                                            <-- ACK
                                            <-- C
    SOH 00 FF NUL[128] CRC CRC              -->
                                            <-- ACK


'''

__author__ = 'Wijnand Modderman <maze@pyth0n.org>'
__copyright__ = ['Copyright (c) 2010 Wijnand Modderman',
                 'Copyright (c) 1981 Chuck Forsberg']
__license__ = 'MIT'
__version__ = '0.3.2'

import logging
import time
import sys
from functools import partial
import collections

# Loggerr
log = logging.getLogger('xmodem')

# Protocol bytes
SOH = bytes([0x01])
STX = bytes([0x02])
EOT = bytes([0x04])
ACK = bytes([0x06])
DLE = bytes([0x10])
NAK = bytes([0x15])
CAN = bytes([0x18])
CRC = bytes([0x43]) # C


class XMODEM(object):
    '''
    XMODEM Protocol handler, expects an object to read from and an object to
    write to.

    >>> def getc(size, timeout=1):
    ...     return data or None
    ...
    >>> def putc(data, timeout=1):
    ...     return size or None
    ...
    >>> modem = XMODEM(getc, putc)


    :param getc: Function to retreive bytes from a stream
    :type getc: callable
    :param putc: Function to transmit bytes to a stream
    :type putc: callable
    :param mode: XMODEM protocol mode
    :type mode: string
    :param pad: Padding character to make the packets match the packet size
    :type pad: char

    '''

    # crctab calculated by Mark G. Mendel, Network Systems Corporation
    crctable = [
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
    ]

    def __init__(self, getc, putc, mode='xmodem', pad=b'\x1a'):
        self.getc = getc
        self.putc = putc
        self.mode = mode
        self.pad = pad

    def abort(self, count=2, timeout=60):
        '''
        Send an abort sequence using CAN bytes.
        '''
        for counter in range(0, count):
            self.putc(CAN, timeout)

    def send(self, stream, retry=32, timeout=360, quiet=0, callback=None):
        '''
        Send a stream via the XMODEM protocol.

            >>> stream = file('/etc/issue', 'rb')
            >>> print modem.send(stream)
            True

        Returns ``True`` upon succesful transmission or ``False`` in case of
        failure.

        :param stream: The stream object to send data from.
        :type stream: stream (file, etc.)
        :param retry: The maximum number of times to try to resend a failed
                      packet before failing.
        :type retry: int
        :param timeout: The number of seconds to wait for a response before
                        timing out.
        :type timeout: int
        :param quiet: If 0, it prints info to stderr.  If 1, it does not print any info.
        :type quiet: int
        :param callback: Reference to a callback function that has the
                         following signature.  This is useful for
                         getting status updates while a xmodem
                         transfer is underway.
                         Expected callback signature:
                         def callback(total_packets, success_count, error_count)
        :type callback: callable
        '''

        # initialize protocol
        try:
            packet_size = dict(
                xmodem    = 128,
                xmodem1k  = 1024,
            )[self.mode]
        except AttributeError:
            raise ValueError("An invalid mode was supplied")

        error_count = 0
        crc_mode = 0
        cancel = 0
        while True:
            char = self.getc(1)
            if char:
                if char == NAK:
                    crc_mode = 0
                    break
                elif char == CRC:
                    crc_mode = 1
                    break
                elif char == CAN:
                    if not quiet:
                        print('received CAN', file=sys.stderr)
                    if cancel:
                        return False
                    else:
                        cancel = 1
                else:
                    log.error('send ERROR expected NAK/CRC, got %s' % \
                        (ord(char),))

            error_count += 1
            if error_count >= retry:
                self.abort(timeout=timeout)
                return False

        # send data
        error_count = 0
        success_count = 0
        total_packets = 0
        sequence = 1
        while True:
            data = stream.read(packet_size)
            if not data:
                log.info('sending EOT')
                # end of stream
                break
            total_packets += 1
            data = data.ljust(packet_size, self.pad)
            if crc_mode:
                crc = self.calc_crc(data)
            else:
                crc = self.calc_checksum(data)

            # emit packet
            while True:
                if packet_size == 128:
                    self.putc(SOH)
                else:  # packet_size == 1024
                    self.putc(STX)
                self.putc(bytes([sequence]))
                self.putc(bytes([0xff - sequence]))
                self.putc(data)
                if crc_mode:
                    self.putc(bytes([crc >> 8]))
                    self.putc(bytes([crc & 0xff]))
                else:
                    self.putc(bytes([crc]))

                char = self.getc(1, timeout)
                if char == ACK:
                    success_count += 1
                    if isinstance(callback, collections.Callable):
                        callback(total_packets, success_count, error_count)
                    break
                if char == NAK:
                    error_count += 1
                    if isinstance(callback, collections.Callable):
                        callback(total_packets, success_count, error_count)
                    if error_count >= retry:
                        # excessive amounts of retransmissions requested,
                        # abort transfer
                        self.abort(timeout=timeout)
                        log.warning('excessive NAKs, transfer aborted')
                        return False

                    # return to loop and resend
                    continue
                else:
                    log.error('Not ACK, Not NAK')
                    error_count += 1
                    if isinstance(callback, collections.Callable):
                        callback(total_packets, success_count, error_count)
                    if error_count >= retry:
                        # excessive amounts of retransmissions requested,
                        # abort transfer
                        self.abort(timeout=timeout)
                        log.warning('excessive protocol errors, transfer aborted')
                        return False

                    # return to loop and resend
                    continue

                # protocol error
                self.abort(timeout=timeout)
                log.error('protocol error')
                return False

            # keep track of sequence
            sequence = (sequence + 1) % 0x100

        while True:
            # end of transmission
            self.putc(EOT)

            #An ACK should be returned
            char = self.getc(1, timeout)
            if char == ACK:
                break
            else:
                error_count += 1
                if error_count >= retry:
                    self.abort(timeout=timeout)
                    log.warning('EOT was not ACKd, transfer aborted')
                    return False

        return True

    def recv(self, stream, crc_mode=1, retry=16, timeout=60, delay=1, quiet=0):
        '''
        Receive a stream via the XMODEM protocol.

            >>> stream = file('/etc/issue', 'wb')
            >>> print modem.recv(stream)
            2342

        Returns the number of bytes received on success or ``None`` in case of
        failure.
        '''

        # initiate protocol
        error_count = 0
        char = 0
        cancel = 0
        while True:
            # first try CRC mode, if this fails,
            # fall back to checksum mode
            if error_count >= retry:
                self.abort(timeout=timeout)
                return None
            elif crc_mode and error_count < (retry / 2):
                if not self.putc(CRC):
                    time.sleep(delay)
                    error_count += 1
            else:
                crc_mode = 0
                if not self.putc(NAK):
                    time.sleep(delay)
                    error_count += 1

            char = self.getc(1, timeout)
            if not char:
                error_count += 1
                continue
            elif char == SOH:
                #crc_mode = 0
                break
            elif char == STX:
                break
            elif char == CAN:
                if cancel:
                    return None
                else:
                    cancel = 1
            else:
                error_count += 1

        # read data
        error_count = 0
        income_size = 0
        packet_size = 128
        sequence = 1
        cancel = 0
        while True:
            while True:
                if char == SOH:
                    packet_size = 128
                    break
                elif char == STX:
                    packet_size = 1024
                    break
                elif char == EOT:
                    # We received an EOT, so send an ACK and return the received
                    # data length
                    self.putc(ACK)
                    return income_size
                elif char == CAN:
                    # cancel at two consecutive cancels
                    if cancel:
                        return None
                    else:
                        cancel = 1
                else:
                    if not quiet:
                        print('recv ERROR expected SOH/EOT, got', ord(char), file=sys.stderr)
                    error_count += 1
                    if error_count >= retry:
                        self.abort()
                        return None
            # read sequence
            error_count = 0
            cancel = 0
            seq1 = ord(self.getc(1))
            seq2 = 0xff - ord(self.getc(1))
            if seq1 == sequence and seq2 == sequence:
                # sequence is ok, read packet
                # packet_size + checksum
                data = self.getc(packet_size + 1 + crc_mode, timeout)
                if crc_mode:
                    csum = (ord(data[-2]) << 8) + ord(data[-1])
                    data = data[:-2]
                    log.debug('CRC (%04x <> %04x)' % \
                        (csum, self.calc_crc(data)))
                    valid = csum == self.calc_crc(data)
                else:
                    csum = data[-1]
                    data = data[:-1]
                    log.debug('checksum (checksum(%02x <> %02x)' % \
                        (ord(csum), self.calc_checksum(data)))
                    valid = ord(csum) == self.calc_checksum(data)

                # valid data, append chunk
                if valid:
                    income_size += len(data)
                    stream.write(data)
                    self.putc(ACK)
                    sequence = (sequence + 1) % 0x100
                    char = self.getc(1, timeout)
                    continue
            else:
                # consume data
                self.getc(packet_size + 1 + crc_mode)
                self.debug('expecting sequence %d, got %d/%d' % \
                    (sequence, seq1, seq2))

            # something went wrong, request retransmission
            self.putc(NAK)

    def calc_checksum(self, data, checksum=0):
        '''
        Calculate the checksum for a given block of data, can also be used to
        update a checksum.

            >>> csum = modem.calc_checksum('hello')
            >>> csum = modem.calc_checksum('world', csum)
            >>> hex(csum)
            '0x3c'

        '''
        return (sum(map(ord, data)) + checksum) % 256

    def calc_crc(self, data, crc=0):
        '''
        Calculate the Cyclic Redundancy Check for a given block of data, can
        also be used to update a CRC.

            >>> crc = modem.calc_crc('hello')
            >>> crc = modem.calc_crc('world', crc)
            >>> hex(crc)
            '0xd5e3'

        '''
        for char in data:
            crc = (crc << 8) ^ self.crctable[((crc >> 8) ^ int(char)) & 0xff]
        return crc & 0xffff


XMODEM1k  = partial(XMODEM, mode='xmodem1k')


def run():
    import optparse
    import subprocess

    parser = optparse.OptionParser(usage='%prog [<options>] <send|recv> filename filename')
    parser.add_option('-m', '--mode', default='xmodem',
        help='XMODEM mode (xmodem, xmodem1k)')

    options, args = parser.parse_args()
    if len(args) != 3:
        parser.error('invalid arguments')
        return 1

    elif args[0] not in ('send', 'recv'):
        parser.error('invalid mode')
        return 1

    def _func(so, si):
        import select
        import subprocess

        print('si', si)
        print('so', so)

        def getc(size, timeout=3):
            w,t,f = select.select([so], [], [], timeout)
            if w:
                data = so.read(size)
            else:
                data = None

            print('getc(', repr(data), ')')
            return data

        def putc(data, timeout=3):
            w,t,f = select.select([], [si], [], timeout)
            if t:
                si.write(data)
                si.flush()
                size = len(data)
            else:
                size = None

            print('putc(', repr(data), repr(size), ')')
            return size

        return getc, putc

    def _pipe(*command):
        pipe = subprocess.Popen(command,
            stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        return pipe.stdout, pipe.stdin

    if args[0] == 'recv':
        import io
        getc, putc = _func(*_pipe('sz', '--xmodem', args[2]))
        stream = open(args[1], 'wb')
        xmodem = XMODEM(getc, putc, mode=options.mode)
        status = xmodem.recv(stream, retry=8)
        stream.close()

    elif args[0] == 'send':
        getc, putc = _func(*_pipe('rz', '--xmodem', args[2]))
        stream = open(args[1], 'rb')
        xmodem = XMODEM(getc, putc, mode=options.mode)
        status = xmodem.send(stream, retry=8)
        stream.close()

if __name__ == '__main__':
    sys.exit(run())

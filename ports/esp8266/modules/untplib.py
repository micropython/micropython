###############################################################################
# The MIT License (MIT)
#
# Copyright (C) 2009-2015 Charles-Francois Natali <cf.natali@gmail.com>
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
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
###############################################################################

#############################################
# Changed to run in MicroPython on the WiPy
# by Andrew Hedges
# 24th Oct 2015
#############################################

"""Python NTP library.

Implementation of client-side NTP (RFC-1305), and useful NTP-related
functions.
"""


#import datetime
import socket
import struct
import time


class NTPException(Exception):
    """Exception raised by this module."""
    pass


class NTP:
    """Helper class defining constants."""

    #_SYSTEM_EPOCH = datetime.date(*time.gmtime(0)[0:3])
    """system epoch"""
    #_NTP_EPOCH = datetime.date(1900, 1, 1)
    """NTP epoch"""
    #NTP_DELTA = (_SYSTEM_EPOCH - _NTP_EPOCH).days * 24 * 3600
    NTP_DELTA = 3155673600
    """delta between system and NTP time"""

    REF_ID_TABLE = {
        "GOES":  "Geostationary Orbit Environment Satellite",
        "GPS\0": "Global Position System",
        "GAL\0": "Galileo Positioning System",
        "PPS\0": "Generic pulse-per-second",
        "IRIG":  "Inter-Range Instrumentation Group",
        "WWVB":  "LF Radio WWVB Ft. Collins, CO 60 kHz",
        "DCF\0": "LF Radio DCF77 Mainflingen, DE 77.5 kHz",
        "HBG\0": "LF Radio HBG Prangins, HB 75 kHz",
        "MSF\0": "LF Radio MSF Anthorn, UK 60 kHz",
        "JJY\0": "LF Radio JJY Fukushima, JP 40 kHz, Saga, JP 60 kHz",
        "LORC":  "MF Radio LORAN C station, 100 kHz",
        "TDF\0": "MF Radio Allouis, FR 162 kHz",
        "CHU\0": "HF Radio CHU Ottawa, Ontario",
        "WWV\0": "HF Radio WWV Ft. Collins, CO",
        "WWVH":  "HF Radio WWVH Kauai, HI",
        "NIST":  "NIST telephone modem",
        "ACTS":  "NIST telephone modem",
        "USNO":  "USNO telephone modem",
        "PTB\0": "European telephone modem",
        "LOCL":  "uncalibrated local clock",
        "CESM":  "calibrated Cesium clock",
        "RBDM":  "calibrated Rubidium clock",
        "OMEG":  "OMEGA radionavigation system",
        "DCN\0": "DCN routing protocol",
        "TSP\0": "TSP time protocol",
        "DTS\0": "Digital Time Service",
        "ATOM":  "Atomic clock (calibrated)",
        "VLF\0": "VLF radio (OMEGA,, etc.)",
        "1PPS":  "External 1 PPS input",
        "FREE":  "(Internal clock)",
        "INIT":  "(Initialization)",
        "\0\0\0\0":   "NULL",
    }
    """reference identifier table"""

    STRATUM_TABLE = {
        0: "unspecified or invalid",
        1: "primary reference (%s)",
    }
    """stratum table"""

    MODE_TABLE = {
        0: "reserved",
        1: "symmetric active",
        2: "symmetric passive",
        3: "client",
        4: "server",
        5: "broadcast",
        6: "reserved for NTP control messages",
        7: "reserved for private use",
    }
    """mode table"""

    LEAP_TABLE = {
        0: "no warning",
        1: "last minute of the day has 61 seconds",
        2: "last minute of the day has 59 seconds",
        3: "unknown (clock unsynchronized)",
    }
    """leap indicator table"""


class NTPPacket:
    """NTP packet class.

    This represents an NTP packet.
    """

    _PACKET_FORMAT = "!BBBbIIIIIIIIIII"
    """packet format to pack/unpack"""

    def __init__(self, version=2, mode=3, tx_timestamp=0):
        """Constructor.

        Parameters:
        version      -- NTP version
        mode         -- packet mode (client, server)
        tx_timestamp -- packet transmit timestamp
        """
        self.leap = 0
        """leap second indicator"""
        self.version = version
        """version"""
        self.mode = mode
        """mode"""
        self.stratum = 0
        """stratum"""
        self.poll = 0
        """poll interval"""
        self.precision = 0
        """precision"""
        self.root_delay = 0
        """root delay"""
        self.root_dispersion = 0
        """root dispersion"""
        self.ref_id = 0
        """reference clock identifier"""
        self.ref_timestamp = 0
        """reference timestamp"""
        self.orig_timestamp = 0
        """originate timestamp"""
        self.recv_timestamp = 0
        """receive timestamp"""
        self.tx_timestamp = tx_timestamp
        """tansmit timestamp"""

    def to_data(self):
        """Convert this NTPPacket to a buffer that can be sent over a socket.

        Returns:
        buffer representing this packet

        Raises:
        NTPException -- in case of invalid field
        """
        try:
            packed = struct.pack(NTPPacket._PACKET_FORMAT,
                (self.leap << 6 | self.version << 3 | self.mode),
                self.stratum,
                self.poll,
                self.precision,
                _to_int(self.root_delay) << 16 | _to_frac(self.root_delay, 16),
                _to_int(self.root_dispersion) << 16 |
                _to_frac(self.root_dispersion, 16),
                self.ref_id,
                _to_int(self.ref_timestamp),
                _to_frac(self.ref_timestamp),
                _to_int(self.orig_timestamp),
                _to_frac(self.orig_timestamp),
                _to_int(self.recv_timestamp),
                _to_frac(self.recv_timestamp),
                _to_int(self.tx_timestamp),
                _to_frac(self.tx_timestamp))
        except struct.error:
            raise NTPException("Invalid NTP packet fields.")
        return packed

    def from_data(self, data):
        """Populate this instance from a NTP packet payload received from
        the network.

        Parameters:
        data -- buffer payload

        Raises:
        NTPException -- in case of invalid packet format
        """
        try:
            unpacked = struct.unpack(NTPPacket._PACKET_FORMAT,
                    data[0:struct.calcsize(NTPPacket._PACKET_FORMAT)])
        except struct.error:
            raise NTPException("Invalid NTP packet.")

        self.leap = unpacked[0] >> 6 & 0x3
        self.version = unpacked[0] >> 3 & 0x7
        self.mode = unpacked[0] & 0x7
        self.stratum = unpacked[1]
        self.poll = unpacked[2]
        self.precision = unpacked[3]
        self.root_delay = (unpacked[4])//2**16
        self.root_dispersion = (unpacked[5])//2**16
        self.ref_id = unpacked[6]
        self.ref_timestamp = _to_time(unpacked[7], unpacked[8])
        self.orig_timestamp = _to_time(unpacked[9], unpacked[10])
        self.recv_timestamp = _to_time(unpacked[11], unpacked[12])
        self.tx_timestamp = _to_time(unpacked[13], unpacked[14])


class NTPStats(NTPPacket):
    """NTP statistics.

    Wrapper for NTPPacket, offering additional statistics like offset and
    delay, and timestamps converted to system time.
    """

    def __init__(self):
        """Constructor."""
        NTPPacket.__init__(self)
        self.dest_timestamp = 0
        """destination timestamp"""

    @property
    def offset(self):
        """offset"""
        return ((self.recv_timestamp - self.orig_timestamp) +
                (self.tx_timestamp - self.dest_timestamp))//2

    @property
    def delay(self):
        """round-trip delay"""
        return ((self.dest_timestamp - self.orig_timestamp) -
                (self.tx_timestamp - self.recv_timestamp))

    @property
    def tx_time(self):
        """Transmit timestamp in system time."""
        return ntp_to_system_time(self.tx_timestamp)

    @property
    def recv_time(self):
        """Receive timestamp in system time."""
        return ntp_to_system_time(self.recv_timestamp)

    @property
    def orig_time(self):
        """Originate timestamp in system time."""
        return ntp_to_system_time(self.orig_timestamp)

    @property
    def ref_time(self):
        """Reference timestamp in system time."""
        return ntp_to_system_time(self.ref_timestamp)

    @property
    def dest_time(self):
        """Destination timestamp in system time."""
        return ntp_to_system_time(self.dest_timestamp)


class NTPClient:
    """NTP client session."""

    def __init__(self):
        """Constructor."""
        pass

    def request(self, host, version=2, port=123, timeout=5):
        """Query a NTP server.

        Parameters:
        host    -- server name/address
        version -- NTP version to use
        port    -- server port
        timeout -- timeout on socket operations

        Returns:
        NTPStats object
        """
        # lookup server address
        addrinfo = socket.getaddrinfo(host, port)[0]
        family, sockaddr = addrinfo[0], addrinfo[4]

        # create the socket
        s = socket.socket(family, socket.SOCK_DGRAM)

        try:
            s.settimeout(timeout)

            # create the request packet - mode 3 is client
            query_packet = NTPPacket(mode=3, version=version,
                                tx_timestamp=system_to_ntp_time(time.time()))

            # send the request
            s.sendto(query_packet.to_data(), sockaddr)

            # wait for the response - check the source address
            src_addr = None,
            while src_addr[0] != sockaddr[0]:
                response_packet, src_addr = s.recvfrom(256)

            # build the destination timestamp
            dest_timestamp = system_to_ntp_time(time.time())
        except socket.timeout:
            raise NTPException("No response received from %s." % host)
        finally:
            s.close()

        # construct corresponding statistics
        stats = NTPStats()
        stats.from_data(response_packet)
        stats.dest_timestamp = dest_timestamp

        return stats


def _to_int(timestamp):
    """Return the integral part of a timestamp.

    Parameters:
    timestamp -- NTP timestamp

    Retuns:
    integral part
    """
    return timestamp
    #return int(timestamp)


def _to_frac(timestamp, n=32):
    """Return the fractional part of a timestamp.

    Parameters:
    timestamp -- NTP timestamp
    n         -- number of bits of the fractional part

    Retuns:
    fractional part
    """
    return 0
    #return int(abs(timestamp - _to_int(timestamp)) * 2**n)


def _to_time(integ, frac, n=32):
    """Return a timestamp from an integral and fractional part.

    Parameters:
    integ -- integral part
    frac  -- fractional part
    n     -- number of bits of the fractional part

    Retuns:
    timestamp
    """
    return integ
    #return integ + float(frac)/2**n


def ntp_to_system_time(timestamp):
    """Convert a NTP time to system time.

    Parameters:
    timestamp -- timestamp in NTP time

    Returns:
    corresponding system time
    """
    return timestamp - NTP.NTP_DELTA


def system_to_ntp_time(timestamp):
    """Convert a system time to a NTP time.

    Parameters:
    timestamp -- timestamp in system time

    Returns:
    corresponding NTP time
    """
    return timestamp + NTP.NTP_DELTA


def leap_to_text(leap):
    """Convert a leap indicator to text.

    Parameters:
    leap -- leap indicator value

    Returns:
    corresponding message

    Raises:
    NTPException -- in case of invalid leap indicator
    """
    if leap in NTP.LEAP_TABLE:
        return NTP.LEAP_TABLE[leap]
    else:
        raise NTPException("Invalid leap indicator.")


def mode_to_text(mode):
    """Convert a NTP mode value to text.

    Parameters:
    mode -- NTP mode

    Returns:
    corresponding message

    Raises:
    NTPException -- in case of invalid mode
    """
    if mode in NTP.MODE_TABLE:
        return NTP.MODE_TABLE[mode]
    else:
        raise NTPException("Invalid mode.")


def stratum_to_text(stratum):
    """Convert a stratum value to text.

    Parameters:
    stratum -- NTP stratum

    Returns:
    corresponding message

    Raises:
    NTPException -- in case of invalid stratum
    """
    if stratum in NTP.STRATUM_TABLE:
        return NTP.STRATUM_TABLE[stratum] % (stratum)
    elif 1 < stratum < 16:
        return "secondary reference (%s)" % (stratum)
    elif stratum == 16:
        return "unsynchronized (%s)" % (stratum)
    else:
        raise NTPException("Invalid stratum or reserved.")


def ref_id_to_text(ref_id, stratum=2):
    """Convert a reference clock identifier to text according to its stratum.

    Parameters:
    ref_id  -- reference clock indentifier
    stratum -- NTP stratum

    Returns:
    corresponding message

    Raises:
    NTPException -- in case of invalid stratum
    """
    fields = (ref_id >> 24 & 0xff, ref_id >> 16 & 0xff,
              ref_id >> 8 & 0xff, ref_id & 0xff)

    # return the result as a string or dot-formatted IP address
    if 0 <= stratum <= 1:
        text = '%c%c%c%c' % fields
        if text in NTP.REF_ID_TABLE:
            return NTP.REF_ID_TABLE[text]
        else:
            return "Unidentified reference source '%s'" % (text)
    elif 2 <= stratum < 255:
        return '%d.%d.%d.%d' % fields
    else:
        raise NTPException("Invalid stratum.")

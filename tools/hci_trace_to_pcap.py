#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2023 Jim Mussared
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

# Simple script to take the output printed when HCI_TRACE is enabled in
# unix/mpbthciport.c, extmod/btstack/btstack_hci_uart.c,
# extmod/nimble/hal/hal_uart.c and turn it into a .pcap file suitable for use
# with Wireshark.

import re
import sys
import struct

# pcap file header:
# typedef struct pcap_hdr_s {
#     guint32 magic_number;   /* magic number */
#     guint16 version_major;  /* major version number */
#     guint16 version_minor;  /* minor version number */
#     gint32  thiszone;       /* GMT to local correction */
#     guint32 sigfigs;        /* accuracy of timestamps */
#     guint32 snaplen;        /* max length of captured packets, in octets */
#     guint32 network;        /* data link type */
# } pcap_hdr_t;

# pcap record header
# typedef struct pcaprec_hdr_s {
#     guint32 ts_sec;         /* timestamp seconds */
#     guint32 ts_usec;        /* timestamp microseconds */
#     guint32 incl_len;       /* number of octets of packet saved in file */
#     guint32 orig_len;       /* actual length of packet */
# } pcaprec_hdr_t;

_LINKTYPE_BLUETOOTH_HCI_H4_WITH_PHDR = 201  # "!I" direction, followed by data
sys.stdout.buffer.write(
    struct.pack("!IHHiIII", 0xA1B2C3D4, 2, 4, 0, 0, 65535, _LINKTYPE_BLUETOOTH_HCI_H4_WITH_PHDR)
)

_DIR_CONTROLLER_TO_HOST = 1
_DIR_HOST_TO_CONTROLLER = 0

reassemble_timestamp = 0
reassemble_packet = bytearray()

with open(sys.argv[1], "r") as f:
    for line in f:
        line = line.strip()
        m = re.match("([<>]) \\[ *([0-9]+)\\] ([A-Fa-f0-9:]+)", line)
        if not m:
            continue

        timestamp = int(m.group(2))
        data = bytes.fromhex(m.group(3).replace(":", ""))

        if m.group(1) == "<":
            # Host to controller.
            # These are always complete.
            sys.stdout.buffer.write(
                struct.pack(
                    "!IIIII",
                    timestamp // 1000,
                    timestamp % 1000 * 1000,
                    len(data) + 4,
                    len(data) + 4,
                    _DIR_HOST_TO_CONTROLLER,
                )
            )
            sys.stdout.buffer.write(data)
        if m.group(1) == ">":
            # Controller to host.
            # Several of the sources print byte-at-a-time so need to reconstruct packets.

            if not reassemble_packet:
                # Use the timestamp of the first fragment.
                reassemble_timestamp = timestamp

            reassemble_packet.extend(data)

            if len(reassemble_packet) > 4:
                plen = 0
                if reassemble_packet[0] == 1:
                    # command
                    plen = 3 + reassemble_packet[3]
                elif reassemble_packet[0] == 2:
                    # acl
                    plen = 5 + reassemble_packet[3] + (reassemble_packet[4] << 8)
                elif reassemble_packet[0] == 4:
                    # event
                    plen = 3 + reassemble_packet[2]

                if len(reassemble_packet) >= plen:
                    # Got a complete packet.
                    data = reassemble_packet[0:plen]
                    reassemble_packet = reassemble_packet[plen:]
                    reassemble_timestamp = timestamp
                    sys.stdout.buffer.write(
                        struct.pack(
                            "!IIIII",
                            reassemble_timestamp // 1000,
                            reassemble_timestamp % 1000 * 1000,
                            len(data) + 4,
                            len(data) + 4,
                            _DIR_CONTROLLER_TO_HOST,
                        )
                    )
                    sys.stdout.buffer.write(data)

if reassemble_packet:
    print(
        "Error: Unknown byte in HCI stream. Remainder:",
        reassemble_packet.hex(":"),
        file=sys.stderr,
    )

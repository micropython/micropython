#
# @file main.py
#
# @section License
# Copyright (C) 2016, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the Pumbaa project.
#

import select
import socket
import harness
from harness import assert_raises


def test_print():
    print(socket)


def test_tcp_client():
    client = socket.socket()
    client.connect(("192.168.1.101", 80))
    assert client.send(b'foo') == 3
    assert client.recv(3) == b'bar'
    client.close()


def test_tcp_server():
    listener = socket.socket()
    listener.bind(("192.168.1.102", 8080))
    listener.listen(5)
    listener.accept()
    listener.close()


def test_udp():
    socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


def test_select():
    poll = select.poll()
    tcp = socket.socket()

    # Register both event channels.
    poll.register(tcp)

    # Timeout waiting for data on the socket.
    assert poll.poll(0.01) == []

    tcp.close()


def test_bad_arguments():
    # Bad socket family.
    with assert_raises(OSError):
        socket.socket(-1)

        # Bad socket type.
    with assert_raises(OSError):
        socket.socket(socket.AF_INET, -1)


def main():
    testcases = [
        (test_print, "test_print"),
        (test_tcp_client, "test_tcp_client"),
        (test_tcp_server, "test_tcp_server"),
        (test_udp, "test_udp"),
        (test_select, "test_select"),
        (test_bad_arguments, "test_bad_arguments")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()

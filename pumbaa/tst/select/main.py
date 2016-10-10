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
from sync import Event, Queue
import harness
from harness import assert_raises


def test_help():
    poll = select.poll()
    help(select)
    help(poll)


def test_register_unregister():
    poll = select.poll()
    queue = Queue()
    event = Event()

    poll.register(queue)
    poll.register(event)

    poll.unregister(queue)
    poll.unregister(event)

    with assert_raises(OSError):
        poll.unregister(queue)


def test_poll():
    poll = select.poll()
    queue = Queue()
    event = Event()

    # Register both event channels.
    poll.register(queue)
    poll.register(event)

    # Timeout waiting for event.
    assert poll.poll(0.01) == []

    # Event write, poll and read.
    event.write(0x1)
    assert poll.poll() == [(event, select.POLLIN)]
    assert event.read(0x1) == 0x1

    # Queue write, poll and read.
    queue.write(b'foo')
    assert poll.poll() == [(queue, select.POLLIN)]
    assert queue.read(3) == b'foo'


def test_bad_arguments():
    poll = select.poll()

    with assert_raises(TypeError, "channel object required"):
        poll.register(None)

    with assert_raises(OSError):
        poll.unregister(None)


def main():
    testcases = [
        (test_help, "test_help"),
        (test_register_unregister, "test_register_unregister"),
        (test_poll, "test_poll"),
        (test_bad_arguments, "test_bad_arguments")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()

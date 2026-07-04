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

# uefi.event — Python ergonomics over the C uefi.Event: timers and hot-plug.
#
# uefi.Event (the C type) is a pollable EVT_NOTIFY_SIGNAL wrapper: its notify sets
# a sticky flag and wakes the idle loop, it reports MP_STREAM_POLL_RD (so it works
# with select / asyncio), and exposes the raw EFI_EVENT as `.ptr`. This module adds:
#   * Timer  — an EVT_TIMER event with periodic/oneshot/async helpers.
#   * ProtocolWatch — device hot-plug via RegisterProtocolNotify.
#   * wait_async(ev) — suspend an asyncio task until an Event/Timer is signalled.

from _uefi import Event
from . import raw
from . import status
from .guid import GUID
from .handle import Handle

# EFI_TIMER_DELAY values for raw.set_timer.
_TIMER_CANCEL = 0
_TIMER_PERIODIC = 1
_TIMER_RELATIVE = 2
_MS_TO_100NS = 10000  # 1 ms = 10000 * 100 ns
_BY_REGISTER_NOTIFY = 1


async def wait_async(ev):
    """Suspend the current asyncio task until `ev` (a uefi.Event) is signalled.

    Yields to the scheduler via the asyncio I/O queue — no busy-poll, no
    WaitForEvent — because uefi.Event is a pollable stream.
    """
    if not ev.is_signaled():
        from asyncio import core

        yield core._io_queue.queue_read(ev)


class Timer:
    """A UEFI timer built on an EVT_TIMER event — pollable and awaitable."""

    def __init__(self):
        self._event = Event(timer=True)

    def set_periodic(self, interval_ms):
        status.check(
            raw.set_timer(self._event.ptr, _TIMER_PERIODIC, int(interval_ms) * _MS_TO_100NS)
        )

    def set_oneshot(self, delay_ms):
        status.check(raw.set_timer(self._event.ptr, _TIMER_RELATIVE, int(delay_ms) * _MS_TO_100NS))

    def cancel(self):
        if self._event.ptr:
            status.check(raw.set_timer(self._event.ptr, _TIMER_CANCEL, 0))

    def is_signaled(self):
        return self._event.is_signaled()

    def clear(self):
        self._event.clear()

    def wait(self, timeout_ms=None):
        return self._event.wait(timeout_ms)

    async def wait_async(self):
        await wait_async(self._event)

    async def sleep_async(self, delay_ms):
        self.clear()
        self.set_oneshot(delay_ms)
        await wait_async(self._event)
        self.clear()

    def close(self):
        self._event.close()  # CloseEvent cancels a pending timer

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()


class ProtocolWatch:
    """Fires whenever a new instance of `guid` is installed — i.e. device hot-plug.

    Async-iterate it to receive arriving Handles, or poll() for the batch that has
    arrived since the last call.
    """

    def __init__(self, guid):
        self._guid = GUID(guid)
        self._event = Event()
        st, reg = raw.register_protocol_notify(self._guid.bytes, self._event.ptr)
        status.check(st)
        self._registration = reg
        self._pending = []

    def poll(self):
        """Return the Handles that have (re)installed `guid` since the last poll."""
        # Clear first: an install racing our drain re-signals the flag, so we won't
        # miss it (the handle is queued in the registration regardless).
        self._event.clear()
        out = []
        while True:
            st, h = raw.locate_handle_notify(self._registration)
            if st == status.NOT_FOUND:
                break
            status.check(st)
            out.append(Handle(h))
        return out

    def __aiter__(self):
        return self

    async def __anext__(self):
        while not self._pending:
            self._pending = self.poll()
            if not self._pending:
                await wait_async(self._event)
        return self._pending.pop(0)

    def close(self):
        self._event.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

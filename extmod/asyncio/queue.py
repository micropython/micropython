# MicroPython asyncio module
# MIT license; Copyright (c) 2026 Matt Trentini
#
# Queue design based on Peter Hinch's asyncio primitives library
# (https://github.com/peterhinch/micropython-async), itself based on
# Paul Sokolovsky's original uasyncio Queue.

from .event import Event


class QueueEmpty(Exception):
    pass


class QueueFull(Exception):
    pass


# FIFO queue, for producer/consumer coordination between tasks.
class Queue:
    def __init__(self, maxsize=0):
        self.maxsize = maxsize
        self._queue = []
        self._unfinished = 0
        self._evget = Event()  # Set (then cleared) each time an item is got (queue was full)
        self._evput = Event()  # Set (then cleared) each time an item is put (queue was empty)
        self._evjoin = Event()  # Set whenever there are no unfinished items
        self._evjoin.set()

    def qsize(self):
        return len(self._queue)

    def empty(self):
        return len(self._queue) == 0

    def full(self):
        return self.maxsize > 0 and len(self._queue) >= self.maxsize

    def _get(self):
        v = self._queue.pop(0)
        self._evget.set()
        self._evget.clear()
        return v

    def get_nowait(self):
        if self.empty():
            raise QueueEmpty
        return self._get()

    async def get(self):
        while self.empty():
            await self._evput.wait()
        return self._get()

    def _put(self, v):
        self._queue.append(v)
        self._unfinished += 1
        self._evjoin.clear()
        self._evput.set()
        self._evput.clear()

    def put_nowait(self, v):
        if self.full():
            raise QueueFull
        self._put(v)

    async def put(self, v):
        while self.full():
            await self._evget.wait()
        self._put(v)

    def task_done(self):
        if self._unfinished <= 0:
            raise ValueError("task_done() called too many times")
        self._unfinished -= 1
        if self._unfinished == 0:
            self._evjoin.set()

    async def join(self):
        await self._evjoin.wait()

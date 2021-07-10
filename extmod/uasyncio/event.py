# MicroPython uasyncio module
# MIT license; Copyright (c) 2019-2020 Damien P. George

from . import core

# Event class for primitive events that can be waited on, set, and cleared
class Event:
    def __init__(self):
        self.state = False  # False=unset; True=set
        self.waiting = core.TaskQueue()  # Queue of Tasks waiting on completion of this event

    def is_set(self):
        return self.state

    def set(self):
        # Event becomes set, schedule any tasks waiting on it
        # Note: This must not be called from anything except the thread running
        # the asyncio loop (i.e. neither hard or soft IRQ, or a different thread).
        while self.waiting.peek():
            core._task_queue.push_head(self.waiting.pop_head())
        self.state = True

    def clear(self):
        self.state = False

    async def wait(self):
        if not self.state:
            # Event not set, put the calling task on the event's waiting queue
            self.waiting.push_head(core.cur_task)
            # Set calling task's data to the event's queue so it can be removed if needed
            core.cur_task.data = self.waiting
            yield
        return True


# MicroPython-extension: This can be set from outside the asyncio event loop,
# such as other threads, IRQs or scheduler context. Implementation is a stream
# that asyncio will poll until a flag is set.
# Note: Unlike Event, this is self-clearing.
try:
    import uio

    class ThreadSafeFlag(uio.IOBase):
        def __init__(self):
            self._flag = 0

        def ioctl(self, req, flags):
            if req == 3:  # MP_STREAM_POLL
                return self._flag * flags
            return None

        def set(self):
            self._flag = 1

        async def wait(self):
            if not self._flag:
                yield core._io_queue.queue_read(self)
            self._flag = 0


except ImportError:
    pass

# MicroPython uasyncio module
# MIT license; Copyright (c) 2019-2020 Damien P. George

from . import core


# Lock class for primitive mutex capability
class Lock:
    def __init__(self):
        # The state can take the following values:
        # - 0: unlocked
        # - 1: locked
        # - <Task>: unlocked but this task has been scheduled to acquire the lock next
        self.state = 0
        # Queue of Tasks waiting to acquire this Lock
        self.waiting = core.TaskQueue()

    def locked(self):
        return self.state == 1

    def release(self):
        if self.state != 1:
            raise RuntimeError("Lock not acquired")
        if self.waiting.peek():
            # Task(s) waiting on lock, schedule next Task
            self.state = self.waiting.pop()
            core._task_queue.push(self.state)
        else:
            # No Task waiting so unlock
            self.state = 0

    # async
    def acquire(self):
        if self.state != 0:
            # Lock unavailable, put the calling Task on the waiting queue
            self.waiting.push(core.cur_task)
            # Set calling task's data to the lock's queue so it can be removed if needed
            core.cur_task.data = self.waiting
            try:
                yield
            except core.CancelledError as er:
                if self.state == core.cur_task:
                    # Cancelled while pending on resume, schedule next waiting Task
                    self.state = 1
                    self.release()
                raise er
        # Lock available, set it as locked
        self.state = 1
        return True

    async def __aenter__(self):
        return await self.acquire()

    async def __aexit__(self, exc_type, exc, tb):
        return self.release()

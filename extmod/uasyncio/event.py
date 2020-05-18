# MicroPython uasyncio module
# MIT license; Copyright (c) 2019-2020 Damien P. George

from micropython import scheduler_lock, scheduler_unlock
from . import core

# Event class for primitive events that can be waited on, set, and cleared
# The following methods are safe to call from a scheduled callback: is_set, set, clear
class Event:
    def __init__(self):
        self.state = False  # False=unset; True=set
        self.waiting = core.TaskQueue()  # Queue of Tasks waiting on completion of this event

    def is_set(self):
        return self.state

    def set(self):
        if self.state:
            return
        # Event becomes set, schedule any tasks waiting on it
        scheduler_lock()
        signal = False
        while self.waiting.peek():
            core._task_queue.push_head(self.waiting.pop_head())
            signal = True
        if signal:
            # signal poll to finish
            core._io_queue.notify_threadsafe()
        self.state = True
        scheduler_unlock()

    def clear(self):
        self.state = False

    async def wait(self):
        scheduler_lock()
        if not self.state:
            # Event not set, put the calling task on the event's waiting queue
            self.waiting.push_head(core.cur_task)
            # Set calling task's data to the event's queue so it can be removed if needed
            core.cur_task.data = self.waiting
            scheduler_unlock()
            yield
        else:
            scheduler_unlock()
        return True

# MicroPython uasyncio module
# MIT license; Copyright (c) 2019 Damien P. George

from time import ticks_ms as ticks, ticks_diff, ticks_add
import sys, select

# Import TaskQueue and Task, preferring built-in C code over Python code
try:
    from _uasyncio import TaskQueue, Task
except:
    from .task import TaskQueue, Task


################################################################################
# Exceptions


class CancelledError(BaseException):
    pass


class TimeoutError(Exception):
    pass


# Used when calling Loop.call_exception_handler
_exc_context = {"message": "Task exception wasn't retrieved", "exception": None, "future": None}


################################################################################
# Sleep functions

# "Yield" once, then raise StopIteration
class SingletonGenerator:
    def __init__(self):
        self.state = None
        self.exc = StopIteration()

    def __iter__(self):
        return self

    def __next__(self):
        if self.state is not None:
            _task_queue.push_sorted(cur_task, self.state)
            self.state = None
            return None
        else:
            self.exc.__traceback__ = None
            raise self.exc


# Pause task execution for the given time (integer in milliseconds, uPy extension)
# Use a SingletonGenerator to do it without allocating on the heap
def sleep_ms(t, sgen=SingletonGenerator()):
    assert sgen.state is None
    sgen.state = ticks_add(ticks(), max(0, t))
    return sgen


# Pause task execution for the given time (in seconds)
def sleep(t):
    return sleep_ms(int(t * 1000))


################################################################################
# Queue and poller for stream IO


class IOQueue:
    def __init__(self):
        self.poller = select.poll()
        self.map = {}  # maps id(stream) to [task_waiting_read, task_waiting_write, stream]

    def _enqueue(self, s, idx):
        if id(s) not in self.map:
            entry = [None, None, s]
            entry[idx] = cur_task
            self.map[id(s)] = entry
            self.poller.register(s, select.POLLIN if idx == 0 else select.POLLOUT)
        else:
            sm = self.map[id(s)]
            assert sm[idx] is None
            assert sm[1 - idx] is not None
            sm[idx] = cur_task
            self.poller.modify(s, select.POLLIN | select.POLLOUT)
        # Link task to this IOQueue so it can be removed if needed
        cur_task.data = self

    def _dequeue(self, s):
        del self.map[id(s)]
        self.poller.unregister(s)

    def queue_read(self, s):
        self._enqueue(s, 0)

    def queue_write(self, s):
        self._enqueue(s, 1)

    def remove(self, task):
        while True:
            del_s = None
            for k in self.map:  # Iterate without allocating on the heap
                q0, q1, s = self.map[k]
                if q0 is task or q1 is task:
                    del_s = s
                    break
            if del_s is not None:
                self._dequeue(s)
            else:
                break

    def wait_io_event(self, dt):
        for s, ev in self.poller.ipoll(dt):
            sm = self.map[id(s)]
            # print('poll', s, sm, ev)
            if ev & ~select.POLLOUT and sm[0] is not None:
                # POLLIN or error
                _task_queue.push_head(sm[0])
                sm[0] = None
            if ev & ~select.POLLIN and sm[1] is not None:
                # POLLOUT or error
                _task_queue.push_head(sm[1])
                sm[1] = None
            if sm[0] is None and sm[1] is None:
                self._dequeue(s)
            elif sm[0] is None:
                self.poller.modify(s, select.POLLOUT)
            else:
                self.poller.modify(s, select.POLLIN)


################################################################################
# Main run loop

# Ensure the awaitable is a task
def _promote_to_task(aw):
    return aw if isinstance(aw, Task) else create_task(aw)


# Create and schedule a new task from a coroutine
def create_task(coro):
    if not hasattr(coro, "send"):
        raise TypeError("coroutine expected")
    t = Task(coro, globals())
    _task_queue.push_head(t)
    return t


# Keep scheduling tasks until there are none left to schedule
def run_until_complete(main_task=None):
    global cur_task
    excs_all = (CancelledError, Exception)  # To prevent heap allocation in loop
    excs_stop = (CancelledError, StopIteration)  # To prevent heap allocation in loop
    while True:
        # Wait until the head of _task_queue is ready to run
        dt = 1
        while dt > 0:
            dt = -1
            t = _task_queue.peek()
            if t:
                # A task waiting on _task_queue; "ph_key" is time to schedule task at
                dt = max(0, ticks_diff(t.ph_key, ticks()))
            elif not _io_queue.map:
                # No tasks can be woken so finished running
                return
            # print('(poll {})'.format(dt), len(_io_queue.map))
            _io_queue.wait_io_event(dt)

        # Get next task to run and continue it
        t = _task_queue.pop_head()
        cur_task = t
        try:
            # Continue running the coroutine, it's responsible for rescheduling itself
            exc = t.data
            if not exc:
                t.coro.send(None)
            else:
                t.data = None
                t.coro.throw(exc)
        except excs_all as er:
            # Check the task is not on any event queue
            assert t.data is None
            # This task is done, check if it's the main task and then loop should stop
            if t is main_task:
                if isinstance(er, StopIteration):
                    return er.value
                raise er
            # Save return value of coro to pass up to caller
            t.data = er
            # Schedule any other tasks waiting on the completion of this task
            waiting = False
            if hasattr(t, "waiting"):
                while t.waiting.peek():
                    _task_queue.push_head(t.waiting.pop_head())
                    waiting = True
                t.waiting = None  # Free waiting queue head
            # Print out exception for detached tasks
            if not waiting and not isinstance(er, excs_stop):
                _exc_context["exception"] = er
                _exc_context["future"] = t
                Loop.call_exception_handler(_exc_context)
            # Indicate task is done
            t.coro = None


# Create a new task from a coroutine and run it until it finishes
def run(coro):
    return run_until_complete(create_task(coro))


################################################################################
# Event loop wrapper


async def _stopper():
    pass


_stop_task = None


class Loop:
    _exc_handler = None

    def create_task(coro):
        return create_task(coro)

    def run_forever():
        global _stop_task
        _stop_task = Task(_stopper(), globals())
        run_until_complete(_stop_task)
        # TODO should keep running until .stop() is called, even if there're no tasks left

    def run_until_complete(aw):
        return run_until_complete(_promote_to_task(aw))

    def stop():
        global _stop_task
        if _stop_task is not None:
            _task_queue.push_head(_stop_task)
            # If stop() is called again, do nothing
            _stop_task = None

    def close():
        pass

    def set_exception_handler(handler):
        Loop._exc_handler = handler

    def get_exception_handler():
        return Loop._exc_handler

    def default_exception_handler(loop, context):
        print(context["message"])
        print("future:", context["future"], "coro=", context["future"].coro)
        sys.print_exception(context["exception"])

    def call_exception_handler(context):
        (Loop._exc_handler or Loop.default_exception_handler)(Loop, context)


# The runq_len and waitq_len arguments are for legacy uasyncio compatibility
def get_event_loop(runq_len=0, waitq_len=0):
    return Loop


def new_event_loop():
    global _task_queue, _io_queue
    # TaskQueue of Task instances
    _task_queue = TaskQueue()
    # Task queue and poller for stream IO
    _io_queue = IOQueue()
    return Loop


# Initialise default event loop
new_event_loop()

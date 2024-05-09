# MicroPython asyncio module, for use with webassembly port
# MIT license; Copyright (c) 2019-2024 Damien P. George

from time import ticks_ms as ticks, ticks_diff, ticks_add
import sys, js, jsffi

# Import TaskQueue and Task from built-in C code.
from _asyncio import TaskQueue, Task


################################################################################
# Exceptions


class CancelledError(BaseException):
    pass


class TimeoutError(Exception):
    pass


# Used when calling Loop.call_exception_handler.
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
            _task_queue.push(cur_task, self.state)
            self.state = None
            return None
        else:
            self.exc.__traceback__ = None
            raise self.exc


# Pause task execution for the given time (integer in milliseconds, uPy extension)
# Use a SingletonGenerator to do it without allocating on the heap
def sleep_ms(t, sgen=SingletonGenerator()):
    if cur_task is None:
        # Support top-level asyncio.sleep, via a JavaScript Promise.
        return jsffi.async_timeout_ms(t)
    assert sgen.state is None
    sgen.state = ticks_add(ticks(), max(0, t))
    return sgen


# Pause task execution for the given time (in seconds)
def sleep(t):
    return sleep_ms(int(t * 1000))


################################################################################
# Main run loop

asyncio_timer = None


class ThenableEvent:
    def __init__(self, thenable):
        self.result = None  # Result of the thenable
        self.waiting = None  # Task waiting on completion of this thenable
        thenable.then(self.set)

    def set(self, value=None):
        # Thenable/Promise is fulfilled, set result and schedule any waiting task.
        self.result = value
        if self.waiting:
            _task_queue.push(self.waiting)
            self.waiting = None
            _schedule_run_iter(0)

    def remove(self, task):
        self.waiting = None

    # async
    def wait(self):
        # Set the calling task as the task waiting on this thenable.
        self.waiting = cur_task
        # Set calling task's data to this object so it can be removed if needed.
        cur_task.data = self
        # Wait for the thenable to fulfill.
        yield
        # Return the result of the thenable.
        return self.result


# Ensure the awaitable is a task
def _promote_to_task(aw):
    return aw if isinstance(aw, Task) else create_task(aw)


def _schedule_run_iter(dt):
    global asyncio_timer
    if asyncio_timer is not None:
        js.clearTimeout(asyncio_timer)
    asyncio_timer = js.setTimeout(_run_iter, dt)


def _run_iter():
    global cur_task
    excs_all = (CancelledError, Exception)  # To prevent heap allocation in loop
    excs_stop = (CancelledError, StopIteration)  # To prevent heap allocation in loop
    while True:
        # Wait until the head of _task_queue is ready to run
        t = _task_queue.peek()
        if t:
            # A task waiting on _task_queue; "ph_key" is time to schedule task at
            dt = max(0, ticks_diff(t.ph_key, ticks()))
        else:
            # No tasks can be woken so finished running
            cur_task = None
            return

        if dt > 0:
            # schedule to call again later
            cur_task = None
            _schedule_run_iter(dt)
            return

        # Get next task to run and continue it
        t = _task_queue.pop()
        cur_task = t
        try:
            # Continue running the coroutine, it's responsible for rescheduling itself
            exc = t.data
            if not exc:
                t.coro.send(None)
            else:
                # If the task is finished and on the run queue and gets here, then it
                # had an exception and was not await'ed on.  Throwing into it now will
                # raise StopIteration and the code below will catch this and run the
                # call_exception_handler function.
                t.data = None
                t.coro.throw(exc)
        except excs_all as er:
            # Check the task is not on any event queue
            assert t.data is None
            # This task is done.
            if t.state:
                # Task was running but is now finished.
                waiting = False
                if t.state is True:
                    # "None" indicates that the task is complete and not await'ed on (yet).
                    t.state = None
                elif callable(t.state):
                    # The task has a callback registered to be called on completion.
                    t.state(t, er)
                    t.state = False
                    waiting = True
                else:
                    # Schedule any other tasks waiting on the completion of this task.
                    while t.state.peek():
                        _task_queue.push(t.state.pop())
                        waiting = True
                    # "False" indicates that the task is complete and has been await'ed on.
                    t.state = False
                if not waiting and not isinstance(er, excs_stop):
                    # An exception ended this detached task, so queue it for later
                    # execution to handle the uncaught exception if no other task retrieves
                    # the exception in the meantime (this is handled by Task.throw).
                    _task_queue.push(t)
                # Save return value of coro to pass up to caller.
                t.data = er
            elif t.state is None:
                # Task is already finished and nothing await'ed on the task,
                # so call the exception handler.

                # Save exception raised by the coro for later use.
                t.data = exc

                # Create exception context and call the exception handler.
                _exc_context["exception"] = exc
                _exc_context["future"] = t
                Loop.call_exception_handler(_exc_context)


# Create and schedule a new task from a coroutine.
def create_task(coro):
    if not hasattr(coro, "send"):
        raise TypeError("coroutine expected")
    t = Task(coro, globals())
    _task_queue.push(t)
    _schedule_run_iter(0)
    return t


################################################################################
# Event loop wrapper


cur_task = None


class Loop:
    _exc_handler = None

    def create_task(coro):
        return create_task(coro)

    def close():
        pass

    def set_exception_handler(handler):
        Loop._exc_handler = handler

    def get_exception_handler():
        return Loop._exc_handler

    def default_exception_handler(loop, context):
        print(context["message"], file=sys.stderr)
        print("future:", context["future"], "coro=", context["future"].coro, file=sys.stderr)
        sys.print_exception(context["exception"], sys.stderr)

    def call_exception_handler(context):
        (Loop._exc_handler or Loop.default_exception_handler)(Loop, context)


def get_event_loop():
    return Loop


def current_task():
    if cur_task is None:
        raise RuntimeError("no running event loop")
    return cur_task


def new_event_loop():
    global _task_queue
    _task_queue = TaskQueue()  # TaskQueue of Task instances.
    return Loop


# Initialise default event loop.
new_event_loop()

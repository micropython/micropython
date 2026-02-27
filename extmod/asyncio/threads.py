# MicroPython asyncio: threading
# MIT license; Copyright (c) 2025 Matthias Urlichs

from _thread import start_new_thread

from .event import ThreadSafeFlag


class ThreadState:
    Lock = None

    def __init__(self, func, args, kwargs):
        self.func = func
        self.args = args
        self.kwargs = kwargs
        self.done = ThreadSafeFlag()

        self.ret = None
        self.exc = None

    def run(self):
        try:
            self.ret = self.func(*self.args, **self.kwargs)
        except BaseException as exc:
            self.exc = exc
        finally:
            self.done.set()


async def to_thread(func, *args, **kwargs):
    state = ThreadState(func, args, kwargs)
    start_new_thread(state.run, ())
    await state.done.wait()
    if state.exc is not None:
        raise state.exc
    return state.ret

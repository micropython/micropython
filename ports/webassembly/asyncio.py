# Minimal asyncio interface that uses JavaScript for scheduling.
# Only implements a very small subset of the asyncio API.

import js, jsffi


class Event:
    def __init__(self):
        self.clear()

    def _set_callbacks(self, resolve, reject):
        self._resolve = resolve
        self._reject = reject

    def is_set(self):
        return self._promise is None

    def set(self):
        if self._promise is not None:
            self._resolve()
            self._promise = None

    def clear(self):
        self._promise = js.Promise.new(self._set_callbacks)

    async def wait(self):
        if self._promise is not None:
            await self._promise
        return True


class Task:
    def __init__(self, coro):
        self._coro = coro

    def done(self):
        return self._coro is None


async def sleep(s):
    await jsffi.async_timeout(s)


def create_task(coro):
    if not hasattr(coro, "send"):
        raise TypeError("coroutine expected")
    t = Task(coro)
    jsffi.create_task(t)
    return t

# Minimal asyncio interface that uses JavaScript for scheduling.
# Only implements a very small subset of the asyncio API.

import jsffi


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

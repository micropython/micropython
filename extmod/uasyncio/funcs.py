# MicroPython uasyncio module
# MIT license; Copyright (c) 2019-2020 Damien P. George

from . import core


async def wait_for(aw, timeout):
    aw = core._promote_to_task(aw)
    if timeout is None:
        return await aw

    def cancel(aw, timeout):
        await core.sleep(timeout)
        aw.cancel()

    cancel_task = core.create_task(cancel(aw, timeout))
    try:
        ret = await aw
    except core.CancelledError:
        # Ignore CancelledError from aw, it's probably due to timeout
        pass
    finally:
        # Cancel the "cancel" task if it's still active (optimisation instead of cancel_task.cancel())
        if cancel_task.coro is not None:
            core._task_queue.remove(cancel_task)
    if cancel_task.coro is None:
        # Cancel task ran to completion, ie there was a timeout
        raise core.TimeoutError
    return ret


async def gather(*aws, return_exceptions=False):
    ts = [core._promote_to_task(aw) for aw in aws]
    for i in range(len(ts)):
        try:
            # TODO handle cancel of gather itself
            # if ts[i].coro:
            #    iter(ts[i]).waiting.push_head(cur_task)
            #    try:
            #        yield
            #    except CancelledError as er:
            #        # cancel all waiting tasks
            #        raise er
            ts[i] = await ts[i]
        except Exception as er:
            if return_exceptions:
                ts[i] = er
            else:
                raise er
    return ts

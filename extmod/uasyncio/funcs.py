# MicroPython uasyncio module
# MIT license; Copyright (c) 2019-2022 Damien P. George

from . import core


async def _run(waiter, aw):
    try:
        result = await aw
        status = True
    except BaseException as er:
        result = None
        status = er
    if waiter.data is None:
        # The waiter is still waiting, cancel it.
        if waiter.cancel():
            # Waiter was cancelled by us, change its CancelledError to an instance of
            # CancelledError that contains the status and result of waiting on aw.
            # If the wait_for task subsequently gets cancelled externally then this
            # instance will be reset to a CancelledError instance without arguments.
            waiter.data = core.CancelledError(status, result)


async def wait_for(aw, timeout, sleep=core.sleep):
    aw = core._promote_to_task(aw)
    if timeout is None:
        return await aw

    # Run aw in a separate runner task that manages its exceptions.
    runner_task = core.create_task(_run(core.cur_task, aw))

    try:
        # Wait for the timeout to elapse.
        await sleep(timeout)
    except core.CancelledError as er:
        status = er.value
        if status is None:
            # This wait_for was cancelled externally, so cancel aw and re-raise.
            runner_task.cancel()
            raise er
        elif status is True:
            # aw completed successfully and cancelled the sleep, so return aw's result.
            return er.args[1]
        else:
            # aw raised an exception, propagate it out to the caller.
            raise status

    # The sleep finished before aw, so cancel aw and raise TimeoutError.
    runner_task.cancel()
    await runner_task
    raise core.TimeoutError


def wait_for_ms(aw, timeout):
    return wait_for(aw, timeout, core.sleep_ms)


class _Remove:
    @staticmethod
    def remove(t):
        pass


# async
def gather(*aws, return_exceptions=False):
    if not aws:
        return []

    def done(t, er):
        # Sub-task "t" has finished, with exception "er".
        nonlocal state
        if gather_task.data is not _Remove:
            # The main gather task has already been scheduled, so do nothing.
            # This happens if another sub-task already raised an exception and
            # woke the main gather task (via this done function), or if the main
            # gather task was cancelled externally.
            return
        elif not return_exceptions and not isinstance(er, StopIteration):
            # A sub-task raised an exception, indicate that to the gather task.
            state = er
        else:
            state -= 1
            if state:
                # Still some sub-tasks running.
                return
        # Gather waiting is done, schedule the main gather task.
        core._task_queue.push(gather_task)

    ts = [core._promote_to_task(aw) for aw in aws]
    for i in range(len(ts)):
        if ts[i].state is not True:
            # Task is not running, gather not currently supported for this case.
            raise RuntimeError("can't gather")
        # Register the callback to call when the task is done.
        ts[i].state = done

    # Set the state for execution of the gather.
    gather_task = core.cur_task
    state = len(ts)
    cancel_all = False

    # Wait for the a sub-task to need attention.
    gather_task.data = _Remove
    try:
        yield
    except core.CancelledError as er:
        cancel_all = True
        state = er

    # Clean up tasks.
    for i in range(len(ts)):
        if ts[i].state is done:
            # Sub-task is still running, deregister the callback and cancel if needed.
            ts[i].state = True
            if cancel_all:
                ts[i].cancel()
        elif isinstance(ts[i].data, StopIteration):
            # Sub-task ran to completion, get its return value.
            ts[i] = ts[i].data.value
        else:
            # Sub-task had an exception with return_exceptions==True, so get its exception.
            ts[i] = ts[i].data

    # Either this gather was cancelled, or one of the sub-tasks raised an exception with
    # return_exceptions==False, so reraise the exception here.
    if state is not 0:
        raise state

    # Return the list of return values of each sub-task.
    return ts

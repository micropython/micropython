# MicroPython uasyncio module
# MIT license; Copyright (c) 2019-2020 Damien P. George

from . import core


class _Task(core.Task):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.shield = False


def shield(aw):
    aw = core._promote_to_task(aw)

    async def shielded(t):
        try:
            return await t
        finally:
            t.shield = False  # not really needed because task will get removed

    # t = core.create_task(shielded(aw))
    t = _Task(shielded(aw), globals())
    core._task_queue.push_head(t)
    t.shield = True
    return t

"""
    ct - Central Timer
    One timer will be used for all of pyBox system calls, leaving remaining three for custom code
    Author: Wolf Paulus wolf@paulus.com
    MIT license; Copyright (c) 2022 wolfpaulus.com
"""
from micropython import const
from machine import Timer
import pybox_log as log

_TIMER_ID = const(0)
_TICK_MS = const(1)
MIN_FREQ = const(1000 // _TICK_MS * 60 * 60)  # about 1 hour
MAX_FREQ = const(_TICK_MS)


def process(_) -> None:
    """
    this is called every ${frequency}-th of a second and will call every callback that is due.
    :return: None
    """
    global _counter
    _counter = _counter + 1 if _counter < MIN_FREQ else 1  # 1..3_600_000
    for t in _tasks:
        if _counter % t[1] == 0:
            t[0]()


def register(task: ()) -> None:
    """
    Registers a callback and call frequency. The callback should have no parameters
    and the frequency should be in MAX_FREQ..MIN_FREQ range
    :param task: callback,freq
    :return: None
    """
    _tasks.append(task)


def unregister(task: (callable, int)) -> None:
    """
    Unregisters a previously registered task
    :param task: callback,freq
    :return: None
    """
    for i in range(len(_tasks) - 1, -1, -1):
        if _tasks[i][0].__name__ == task[0].__name__:
            _tasks.pop(i)
            break
    else:
        log.log(log.ERROR, f"Could not unregister {task[0].__name__}")


_counter = 0
_tasks = []
_timer = Timer(_TIMER_ID)
_timer.init(mode=Timer.PERIODIC, period=_TICK_MS, callback=process)

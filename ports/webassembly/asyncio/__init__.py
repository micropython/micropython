# MicroPython asyncio module, for use with webassembly port
# MIT license; Copyright (c) 2024 Damien P. George

from .core import *
from .funcs import wait_for, wait_for_ms, gather
from .event import Event
from .lock import Lock

__version__ = (3, 0, 0)

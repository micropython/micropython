"""
categories: Types,enum
description: IntEnum members fail isinstance(x, int) check
cause: IntEnum members are created with object.__new__() rather than as true int subclass instances due to metaclass interaction complexity in MicroPython
workaround: Use int(x) to convert, or check hasattr(x, '_value_') for enum members. All integer operations work correctly through operator forwarding.
"""

from enum import IntEnum


class Status(IntEnum):
    RUNNING = 1
    STOPPED = 2


s = Status.RUNNING
print(isinstance(s, int))  # CPython: True, MicroPython: False
print(isinstance(s, Status))  # Both: True
print(int(s))  # Both: 1
print(s + 1)  # Both: 2 (operators work correctly)

"""
categories: Modules,select
description: ``poll.poll()`` returns the socket object itself rather than its file number, i.e. ``list[tuple[IOBase, int]]`` instead of ``list[tuple[int, int]]``.
cause: Most MicroPython ports don't ordinarily rely on integral fileno values.
workaround: If portability is required, compare poll outputs both to the IO object and its ``.fileno()`` value.
"""

import select, sys

p = select.poll()
p.register(sys.stdout, select.POLLOUT)
print(p.poll(0))

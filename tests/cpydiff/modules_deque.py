"""
categories: Modules,deque
description: Deque not implemented
cause: Unknown
workaround: Use regular lists. micropython-lib has implementation of collections.deque.
"""
import collections
D = collections.deque()
print(D)

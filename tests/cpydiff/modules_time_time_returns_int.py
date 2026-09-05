"""
categories: Modules.time
description: ``time`` returns an integer instead of a float.
cause: Unknown
workaround: use ``int(time())`` if you need a timestamp with just the seconds or ``time_ns()/1_000_000_000`` if you need a float.
"""

import time

print(type(time.time()))

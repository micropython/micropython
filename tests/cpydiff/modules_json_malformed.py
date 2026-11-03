"""
categories: Modules,json
description: JSON module accepts some malformed json
cause: MicroPython is optimised for code space. For more information, see `this issue <https://github.com/micropython/micropython/issues/19581>`_.
workaround: Only accept json from trusted sources.
"""

import json

try:
    print(json.loads("[}"))
except ValueError as e:
    print("ValueError")

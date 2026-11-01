"""
categories: Modules,json
description: JSON module accepts some malformed json
cause: MicroPython is optimised for code space. For more information, see `this issue <https://github.com/micropython/micropython/issues/19581>`_.
workaround: Malformed JSON will not crash the interpreter, but can result in a Python data structure that does not correspond to the JSON text.  Code that relies on ``json.loads`` raising an exception on malformed input data must implement a validation pass on the returned data.
"""

import json

try:
    print(json.loads("[}"))
except ValueError as e:
    print("ValueError")

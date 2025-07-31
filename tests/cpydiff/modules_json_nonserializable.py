"""
categories: Modules,json
description: JSON module does not throw exception when object is not serialisable
cause: Unknown
workaround: Unknown
"""

import json

try:
    print(json.dumps(b"shouldn't be able to serialise bytes"))
except TypeError:
    print("TypeError")

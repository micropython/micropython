"""
categories: Modules,json
description: JSON module does not throw exception when object is not serialisable
cause: Unknown
workaround: Unknown
"""
import json

a = bytes(x for x in range(256))
try:
    z = json.dumps(a)
    x = json.loads(z)
    print("Should not get here")
except TypeError:
    print("TypeError")

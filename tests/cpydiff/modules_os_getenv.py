"""
categories: Modules,os
description: ``getenv`` returns actual value instead of cached value
cause: The ``environ`` attribute is not implemented
workaround: Unknown
"""

import os

print(os.getenv("NEW_VARIABLE"))
os.putenv("NEW_VARIABLE", "VALUE")
print(os.getenv("NEW_VARIABLE"))

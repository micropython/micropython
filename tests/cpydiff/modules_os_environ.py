"""
categories: Modules,os
description: ``environ`` attribute is not implemented
cause: Unknown
workaround: Use ``getenv``, ``putenv`` and ``unsetenv``
"""
import os

try:
    print(os.environ.get("NEW_VARIABLE"))
    os.environ["NEW_VARIABLE"] = "VALUE"
    print(os.environ["NEW_VARIABLE"])
except AttributeError:
    print("should not get here")
    print(os.getenv("NEW_VARIABLE"))
    os.putenv("NEW_VARIABLE", "VALUE")
    print(os.getenv("NEW_VARIABLE"))

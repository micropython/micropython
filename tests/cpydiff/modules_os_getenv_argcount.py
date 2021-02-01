"""
categories: Modules,os
description: ``getenv`` only allows one argument
cause: Unknown
workaround: Test that the return value is ``None``
"""
import os

try:
    print(os.getenv("NEW_VARIABLE", "DEFAULT"))
except TypeError:
    print("should not get here")
    # this assumes NEW_VARIABLE is never an empty variable
    print(os.getenv("NEW_VARIABLE") or "DEFAULT")

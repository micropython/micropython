"""
categories: Core,import
description: Failed to load modules are still registered as loaded
cause: Unknown
workaround: Unknown
"""
import sys

try:
    from modules import foo
except NameError as e:
    print(e)
try:
    from modules import foo
    print('Should not get here')
except NameError as e:
    print(e)

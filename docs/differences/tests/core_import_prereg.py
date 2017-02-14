"""
categories: Core,import
description: Failed to load modules are still registered as loaded
cause: Unknown
workaround: Unknown
"""
try:
    from testmodules import foo
except NameError as e:
    print(e)
try:
    from testmodules import foo
    print('Should not get here')
except NameError as e:
    print(e)

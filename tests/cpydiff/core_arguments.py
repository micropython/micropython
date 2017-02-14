"""
categories: Core
description: Error messages may display incorrect argument counts
cause: Unknown
workaround: Unknown
"""
try:
    [].append()
except Exception as e:
    print(e)

"""
categories: Modules,sys
description: Overriding sys.stdin, sys.stdout and sys.stderr not possible
cause: They are stored in read-only memory.
workaround: Unknown
"""
import sys

sys.stdin = None
print(sys.stdin)

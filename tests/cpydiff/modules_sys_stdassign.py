"""
categories: Modules,sys
description: Override sys.stdin, sys.stdout and sys.stderr. Impossible as they are stored in read-only memory.
cause: Unknown
workaround: Unknown
"""
import sys
sys.stdin = None
print(sys.stdin)

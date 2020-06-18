try:
    import usys as sys
except ImportError:
    import sys

print(sys.stdin.fileno())
print(sys.stdout.fileno())

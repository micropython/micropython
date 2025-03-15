# Test sys.std*.buffer objects.

import sys

try:
    sys.stdout.buffer
    sys.stdin.buffer
    sys.stderr.buffer
except AttributeError:
    print("SKIP")
    raise SystemExit

# CPython is more verbose; no need to match exactly

print('FileIO' in str(sys.stdout.buffer))
print('FileIO' in str(sys.stderr.buffer))
print('FileIO' in str(sys.stdin.buffer))

print('FileIO' in str(type(sys.stdout.buffer)))
print('FileIO' in str(type(sys.stderr.buffer)))
print('FileIO' in str(type(sys.stdin.buffer)))

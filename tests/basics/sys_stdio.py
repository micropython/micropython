# Test sys.std* objects.

import sys

try:
    sys.stdout
    sys.stdin
    sys.stderr
except AttributeError:
    print("SKIP")
    raise SystemExit

# CPython is more verbose; no need to match exactly

print('TextIOWrapper' in str(sys.stdout))
print('TextIOWrapper' in str(sys.stderr))
print('TextIOWrapper' in str(sys.stdin))

print('TextIOWrapper' in str(type(sys.stdout)))
print('TextIOWrapper' in str(type(sys.stderr)))
print('TextIOWrapper' in str(type(sys.stdin)))

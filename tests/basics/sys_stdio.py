# Test sys.std* objects.

import sys

# CPython is more verbose; no need to match exactly

print('TextIOWrapper' in str(sys.stdout))
print('TextIOWrapper' in str(sys.stderr))
print('TextIOWrapper' in str(sys.stdin))

print('TextIOWrapper' in str(type(sys.stdout)))
print('TextIOWrapper' in str(type(sys.stderr)))
print('TextIOWrapper' in str(type(sys.stdin)))

# # .buffer member is optional
# try:
#     print('FileIO' in str(sys.stdout.buffer))
#     print('FileIO' in str(sys.stderr.buffer))
#     print('FileIO' in str(sys.stdin.buffer))
#
#     print('FileIO' in str(type(sys.stdout.buffer)))
#     print('FileIO' in str(type(sys.stderr.buffer)))
#     print('FileIO' in str(type(sys.stdin.buffer)))
# except AttributeError:
#     print("SKIP")

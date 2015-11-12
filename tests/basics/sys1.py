# test sys module

import sys

print(sys.__name__)
print(type(sys.path))
print(type(sys.argv))
print(sys.byteorder in ('little', 'big'))
print(sys.maxsize > 100)
print(sys.implementation.name in ('cpython', 'micropython'))

try:
    sys.exit()
except SystemExit as e:
    print("SystemExit", e.args)

try:
    sys.exit(42)
except SystemExit as e:
    print("SystemExit", e.args)

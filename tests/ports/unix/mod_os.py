# This module is not entirely compatible with CPython
import os

if not hasattr(os, "getenv"):
    print("SKIP")
    raise SystemExit

os.putenv("TEST_VARIABLE", "TEST_VALUE")

print(os.getenv("TEST_VARIABLE"))
print(os.getenv("TEST_VARIABLE", "TEST_DEFAULT_VALUE"))

os.unsetenv("TEST_VARIABLE")

print(os.getenv("TEST_VARIABLE"))
print(os.getenv("TEST_VARIABLE", "TEST_DEFAULT_VALUE"))

print(os.system("exit 0"))

rand = os.urandom(4)
print(type(rand) is bytes, len(rand))

os.errno(2)
print(os.errno())

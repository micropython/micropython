# Test passing in bad values to socket.socket constructor.

try:
    import socket
except:
    print("SKIP")
    raise SystemExit

try:
    s = socket.socket(None)
except TypeError:
    print("TypeError")

try:
    s = socket.socket(socket.AF_INET, None)
except TypeError:
    print("TypeError")

# This may or may not raise an exception, depending on the socket implementation.
# The test is here for coverage.
try:
    s = socket.socket(socket.AF_INET, 123456)
except OSError:
    pass

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_RAW, None)
except TypeError:
    print("TypeError")

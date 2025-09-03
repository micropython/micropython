try:
    import socket
except:
    print("SKIP")
    raise SystemExit

try:
    s = socket(None)
except TypeError:
    print("TypeError")

try:
    s = socket(socket.AF_INET, None)
except TypeError:
    print("TypeError")

try:
    s = socket(socket.AF_INET, socket.SOCK_STREAM, None)
except TypeError:
    print("TypeError")

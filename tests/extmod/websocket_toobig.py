try:
    import io
    import errno
    import websocket
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    buf = "x" * 65536
except MemoryError:
    print("SKIP")
    raise SystemExit


# do a websocket write and then return the raw data from the stream
def ws_write(msg, sz):
    s = io.BytesIO()
    ws = websocket.websocket(s)
    ws.write(msg)
    s.seek(0)
    return s.read(sz)


try:
    print(ws_write(buf, 1))
except OSError as e:
    print("ioctl: ENOBUFS:", e.errno == errno.ENOBUFS)

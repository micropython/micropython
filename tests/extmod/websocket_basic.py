try:
    import urandom
    import uio
    import uerrno
    import websocket
except ImportError:
    print("SKIP")
    raise SystemExit

# When writing a websocket frame as a client, the masking key used is obtained
# from the "urandom" module.  Seeding the random number generator with a
# constant guarantees that the masking keys used are deterministic.
urandom.seed(0x875513b0)

# put raw data in the stream and do a websocket read
def ws_read(msg, sz):
    ws = websocket.websocket(uio.BytesIO(msg))
    return ws.read(sz)

# do a websocket write and then return the raw data from the stream
def ws_write(msg, sz, **kwargs):
    s = uio.BytesIO()
    ws = websocket.websocket(s, **kwargs)
    ws.write(msg)
    s.seek(0)
    return s.read(sz)

# basic frame
print(ws_read(b"\x81\x04ping", 4))
print(ws_read(b"\x80\x04ping", 4)) # FRAME_CONT
print(ws_write(b"pong", 6))

# split frames are not supported
# print(ws_read(b"\x01\x04ping", 4))

# extended payloads
print(ws_read(b'\x81~\x00\x80' + b'ping' * 32, 128))
print(ws_write(b"pong" * 32, 132))

# read mask (returned data will be 'mask' ^ 'mask')
print(ws_read(b"\x81\x84maskmask", 4))

# write mask
print(ws_write(b"pong", 10, debug_mask=b"\x01\x00\x01\x00"))
print(ws_write(b"pong", 10, is_client=True))

# close control frame
s = uio.BytesIO(b'\x88\x00') # FRAME_CLOSE
ws = websocket.websocket(s)
print(ws.read(1))
s.seek(2)
print(s.read(4))

# misc control frames
print(ws_read(b"\x89\x00\x81\x04ping", 4)) # FRAME_PING
print(ws_read(b"\x8a\x00\x81\x04pong", 4)) # FRAME_PONG

# close method
ws = websocket.websocket(uio.BytesIO())
ws.close()

# ioctl
ws = websocket.websocket(uio.BytesIO())
print(ws.ioctl(8)) # GET_DATA_OPTS
print(ws.ioctl(9, 2)) # SET_DATA_OPTS
print(ws.ioctl(9))
try:
    ws.ioctl(-1)
except OSError as e:
    print("ioctl: EINVAL:", e.args[0] == uerrno.EINVAL)

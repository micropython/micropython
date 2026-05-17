# Test SSL ioctl method.
# Direct access to this method is only available if MICROPY_UNIX_COVERAGE is enabled.

try:
    import io, ssl

    io.BytesIO
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

_MP_STREAM_POLL = 3
_MP_STREAM_CLOSE = 4
_MP_STREAM_GET_FILENO = 10

s = ssl.wrap_socket(io.BytesIO(), server_side=1, do_handshake=0)

if not hasattr(s, "ioctl"):
    print("SKIP")
    raise SystemExit

# These ioctl's should be unsupported.
for request in (-1, 0, _MP_STREAM_GET_FILENO):
    try:
        s.ioctl(request, 0)
    except OSError:
        print(request, "OSError")

# These ioctl's should be supported.
for request in (_MP_STREAM_CLOSE, _MP_STREAM_POLL, _MP_STREAM_CLOSE):
    print(request, s.ioctl(request, 0))

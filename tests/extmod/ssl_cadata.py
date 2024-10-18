# Test ssl.wrap_socket() with cadata passed in.

try:
    import io
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

# Invalid cadata.
try:
    ssl.wrap_socket(io.BytesIO(), cadata=b"!")
except AttributeError:
    # "cadata" keyword argument is not supported by axtls.
    print("SKIP")
    raise SystemExit
except ValueError as er:
    print(repr(er))

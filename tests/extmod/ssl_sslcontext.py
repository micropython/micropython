# Very basic test of ssl.SSLContext class.

try:
    import socket, ssl
except ImportError:
    print("SKIP")
    raise SystemExit

# Test constructing with arguments.
ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)

# Test printing object.
ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
print("SSLContext" in str(ctx))

# Coverage test for destructor, and calling it twice.
if hasattr(ctx, "__del__"):
    ctx.__del__()
    ctx.__del__()

# Test calling .wrap_socket() method, multiple times.
ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
ctx.wrap_socket(socket.socket(), do_handshake_on_connect=False)
ctx.wrap_socket(socket.socket(), do_handshake_on_connect=False)

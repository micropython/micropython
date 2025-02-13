# Test DTLS functionality including timeout handling

try:
    from tls import PROTOCOL_DTLS_CLIENT, PROTOCOL_DTLS_SERVER, SSLContext, CERT_NONE
    import io
except ImportError:
    print("SKIP")
    raise SystemExit


class DummySocket(io.IOBase):
    def __init__(self):
        self.write_buffer = bytearray()
        self.read_buffer = bytearray()

    def write(self, data):
        return len(data)

    def readinto(self, buf):
        # This is a placeholder socket that doesn't actually read anything
        # so the read buffer is always empty.
        return None

    def ioctl(self, req, arg):
        if req == 4:  # MP_STREAM_CLOSE
            return 0
        return -1


# Create dummy sockets for testing
server_socket = DummySocket()
client_socket = DummySocket()

# Wrap the DTLS Server
dtls_server_ctx = SSLContext(PROTOCOL_DTLS_SERVER)
dtls_server_ctx.verify_mode = CERT_NONE
dtls_server = dtls_server_ctx.wrap_socket(server_socket, do_handshake_on_connect=False)
print("Wrapped DTLS Server")

# Wrap the DTLS Client
dtls_client_ctx = SSLContext(PROTOCOL_DTLS_CLIENT)
dtls_client_ctx.verify_mode = CERT_NONE
dtls_client = dtls_client_ctx.wrap_socket(client_socket, do_handshake_on_connect=False)
print("Wrapped DTLS Client")

# Trigger the timing check multiple times with different elapsed times
for i in range(10):  # Try multiple iterations to hit the timing window
    dtls_client.write(b"test")
    data = dtls_server.read(1024)  # This should eventually hit the timing condition

print("OK")

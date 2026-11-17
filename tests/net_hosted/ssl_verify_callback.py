# test ssl verify_callback

import io
import socket
import tls

context = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)

if not hasattr(context, "verify_callback"):
    print("SKIP")
    raise SystemExit


def verify_callback(cert, depth):
    print("verify_callback:", type(cert), len(cert) > 100, depth)
    return 0


def verify_callback_fail(cert, depth):
    print("verify_callback_fail:", type(cert), len(cert) > 100, depth)
    return 1


def test(peer_addr):
    context.verify_mode = tls.CERT_OPTIONAL
    context.verify_callback = verify_callback
    s = socket.socket()
    s.connect(peer_addr)
    s = context.wrap_socket(s)
    s.close()

    context.verify_callback = verify_callback_fail
    s = socket.socket()
    s.connect(peer_addr)
    try:
        s = context.wrap_socket(s)
    except OSError as e:
        print(e.args[1])


if __name__ == "__main__":
    test(socket.getaddrinfo("micropython.org", 443)[0][-1])

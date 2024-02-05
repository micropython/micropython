# test ssl.getpeercert() method

import io
import socket
import ssl

s_test = ssl.wrap_socket(io.BytesIO(), server_side=True, do_handshake=False)
s_test.close()
if not hasattr(s_test, "getpeercert"):
    print("SKIP")
    raise SystemExit


def test(peer_addr):
    s = socket.socket()
    s.connect(peer_addr)
    s = ssl.wrap_socket(s)
    cert = s.getpeercert(True)
    print(type(cert), len(cert) > 100)
    s.close()


if __name__ == "__main__":
    test(socket.getaddrinfo("micropython.org", 443)[0][-1])

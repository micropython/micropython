import socket
import ssl

if not hasattr(ssl, "CERT_REQUIRED"):
    print("SKIP")
    raise SystemExit

# For details about this cert, see comment in test_sslcontext_client.py
root_cert_path = "isrgrootx1.der"


def main(use_stream=True):
    with open(root_cert_path, "rb") as f:
        cadata = f.read()
    s = socket.socket()
    ai = socket.getaddrinfo("micropython.org", 443)
    addr = ai[0][-1]
    s.connect(addr)
    s = ssl.wrap_socket(
        s, cert_reqs=ssl.CERT_REQUIRED, cadata=cadata, server_hostname="micropython.org"
    )
    s.write(b"GET / HTTP/1.0\r\n\r\n")
    print(s.read(17))
    s.close()


main()

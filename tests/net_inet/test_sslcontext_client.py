import os
import socket
import ssl

if not hasattr(ssl, "CERT_REQUIRED"):
    print("SKIP")
    raise SystemExit

# This ISRG Root X1 certificate was downloaded from https://letsencrypt.org/certificates/#root-cas
# This cert is used to sign the intermediate cert in use by micropython.org
# To check the current intermediate cert, can run:
# $ openssl s_client -showcerts -connect micropython.org:443 </dev/null 2>/dev/null
root_cert_path = "isrgrootx1.der"


def main(use_stream=True):
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)

    context.verify_mode = ssl.CERT_REQUIRED
    assert context.verify_mode == ssl.CERT_REQUIRED

    context.load_verify_locations(cafile=root_cert_path)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    addr = socket.getaddrinfo("micropython.org", 443)[0][-1]

    # CPython can wrap the socket even if not connected yet.
    # ssl_sock = context.wrap_socket(s, server_hostname='micropython.org')
    # ssl_sock.connect(addr)

    # MicroPython needs to connect first, CPython can do this too.
    s.connect(addr)
    # server_hostname must match CN (Common Name) in the certificate
    # presented by the server
    ssl_sock = context.wrap_socket(s, server_hostname="micropython.org")
    ssl_sock.write(b"GET / HTTP/1.0\r\n\r\n")
    print(ssl_sock.read(17))
    assert isinstance(ssl_sock.cipher(), tuple)
    ssl_sock.close()


main()

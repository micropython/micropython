import os
import socket
import ssl

# This certificate was obtained from micropython.org using openssl:
# $ openssl s_client -showcerts -connect micropython.org:443 </dev/null 2>/dev/null
# The certificate is from Let's Encrypt:
# 1 s:C=US, O=Let's Encrypt, CN=R10
#   i:C=US, O=Internet Security Research Group, CN=ISRG Root X1
#   a:PKEY: RSA, 2048 (bit); sigalg: sha256WithRSAEncryption
#   v:NotBefore: Mar 13 00:00:00 2024 GMT; NotAfter: Mar 12 23:59:59 2027 GMT
# Copy PEM content to a file (mpycert.pem) and convert to DER e.g.
# $ openssl x509 -in mpycert.pem -out mpycert.der -outform DER

ca_cert_chain = "mpycert.der"
try:
    os.stat(ca_cert_chain)
except OSError:
    print("SKIP")
    raise SystemExit


def main(use_stream=True):
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)

    context.verify_mode = ssl.CERT_REQUIRED
    assert context.verify_mode == ssl.CERT_REQUIRED

    context.load_verify_locations(cafile=ca_cert_chain)

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

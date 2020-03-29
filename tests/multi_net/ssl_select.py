# Test creating an SSL connection and transferring some data, and specifically
# using select to "poll" for reading while reading chunks that are smaller than
# the SSL record transmitted to make sure that the poll ioctl handles the case
# where the ssl layer has some bytes buffered internally and the underlying
# raw socket is not readable.
# This test won't run under CPython because it doesn't fix the bug this test tests:
# https://docs.python.org/3/library/ssl.html#notes-on-non-blocking-sockets

try:
    import usocket as socket, ussl as ssl, ubinascii as binascii, uselect as select
except ModuleNotFoundError:
    import socket, ssl, binascii, select

PORT = 8000

# This self-signed key/cert pair is randomly generated and to be used for
# testing/demonstration only.
# openssl req -x509 -newkey rsa:1024 -keyout key.pem -out cert.pem -days 36500 -nodes
cert = """
-----BEGIN CERTIFICATE-----
MIICaDCCAdGgAwIBAgIUaYEwlY581HuPWHm2ndTWejuggAIwDQYJKoZIhvcNAQEL
BQAwRTELMAkGA1UEBhMCVVMxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM
GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAgFw0yMDA0MTgxOTAwMDBaGA8yMTIw
MDMyNTE5MDAwMFowRTELMAkGA1UEBhMCVVMxEzARBgNVBAgMClNvbWUtU3RhdGUx
ITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCBnzANBgkqhkiG9w0B
AQEFAAOBjQAwgYkCgYEAxmACtMgGR2tTKVHzxG67Yx61pWNynXUE0q00yJ0a34AK
uQKzvyEdvkk5lL3snV4N5wKeRgWmS3/krl/YQO+Rk4eSJRqJc8INd3qSOFSNUgPg
W0VPP9vPox8au5Ngqn06jgtdD1F0a6Z+f+N3+JyRPAaetIWlFC9WEn+zzz0/cmkC
AwEAAaNTMFEwHQYDVR0OBBYEFBaI7GVj4GjxPWq+RO7A/4INOq2RMB8GA1UdIwQY
MBaAFBaI7GVj4GjxPWq+RO7A/4INOq2RMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZI
hvcNAQELBQADgYEAMpdYd8jkWxoXMxV+X2rpyx/BnPrPa+l2LehlulrU7lRh4QIU
t4f+W+yBvkFscPatpRfJoXXqregmhLxo8poKw08pjn7DNKBzcsPsxnmRIvFZuL2J
wYHGyP9HcMpsnx+UW2YjjQ4R1I0smRI7ZKiax8AJkN/P9eHH9Xku6ostXYk=
-----END CERTIFICATE-----
"""
key = """
-----BEGIN PRIVATE KEY-----
MIICeAIBADANBgkqhkiG9w0BAQEFAASCAmIwggJeAgEAAoGBAMZgArTIBkdrUylR
88Ruu2MetaVjcp11BNKtNMidGt+ACrkCs78hHb5JOZS97J1eDecCnkYFpkt/5K5f
2EDvkZOHkiUaiXPCDXd6kjhUjVID4FtFTz/bz6MfGruTYKp9Oo4LXQ9RdGumfn/j
d/ickTwGnrSFpRQvVhJ/s889P3JpAgMBAAECgYBPkxnizM3//iRY0d/37vdKFnqF
AnRqhxNNM1+WDbdG6kTi3BugUrdsqlDnwpvUsHLhNOKqcf+4D3B7JkVIHxGEqLSl
YMbQrldodPwIP0ycf9hegzuhEvuYGkex22edmQ5brkdIt6QCv0QRtProYowJx4p6
CuM5423ORejs6Vw9gQJBAOF//1Ovmm5Q1d90ZzjFhZCwG3/z5uwqZMGBxJTaibSC
O5cci3n9Tcc4AebnMf5eyrXHovtSg1FfDxS+IUccXRECQQDhNM3R31YvYmRZwrTn
f71y+buXpUtMDUDhFK8FNZN1/zJ6dJVrWQ/MVj+TaNjLUYNdPmRPHQdt8+Fx65y9
95/ZAkEAqgmkdGwz3P9jZm4V778xqhrBgche1rJY63l4zG3F7LFPUfEaU1BoN9LJ
zF2FWzQLUutIwI5FqzQs4Q1FdqOyoQJBALAL1iUMwFO0R5v/X+lj6xXY8PM/jJf7
+E67G4In+okQIEanojJTYc0rUvGJ0YdGxjj6z/EkUS17qy2hsFq0GykCQQCiucp9
7kbPpzw/gW+ERfoLgtZKrP/+Au9C5sz2wxUpeKhYihVePF8pmytyD8mqt/3LIJhZ
NA2FEss2+KJUCjHc
-----END PRIVATE KEY-----
"""
chain = cert + key
# Produce cert/key for MicroPython
cert = cert[cert.index("M") : cert.index("=") + 2]
key = key[key.index("M") : key.rstrip().rindex("\n") + 1]
cert = binascii.a2b_base64(cert)
key = binascii.a2b_base64(key)

# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()
    s2, _ = s.accept()
    if hasattr(ssl, "SSLContext"):
        fn = "/tmp/MP_test_cert.pem"
        with open(fn, "w") as f:
            f.write(chain)
        ctx = ssl.SSLContext()
        ctx.load_cert_chain(fn)
        s2 = ctx.wrap_socket(s2, server_side=True)
    else:
        s2 = ssl.wrap_socket(s2, server_side=True, key=key, cert=cert)
    print(s2.read(16))
    s2.write(b"server to client")
    # test larger client->server record being read in small chunks
    total = 0
    fileno = s2
    if hasattr(s2, "fileno"):
        fileno = s2.fileno()
    while True:
        if hasattr(ssl, "SSLContext"):
            # select doesn't actually work right in CPython! so skip it
            sel = [[1]]
        else:
            sel = select.select([fileno], [], [])
        if len(sel[0]) == 1:
            buf = s2.read(20)
            print("got", len(buf))
            total += len(buf)
            if total == 200:
                break
    print("got", total)
    s2.write(b"server to client 2")
    print("DONE", len(buf))
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    s = ssl.wrap_socket(s)
    s.write(b"client to server")
    print(s.read(16))
    s.write(bytes(200))
    print(s.read(18))
    s.close()

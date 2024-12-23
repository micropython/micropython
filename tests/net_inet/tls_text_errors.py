# test that modtls produces a text error message

import socket, ssl


def test(addr):
    s = socket.socket()
    s.connect(addr)
    try:
        ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        if hasattr(ssl_context, "check_hostname"):
            # Disable hostname check on CPython.
            ssl_context.check_hostname = False
        s = ssl_context.wrap_socket(s)
        print("wrap: no exception")
    except OSError as e:
        # mbedtls produces "mbedtls -0x7200: SSL - An invalid SSL record was received"
        # axtls produces "RECORD_OVERFLOW" but also prints "TLS buffer overflow,..."
        # CPython produces "[SSL: WRONG_VERSION_NUMBER] wrong version number (_ssl.c:1108)"
        # CPython newer version produces "[SSL] record layer failure (_ssl.c:1000)"
        ok = (
            "SSL_INVALID_RECORD" in str(e)
            or "RECORD_OVERFLOW" in str(e)
            or "wrong version" in str(e)
            or "record layer failure" in str(e)
        )
        print("wrap:", ok)
        if not ok:
            print("got exception:", e)
    s.close()


if __name__ == "__main__":
    # connect to plain HTTP port, oops!
    addr = socket.getaddrinfo("micropython.org", 80)[0][-1]
    test(addr)

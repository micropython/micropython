# test that modtls produces a numerical error message when out of heap

import socket, tls

# This test is specific to the mbedTLS implementation, so require that.
if not hasattr(tls, "MBEDTLS_VERSION"):
    print("SKIP")
    raise SystemExit

try:
    from micropython import heap_lock, heap_unlock
except:
    print("SKIP")
    raise SystemExit

try:
    from micropython import alloc_emergency_exception_buf

    alloc_emergency_exception_buf(256)
except:
    pass


# test with heap locked to see it switch to number-only error message
def test(addr):
    ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    ctx.verify_mode = tls.CERT_NONE
    s = socket.socket()
    s.connect(addr)
    try:
        s.setblocking(False)
        s = ctx.wrap_socket(s, do_handshake_on_connect=False)
        heap_lock()
        print("heap is locked")
        while True:
            ret = s.write("foo")
            if ret:
                break
        heap_unlock()
        print("wrap: no exception")
    except OSError as e:
        heap_unlock()
        # mbedtls produces "-29184"
        # axtls produces "RECORD_OVERFLOW"
        ok = "-29184" in str(e) or "RECORD_OVERFLOW" in str(e)
        print("wrap:", ok)
        if not ok:
            print("got exception:", e)
    s.close()


if __name__ == "__main__":
    # connect to plain HTTP port, oops!
    addr = socket.getaddrinfo("micropython.org", 80)[0][-1]
    test(addr)

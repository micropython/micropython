# test that modtls produces a numerical error message when out of heap

try:
    import usocket as socket, ussl as ssl, sys
except:
    import socket, ssl, sys
try:
    from micropython import alloc_emergency_exception_buf, heap_lock, heap_unlock
except:
    print("SKIP")
    raise SystemExit


# test with heap locked to see it switch to number-only error message
def test(addr):
    alloc_emergency_exception_buf(256)
    s = socket.socket()
    s.connect(addr)
    try:
        s.setblocking(False)
        s = ssl.wrap_socket(s, do_handshake=False)
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

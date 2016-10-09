try:
    import usocket as socket
except:
    import socket


def main(use_stream=False):
    s = socket.socket()

    ai = socket.getaddrinfo("google.com", 80)
    print("Address infos:", ai)
    addr = ai[0][-1]

    print("Connect address:", addr)
    s.connect(addr)

    if use_stream:
        # MicroPython socket objects support stream (aka file) interface
        # directly, but the line below is needed for CPython.
        s = s.makefile("rwb", 0)
        s.write(b"GET / HTTP/1.0\r\n\r\n")
        print(s.readall())
    else:
        s.send(b"GET / HTTP/1.0\r\n\r\n")
        print(s.recv(4096))

    s.close()


main()

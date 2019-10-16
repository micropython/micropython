# test that socket.connect() has correct polling behaviour before, during and after

try:
    import usocket as socket, uselect as select
except:
    import socket, select


def test(peer_addr):
    s = socket.socket()
    poller = select.poll()
    poller.register(s)

    # test poll before connect
    p = poller.poll(0)
    print(len(p), p[0][-1])

    s.connect(peer_addr)

    # test poll during connection
    print(len(poller.poll(0)))

    # test poll after connection is established
    p = poller.poll(1000)
    print(len(p), p[0][-1])

    s.close()


if __name__ == "__main__":
    test(socket.getaddrinfo('micropython.org', 80)[0][-1])

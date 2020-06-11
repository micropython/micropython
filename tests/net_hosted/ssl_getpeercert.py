# test ssl.getpeercert() method

try:
    import usocket as socket
    import ussl as ssl
except:
    import socket
    import ssl


def test(peer_addr):
    s = socket.socket()
    s.connect(peer_addr)
    s = ssl.wrap_socket(s)
    cert = s.getpeercert(True)
    print(type(cert), len(cert) > 100)
    s.close()


if __name__ == "__main__":
    test(socket.getaddrinfo("micropython.org", 443)[0][-1])

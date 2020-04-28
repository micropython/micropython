try:
    import usocket as socket, sys
except:
    import socket, sys


def test_non_existant():
    try:
        res = socket.getaddrinfo("nonexistant.example.com", 80)
        print("getaddrinfo returned", res)
    except OSError as e:
        # print(e)
        print("getaddrinfo raised")


def test_bogus():
    try:
        res = socket.getaddrinfo("..", 80)
        print("getaddrinfo returned", res)
    except OSError as e:
        # print(e)
        print("getaddrinfo raised")
    except Exception as e:
        print("getaddrinfo raised")  # CPython raises UnicodeError!?


def test_ip_addr():
    try:
        res = socket.getaddrinfo("10.10.10.10", 80)
        print("getaddrinfo returned resolutions")
    except Exception as e:
        print("getaddrinfo raised", e)


def test_0_0_0_0():
    try:
        res = socket.getaddrinfo("0.0.0.0", 80)
        print("getaddrinfo returned resolutions")
    except Exception as e:
        print("getaddrinfo raised", e)


def test_valid():
    try:
        res = socket.getaddrinfo("micropython.org", 80)
        print("getaddrinfo returned resolutions")
    except Exception as e:
        print("getaddrinfo raised", e)


def test_bind_resolves_0_0_0_0():
    s = socket.socket()
    try:
        s.bind(("0.0.0.0", 31245))
        print("bind actually bound")
        s.close()
    except Exception as e:
        print("bind raised", e)


def test_bind_resolves_localhost():
    s = socket.socket()
    try:
        s.bind(("localhost", 31245))
        print("bind actually bound")
        s.close()
    except Exception as e:
        print("bind raised", e)


def test_connect_resolves():
    s = socket.socket()
    try:
        s.connect(("micropython.org", 80))
        print("connect actually connected")
        s.close()
    except Exception as e:
        print("connect raised", e)


def test_connect_non_existant():
    s = socket.socket()
    try:
        s.connect(("nonexistant.example.com", 80))
        print("connect actually connected")
        s.close()
    except OSError as e:
        print("connect raised OSError")
    except Exception as e:
        print("connect raised", e)


test_funs = [n for n in dir() if n.startswith("test_")]
for f in sorted(test_funs):
    print("--", f, end=": ")
    eval(f + "()")

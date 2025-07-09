import socket, sys


def check_ipv6_support():
    try:
        socket.AF_INET6
        return True
    except AttributeError:
        return False


if not check_ipv6_support():
    print("SKIP")
    raise SystemExit


def test_ipv6_localhost():
    try:
        res = socket.getaddrinfo("::1", 80, socket.AF_INET6)
        print("IPv6 localhost getaddrinfo returned resolutions")
    except Exception as e:
        print("IPv6 localhost getaddrinfo raised", e)


def test_ipv6_wildcard():
    try:
        res = socket.getaddrinfo("::", 80, socket.AF_INET6)
        print("IPv6 wildcard getaddrinfo returned resolutions")
    except Exception as e:
        print("IPv6 wildcard getaddrinfo raised", e)


def test_ipv6_address():
    try:
        res = socket.getaddrinfo("2001:db8::1", 80, socket.AF_INET6)
        print("IPv6 address getaddrinfo returned resolutions")
    except Exception as e:
        print("IPv6 address getaddrinfo raised", e)


def test_dual_stack():
    try:
        # Test that both IPv4 and IPv6 can be resolved
        res4 = socket.getaddrinfo("127.0.0.1", 80, socket.AF_INET)
        res6 = socket.getaddrinfo("::1", 80, socket.AF_INET6)
        print("Dual stack getaddrinfo returned resolutions")
    except Exception as e:
        print("Dual stack getaddrinfo raised", e)


def test_family_unspec():
    try:
        # Test unspecified family (should return both IPv4 and IPv6 if available)
        res = socket.getaddrinfo("localhost", 80)
        has_ipv4 = any(addr[0] == socket.AF_INET for addr in res)
        has_ipv6 = any(addr[0] == socket.AF_INET6 for addr in res)
        if has_ipv4 or has_ipv6:
            print("Unspecified family getaddrinfo returned resolutions")
        else:
            print("Unspecified family getaddrinfo returned empty")
    except Exception as e:
        print("Unspecified family getaddrinfo raised", e)


def test_ipv6_domain():
    try:
        # Try to resolve a domain name with IPv6
        res = socket.getaddrinfo("google.com", 80, socket.AF_INET6)
        print("IPv6 domain getaddrinfo returned resolutions")
    except Exception as e:
        print("IPv6 domain getaddrinfo raised", e)


test_funs = [n for n in dir() if n.startswith("test_")]
for f in sorted(test_funs):
    print("--", f, end=": ")
    eval(f + "()")

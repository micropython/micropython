# test that socket.connect() on a non-blocking socket raises EINPROGRESS
# and that an immediate write/send/read/recv does the right thing

try:
    import usocket as socket, ussl as ssl, sys
except:
    import socket, ssl, sys

def do_connect(peer_addr, tls):
    s = socket.socket()
    s.setblocking(False)
    try:
        s.connect(peer_addr)
    except OSError as er:
        print('connect:', er.args[0] == 115) # 115 is EINPROGRESS
    # wrap with ssl/tls if desired
    if tls:
        try:
            if sys.implementation.name == 'micropython':
                s = ssl.wrap_socket(s)
            else:
                s = ssl.wrap_socket(s, do_handshake_on_connect=False)
            print("wrap: True")
        except Exception as e:
            print("wrap:", e)
    return s

def test(peer_addr, tls=False):
    # a fresh socket is opened for each combination because MP on linux is too fast

    # hasRW is used to force socket.read and socket.write to None in CPython since it doesn't
    # have these methods and we expect a None result
    hasRW = sys.implementation.name == 'micropython' or tls or None

    # connect + send
    s = do_connect(peer_addr, tls)
    # send -> EAGAIN
    try:
        print('send ret:', s.send(b'1234'))
    except OSError as er:
        print('send:', er, er.args[0] == 11) # 11 is EAGAIN
    s.close()

    # connect + write
    s = do_connect(peer_addr, tls)
    import time
    time.sleep(0.2)
    # write -> None
    try:
        ret = hasRW and s.write(b'1234') # None in CPython
        print('write ret:', ret, ret is None)
    except OSError as er:
        print('write:', er, False) # should not raise
    except ValueError as er: # CPython
        print('write:', er, er.args[0] == 'Write on closed or unwrapped SSL socket.')
    s.close()

    # connect + recv
    s = do_connect(peer_addr, tls)
    # recv -> EAGAIN
    try:
        print('recv ret:', s.recv(10))
    except OSError as er:
        print('recv:', er, er.args[0] == 11) # 11 is EAGAIN
    s.close()

    # connect + read
    s = do_connect(peer_addr, tls)
    # read -> None
    try:
        ret = hasRW and s.read(10)
        print('read ret:', ret, ret is None)
    except OSError as er:
        print('read:', er, False) # should not raise
    except ValueError as er: # CPython
        print('read:', er, er.args[0] == 'Read on closed or unwrapped SSL socket.')
    s.close()

if __name__ == "__main__":
    print("--- Plain sockets ---")
    test(socket.getaddrinfo('micropython.org', 80)[0][-1], False)
    print("--- SSL sockets ---")
    test(socket.getaddrinfo('micropython.org', 443)[0][-1], True)

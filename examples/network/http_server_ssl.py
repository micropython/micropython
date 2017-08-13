try:
    import usocket as socket
except:
    import socket
import ussl as ssl


CONTENT = b"""\
HTTP/1.0 200 OK

Hello #%d from MicroPython!
"""

def main(use_stream=True):
    s = socket.socket()

    # Binding to all interfaces - server will be accessible to other hosts!
    ai = socket.getaddrinfo("0.0.0.0", 8443)
    print("Bind address info:", ai)
    addr = ai[0][-1]

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(addr)
    s.listen(5)
    print("Listening, connect your browser to https://<this_host>:8443/")

    counter = 0
    while True:
        res = s.accept()
        client_s = res[0]
        client_addr = res[1]
        print("Client address:", client_addr)
        print("Client socket:", client_s)
        client_s = ssl.wrap_socket(client_s, server_side=True)
        print(client_s)
        print("Request:")
        if use_stream:
            # Both CPython and MicroPython SSLSocket objects support read() and
            # write() methods.
            # Browsers are prone to terminate SSL connection abruptly if they
            # see unknown certificate, etc. We must continue in such case -
            # next request they issue will likely be more well-behaving and
            # will succeed.
            try:
                req = client_s.read(4096)
                print(req)
                if req:
                    client_s.write(CONTENT % counter)
            except Exception as e:
                print("Exception serving request:", e)
        else:
            print(client_s.recv(4096))
            client_s.send(CONTENT % counter)
        client_s.close()
        counter += 1
        print()


main()

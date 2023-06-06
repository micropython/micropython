# Do not use this code in real projects! Read
# http_server_simplistic_commented.py for details.
try:
    import usocket as socket
except:
    import socket


CONTENT = b"""\
HTTP/1.0 200 OK

Hello #%d from MicroPython!
"""


def main():
    s = socket.socket()
    ai = socket.getaddrinfo("0.0.0.0", 8080)
    addr = ai[0][-1]

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    s.bind(addr)
    s.listen(5)
    print("Listening, connect your browser to http://<this_host>:8080/")

    counter = 0
    while True:
        res = s.accept()
        client_s = res[0]
        req = client_s.recv(4096)
        print("Request:")
        print(req)
        client_s.send(CONTENT % counter)
        client_s.close()
        counter += 1
        print()


main()

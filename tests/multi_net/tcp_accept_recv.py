# Test recv on listening socket after accept(), with various listen() arguments

import socket

PORT = 8000

# Test cases for listen() function
LISTEN_ARGS = [None, 0, 1, 2]  # None means no argument


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    multitest.next()

    test_num = 0
    for blocking_mode in [True, False]:
        for listen_arg in LISTEN_ARGS:
            test_num += 1
            s = socket.socket()
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])

            # Call listen with or without argument based on test case
            if listen_arg is None:
                print(f"Test case {test_num}/8: listen() blocking={blocking_mode}")
                s.listen()
            else:
                print(f"Test case {test_num}/8: listen({listen_arg}) blocking={blocking_mode}")
                s.listen(listen_arg)

            # Signal client that server is ready
            multitest.broadcast(f"server_ready_{test_num}")

            # Wait for client connection
            c, _ = s.accept()

            # Set blocking mode after accept
            s.setblocking(blocking_mode)

            try:
                print("recv", s.recv(10))  # should raise Errno 107 ENOTCONN
            except OSError as er:
                # Verify the error code is either 107 (ENOTCONN) or 128 (ENOTCONN on Windows)
                print(er.errno in (107, 128))

            # Cleanup
            c.close()
            s.close()

            # Signal client we're done with this test case
            multitest.broadcast(f"server_done_{test_num}")


# Client
def instance1():
    multitest.next()

    test_num = 0
    for blocking_mode in [True, False]:
        for _ in LISTEN_ARGS:
            test_num += 1
            # Wait for server to be ready
            multitest.wait(f"server_ready_{test_num}")

            # Connect to server
            s = socket.socket()
            s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
            s.send(b"GET / HTTP/1.0\r\n\r\n")
            s.close()

            # Wait for server to finish this test case
            multitest.wait(f"server_done_{test_num}")

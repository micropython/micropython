# Test UDP reception when there are multiple incoming UDP packets that need to be
# queued internally in the TCP/IP stack.

import socket

NUM_NEW_SOCKETS = 4
NUM_PACKET_BURSTS = 6
NUM_PACKET_GROUPS = 5
TOTAL_PACKET_BURSTS = NUM_NEW_SOCKETS * NUM_PACKET_BURSTS
# The tast passes if more than 75% of packets are received in each group.
PACKET_RECV_THRESH = 0.75 * TOTAL_PACKET_BURSTS
PORT = 8000


# Server
def instance0():
    recv_count = {i: 0 for i in range(NUM_PACKET_GROUPS)}
    multitest.globals(IP=multitest.get_network_ip())
    multitest.next()
    for i in range(NUM_NEW_SOCKETS):
        print("test socket", i)
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(socket.getaddrinfo("0.0.0.0", PORT + i)[0][-1])
        s.settimeout(0.250)
        multitest.broadcast("server ready")
        for burst in range(NUM_PACKET_BURSTS):
            # Wait for all packets to be sent, without receiving any yet.
            multitest.wait("data sent burst={}".format(burst))
            # Try to receive all packets (they should be waiting in the queue).
            for group in range(NUM_PACKET_GROUPS):
                try:
                    data, addr = s.recvfrom(1000)
                except:
                    continue
                recv_burst, recv_group = data.split(b":")
                recv_burst = int(recv_burst)
                recv_group = int(recv_group)
                if recv_burst == burst:
                    recv_count[recv_group] += 1
            # Inform the client that all data was received.
            multitest.broadcast("data received burst={}".format(burst))
        s.close()

    # Check how many packets were received.
    for group, count in recv_count.items():
        if count >= PACKET_RECV_THRESH:
            print("pass group={}".format(group))
        else:
            print("fail group={} received={}%".format(group, 100 * count // TOTAL_PACKET_BURSTS))


# Client
def instance1():
    multitest.next()
    for i in range(NUM_NEW_SOCKETS):
        print("test socket", i)
        ai = socket.getaddrinfo(IP, PORT + i)[0][-1]
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        multitest.wait("server ready")
        for burst in range(NUM_PACKET_BURSTS):
            # Send a bunch of packets all in a row.
            for group in range(NUM_PACKET_GROUPS):
                s.sendto(b"%d:%d" % (burst, group), ai)
            # Inform the server that the data has been sent.
            multitest.broadcast("data sent burst={}".format(burst))
            # Wait for the server to finish receiving.
            multitest.wait("data received burst={}".format(burst))
        s.close()

.. _esp32_udp_multicast:

UDP MULTICAST
=============

Use UDP MULTICAST connections to communicate between two devices in different networks
with IP-addresses 172.16.55.55 and 192.168.44.44.

Run `udp_multicast_client.py <https://github.com/micropython/micropython/blob/master/examples/network/udp_multicast_client.py>`_ on one device.
Client send GET and receive ACK from the server.
Output is::

    GET to   ('224.0.0.111', 5555)   sent     "client_ip:172.16.55.55  mac:b8-d6-1a-5a-c2-e4"
    ACK from ('192.168.44.44', 5555) received "server_ip:192.168.44.44 mac:24-6f-28-7a-f3-9c"
    GET to   ('224.0.0.111', 5555)   sent     "client_ip:172.16.55.55  mac:b8-d6-1a-5a-c2-e4"
    ACK from ('192.168.44.44', 5555) received "server_ip:192.168.44.44 mac:24-6f-28-7a-f3-9c"
    ...

Run `udp_multicast_server.py <https://github.com/micropython/micropython/blob/master/examples/network/udp_multicast_server.py>`_ on another device.
Server receive GET and send ACK to the client.
Output is::

    GET from ('172.16.55.55', 5555) received "client_ip:172.16.55.55  mac:b8-d6-1a-5a-c2-e4"
    ACK to   ('224.0.0.111', 5555)  sent     "server_ip:192.168.44.44 mac:24-6f-28-7a-f3-9c"
    GET from ('172.16.55.55', 5555) received "client_ip:172.16.55.55  mac:b8-d6-1a-5a-c2-e4"
    ACK to   ('224.0.0.111', 5555)  sent     "server_ip:192.168.44.44 mac:24-6f-28-7a-f3-9c"
    ...

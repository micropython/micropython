# udp_multicast_server.py::

import network, socket, errno, struct, time

print("connect to AP")

SSID = "SSID"
PASSWORD = "PASSWORD"

wlan = network.WLAN(network.STA_IF)
# class C network
wlan.ifconfig(("192.168.44.44", "255.255.255.0", "192.168.44.1", "192.168.44.1"))
wlan.active(True)
wlan.connect(SSID, PASSWORD)
while not wlan.isconnected():
    pass

print("udp_multicast_server.py")
print("server receive GET and send ACK to the client")


def inet_aton(str_addr):
    return bytes(map(int, str_addr.split(".")))


# TIMEOUT = None  # block
# TIMEOUT = 5  # seconds
TIMEOUT = 0  # non blocking

MULTICAST_IP = "224.0.0.111"
PORT = 5555
sockaddr = (MULTICAST_IP, PORT)

wlan = network.WLAN(network.STA_IF)
server_ip = wlan.ifconfig()[0]

skt = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
skt.settimeout(None)
skt.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
skt.setsockopt(
    socket.IPPROTO_IP,
    socket.IP_ADD_MEMBERSHIP,
    struct.pack(">4s4s", inet_aton(MULTICAST_IP), inet_aton(server_ip)),
)  # join to the multicast address
skt.bind(sockaddr)
skt.settimeout(TIMEOUT)

while True:
    try:
        received, addr = skt.recvfrom(1024)
        if received:
            print(f'GET from {addr}\t received "{received.decode()}"')

            mac = wlan.config("mac")
            str_to_send = f"server_ip:{server_ip}\t mac:{'-'.join(map('{:02x}'.format, mac))}"
            skt.sendto(str_to_send, sockaddr)
            print(f'ACK to   {sockaddr}\t sent     "{str_to_send}"')
    except OSError as e:
        if e.args[0] in (errno.EAGAIN, errno.ETIMEDOUT):
            pass
        else:
            try:
                skt.close()
            except:
                pass
            raise (e)

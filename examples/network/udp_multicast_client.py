# udp_multicast_client.py::

import network, socket, errno, struct, time

print("connect to AP")

SSID = "SSID"
PASSWORD = "PASSWORD"

wlan = network.WLAN(network.STA_IF)
# class B network
wlan.ifconfig(("172.16.55.55", "255.255.255.0", "172.16.55.1", "172.16.55.1"))
wlan.active(True)
wlan.connect(SSID, PASSWORD)
while not wlan.isconnected():
    pass

print("udp_multicast_client.py")
print("client send GET and receive ACK from the server")


def inet_aton(str_addr):
    return bytes(map(int, str_addr.split(".")))


# TIMEOUT = None  # block
# TIMEOUT = 5  # seconds
TIMEOUT = 0  # non blocking

MULTICAST_IP = "224.0.0.111"
PORT = 5555
sockaddr = (MULTICAST_IP, PORT)

wlan = network.WLAN(network.STA_IF)
client_ip = wlan.ifconfig()[0]

skt = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
skt.settimeout(None)
skt.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
skt.setsockopt(
    socket.IPPROTO_IP,
    socket.IP_ADD_MEMBERSHIP,
    struct.pack(">4s4s", inet_aton(MULTICAST_IP), inet_aton(client_ip)),
)  # join to the multicast address
skt.bind(sockaddr)  # not skt.connect(sockaddr)
skt.settimeout(TIMEOUT)

t = 0
while True:
    try:
        if time.time() - t >= 3:
            t = time.time()

            mac = wlan.config("mac")
            str_to_send = f"client_ip:{client_ip}\t mac:{'-'.join(map('{:02x}'.format, mac))}"
            skt.sendto(str_to_send, sockaddr)
            print(f'GET to   {sockaddr}\t sent     "{str_to_send}"')

        received, addr = skt.recvfrom(1024)
        if received:
            print(f'ACK from {addr}\t received "{received.decode()}"')
    except OSError as e:
        if e.args[0] in (errno.EAGAIN, errno.ETIMEDOUT):
            pass
        else:
            try:
                skt.close()
            except:
                pass
            raise (e)

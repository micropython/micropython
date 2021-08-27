import wifi
import socketpool
import struct
import time

# connect to wifi
print("Connecting to Wifi")
wifi.radio.connect("mySSID", "myPASS")
pool = socketpool.SocketPool(wifi.radio)

# make socket
print("Creating socket")
sock = pool.socket(pool.AF_INET, pool.SOCK_DGRAM)

# Fill packet
packet = bytearray(48)
packet[0] = 0b00100011  # Not leap second, NTP version 4, Client mode
NTP_TO_UNIX_EPOCH = 2208988800  # 1970-01-01 00:00:00

print("Sending packet")
sock.sendto(packet, ("pool.ntp.org", 123))

size, address = sock.recvfrom_into(packet)
print("Received packet")

seconds = struct.unpack_from("!I", packet, offset=len(packet) - 8)[0]
print("Address:", address)
print("Time:", time.localtime(seconds - NTP_TO_UNIX_EPOCH))

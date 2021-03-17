import wifi
import socketpool
import ssl
import time

TIMEOUT = None
HOST = "192.168.10.179"
PORT = 5000

# Connect to wifi
print("Connecting to wifi")
wifi.radio.connect("mySSID", "myPASS")
pool = socketpool.SocketPool(wifi.radio)

print("Creating Socket")
with pool.socket(pool.AF_INET, pool.SOCK_STREAM) as s:
    s.settimeout(TIMEOUT)

    print("Connecting")
    s.connect((HOST, PORT))
    print("Sending")
    sent = s.send(b"Hello, world")
    print("Receiving")
    buff = bytearray(128)
    numbytes = s.recv_into(buff)
print(repr(buff))

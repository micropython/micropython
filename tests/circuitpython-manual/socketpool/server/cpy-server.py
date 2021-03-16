import wifi
import socketpool

TIMEOUT = None

print("Connecting to Wifi")
wifi.radio.connect("mySSID", "myPASS")

pool = socketpool.SocketPool(wifi.radio)

print("Finding IP address")
print(wifi.radio.ipv4_address)
HOST = str(wifi.radio.ipv4_address)
PORT = 80  # Port to listen on

print("Creating socket")
sock = pool.socket(pool.AF_INET, pool.SOCK_STREAM)

sock.bind((HOST, PORT))
sock.listen(1)
print("Accepting connections")
conn, addr = sock.accept()
with conn:
    print("Connected by", addr)
    buff = bytearray(128)
    print("Receiving")
    numbytes = conn.recvfrom_into(buff)
    print(buff[: numbytes[0]])
    if numbytes:
        print("Sending")
        conn.send(buff[: numbytes[0]])

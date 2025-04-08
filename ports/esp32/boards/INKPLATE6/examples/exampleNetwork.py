import network
import time
from inkplate6 import Inkplate

ssid = "e-radionica.com"
password = "croduino"

# More info here: https://docs.micropython.org/en/latest/esp8266/tutorial/network_basics.html
def do_connect():
    import network

    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print("connecting to network...")
        sta_if.active(True)
        sta_if.connect(ssid, password)
        while not sta_if.isconnected():
            pass
    print("network config:", sta_if.ifconfig())


# More info here: https://docs.micropython.org/en/latest/esp8266/tutorial/network_tcp.html
def http_get(url):
    import socket

    res = ""
    _, _, host, path = url.split("/", 3)
    addr = socket.getaddrinfo(host, 80)[0][-1]
    s = socket.socket()
    s.connect(addr)
    s.send(bytes("GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n" % (path, host), "utf8"))
    while True:
        data = s.recv(100)
        if data:
            res += str(data, "utf8")
        else:
            break
    s.close()

    return res


# Calling functions defined above
do_connect()
response = http_get("http://micropython.org/ks/test.html")

# Initialise our Inkplate object
display = Inkplate(Inkplate.INKPLATE_1BIT)
display.begin()

# Print response in lines
cnt = 0
for x in response.split("\n"):
    display.printText(
        10, 10 + cnt, x.upper()
    )  # Default font has only upper case letters
    cnt += 10

# Display image from buffer
display.display()

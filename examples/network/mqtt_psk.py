import ubinascii as binascii
try:
    import usocket as _socket
except:
    import _socket
try:
    import ussl as ssl
except:
    import ssl


def main(use_ssl=True):
    s = _socket.socket()

    ai = _socket.getaddrinfo("mosquitto.local", 8883 if use_sll else 1883)
    print("Address infos:", ai)
    addr = ai[0][-1]

    print("Connect address:", addr)
    s.connect(addr)

    if use_ssl:
        # PSK key in hex
        psk_key = binascii.unhexlify(b'bb500112233445566778899AABBCCDD0')
        s = ssl.wrap_socket(s, psk_ident='itsme', psk_key=psk_key)
    print(s)

    s.write(b"\x10\x10\x00\x04MQTT\x04\x02\x00\x3c\x00\x04hack")
    ack = s.read(4)
    if ack == b'\x20\x02\x00\x00':
        print("Got MQTT CONNACK")
    else:
        print("OOPS, got:", ack)

    s.close()


main()

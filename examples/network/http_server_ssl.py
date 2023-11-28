import binascii
import socket
import ssl


# This self-signed key/cert pair is randomly generated and to be used for
# testing/demonstration only.  You should always generate your own key/cert.
key = binascii.unhexlify(
    b"3082013b020100024100cc20643fd3d9c21a0acba4f48f61aadd675f52175a9dcf07fbef"
    b"610a6a6ba14abb891745cd18a1d4c056580d8ff1a639460f867013c8391cdc9f2e573b0f"
    b"872d0203010001024100bb17a54aeb3dd7ae4edec05e775ca9632cf02d29c2a089b563b0"
    b"d05cdf95aeca507de674553f28b4eadaca82d5549a86058f9996b07768686a5b02cb240d"
    b"d9f1022100f4a63f5549e817547dca97b5c658038e8593cb78c5aba3c4642cc4cd031d86"
    b"8f022100d598d870ffe4a34df8de57047a50b97b71f4d23e323f527837c9edae88c79483"
    b"02210098560c89a70385c36eb07fd7083235c4c1184e525d838aedf7128958bedfdbb102"
    b"2051c0dab7057a8176ca966f3feb81123d4974a733df0f958525f547dfd1c271f9022044"
    b"6c2cafad455a671a8cf398e642e1be3b18a3d3aec2e67a9478f83c964c4f1f"
)
cert = binascii.unhexlify(
    b"308201d53082017f020203e8300d06092a864886f70d01010505003075310b3009060355"
    b"0406130258583114301206035504080c0b54686550726f76696e63653110300e06035504"
    b"070c075468654369747931133011060355040a0c0a436f6d70616e7958595a3113301106"
    b"0355040b0c0a436f6d70616e7958595a3114301206035504030c0b546865486f73744e61"
    b"6d65301e170d3139313231383033333935355a170d3239313231353033333935355a3075"
    b"310b30090603550406130258583114301206035504080c0b54686550726f76696e636531"
    b"10300e06035504070c075468654369747931133011060355040a0c0a436f6d70616e7958"
    b"595a31133011060355040b0c0a436f6d70616e7958595a3114301206035504030c0b5468"
    b"65486f73744e616d65305c300d06092a864886f70d0101010500034b003048024100cc20"
    b"643fd3d9c21a0acba4f48f61aadd675f52175a9dcf07fbef610a6a6ba14abb891745cd18"
    b"a1d4c056580d8ff1a639460f867013c8391cdc9f2e573b0f872d0203010001300d06092a"
    b"864886f70d0101050500034100b0513fe2829e9ecbe55b6dd14c0ede7502bde5d46153c8"
    b"e960ae3ebc247371b525caeb41bbcf34686015a44c50d226e66aef0a97a63874ca5944ef"
    b"979b57f0b3"
)


CONTENT = b"""\
HTTP/1.0 200 OK

Hello #%d from MicroPython!
"""


def main(use_stream=True):
    s = socket.socket()

    # Binding to all interfaces - server will be accessible to other hosts!
    ai = socket.getaddrinfo("0.0.0.0", 8443)
    print("Bind address info:", ai)
    addr = ai[0][-1]

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(addr)
    s.listen(5)
    print("Listening, connect your browser to https://<this_host>:8443/")

    counter = 0
    while True:
        res = s.accept()
        client_s = res[0]
        client_addr = res[1]
        print("Client address:", client_addr)
        print("Client socket:", client_s)
        # CPython uses key keyfile/certfile arguments, but MicroPython uses key/cert
        client_s = ssl.wrap_socket(client_s, server_side=True, key=key, cert=cert)
        print(client_s)
        print("Request:")
        if use_stream:
            # Both CPython and MicroPython SSLSocket objects support read() and
            # write() methods.
            # Browsers are prone to terminate SSL connection abruptly if they
            # see unknown certificate, etc. We must continue in such case -
            # next request they issue will likely be more well-behaving and
            # will succeed.
            try:
                req = client_s.readline()
                print(req)
                while True:
                    h = client_s.readline()
                    if h == b"" or h == b"\r\n":
                        break
                    print(h)
                if req:
                    client_s.write(CONTENT % counter)
            except Exception as e:
                print("Exception serving request:", e)
        else:
            print(client_s.recv(4096))
            client_s.send(CONTENT % counter)
        client_s.close()
        counter += 1
        print()


main()

# Simple test creating an SSL connection and transferring some data
# This test won't run under CPython because it requires key/cert

try:
    import ubinascii as binascii, usocket as socket, ussl as ssl
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000
_MBEDTLS = hasattr(ssl, "CERT_REQUIRED")

# This self-signed key/cert pair is randomly generated and to be used for
# testing/demonstration only.  You should always generate your own key/cert.
if _MBEDTLS:
    cert = b"""-----BEGIN CERTIFICATE-----
    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
    Pe+FTUACEZa3oGs=
    -----END CERTIFICATE-----"""

    key = b"""-----BEGIN PRIVATE KEY-----
    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
    pvvPUbqDVjrthaT4hv/xoiQjdI2DeYw=
    -----END PRIVATE KEY-----"""

else:
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

# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()
    s2, _ = s.accept()
    s2 = ssl.wrap_socket(s2, server_side=True, key=key, cert=cert)
    print(s2.read(16))
    s2.write(b"server to client")
    s2.close()
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    if _MBEDTLS:
        s = ssl.wrap_socket(s, cert_reqs=ssl.CERT_REQUIRED, ca_certs=cert)
    else:
        s = ssl.wrap_socket(s)
    s.write(b"client to server")
    print(s.read(16))
    s.close()

try:
    import uselect
    import ussl
    import io
    import ubinascii as binascii
except ImportError:
    print("SKIP")
    raise SystemExit

from micropython import const

_MP_STREAM_POLL_RD = const(0x0001)
_MP_STREAM_POLL_WR = const(0x0004)
_MP_STREAM_POLL_NVAL = const(0x0020)
_MP_STREAM_POLL = const(3)
_MP_STREAM_CLOSE = const(4)


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


class _Pipe(io.IOBase):
    def __init__(self):
        self._other = None
        self.block_reads = False
        self.block_writes = False

        self.write_buffers = []
        self.last_poll_arg = None

    def readinto(self, buf):
        if self.block_reads or len(self._other.write_buffers) == 0:
            return None

        read_buf = self._other.write_buffers[0]
        l = min(len(buf), len(read_buf))
        buf[:l] = read_buf[:l]
        if l == len(read_buf):
            self._other.write_buffers.pop(0)
        else:
            self._other.write_buffers[0] = read_buf[l:]
        return l

    def write(self, buf):
        if self.block_writes:
            return None

        self.write_buffers.append(memoryview(bytes(buf)))
        return len(buf)

    def ioctl(self, request, arg):
        if request == _MP_STREAM_POLL:
            self.last_poll_arg = arg
            ret = 0
            if arg & _MP_STREAM_POLL_RD:
                if not self.block_reads and self._other.write_buffers:
                    ret |= _MP_STREAM_POLL_RD
            if arg & _MP_STREAM_POLL_WR:
                if not self.block_writes:
                    ret |= _MP_STREAM_POLL_WR
            return ret

        elif request == _MP_STREAM_CLOSE:
            return 0

        raise NotImplementedError()

    @classmethod
    def new_pair(cls):
        p1 = cls()
        p2 = cls()
        p1._other = p2
        p2._other = p1
        return p1, p2


def assert_poll(s, i, arg, expected_arg, expected_ret):
    ret = s.ioctl(_MP_STREAM_POLL, arg)
    assert i.last_poll_arg == expected_arg
    i.last_poll_arg = None
    assert ret == expected_ret


def assert_raises(cb, *args, **kwargs):
    try:
        cb(*args, **kwargs)
        raise AssertionError("should have raised")
    except Exception as exc:
        pass


client_io, server_io = _Pipe.new_pair()

client_io.block_reads = True
client_io.block_writes = True
client_sock = ussl.wrap_socket(client_io, do_handshake=False)

server_sock = ussl.wrap_socket(server_io, key=key, cert=cert, server_side=True, do_handshake=False)

# Do a test read, at this point the TLS handshake wants to write,
# so it returns None:
assert client_sock.read(128) is None

# Polling for either read or write actually check if the underlying socket can write:
assert_poll(client_sock, client_io, _MP_STREAM_POLL_RD, _MP_STREAM_POLL_WR, 0)
assert_poll(client_sock, client_io, _MP_STREAM_POLL_WR, _MP_STREAM_POLL_WR, 0)

# Mark the socket as writable, and do another test read:
client_io.block_writes = False
assert client_sock.read(128) is None

# The client wrote the CLIENT_HELLO message
assert len(client_io.write_buffers) == 1

# At this point the TLS handshake wants to read, but we don't know that yet:
assert_poll(client_sock, client_io, _MP_STREAM_POLL_RD, _MP_STREAM_POLL_RD, 0)
assert_poll(client_sock, client_io, _MP_STREAM_POLL_WR, _MP_STREAM_POLL_WR, _MP_STREAM_POLL_WR)

# Do a test write
client_sock.write(b"foo")

# Now we know that we want to read:
assert_poll(client_sock, client_io, _MP_STREAM_POLL_RD, _MP_STREAM_POLL_RD, 0)
assert_poll(client_sock, client_io, _MP_STREAM_POLL_WR, _MP_STREAM_POLL_RD, 0)

# Unblock reads and nudge the two sockets:
client_io.block_reads = False
while server_io.write_buffers or client_io.write_buffers:
    if server_io.write_buffers:
        assert client_sock.read(128) is None
    if client_io.write_buffers:
        assert server_sock.read(128) is None

# At this point, the handshake is done, try writing data:
client_sock.write(b"foo")
assert server_sock.read(3) == b"foo"

# Test reading partial data:
client_sock.write(b"foobar")
assert server_sock.read(3) == b"foo"
server_io.block_reads = True
assert_poll(
    server_sock, server_io, _MP_STREAM_POLL_RD, None, _MP_STREAM_POLL_RD
)  # Did not go to the socket, just consumed buffered data
assert server_sock.read(3) == b"bar"


# Polling on a closed socket errors out:
client_io, _ = _Pipe.new_pair()
client_sock = ussl.wrap_socket(client_io, do_handshake=False)
client_sock.close()
assert_poll(
    client_sock, client_io, _MP_STREAM_POLL_RD, None, _MP_STREAM_POLL_NVAL
)  # Did not go to the socket


# Errors propagates to poll:
client_io, server_io = _Pipe.new_pair()
client_sock = ussl.wrap_socket(client_io, do_handshake=False)

# The server returns garbage:
server_io.write(b"fooba")  # Needs to be exactly 5 bytes

assert_poll(client_sock, client_io, _MP_STREAM_POLL_RD, _MP_STREAM_POLL_RD, _MP_STREAM_POLL_RD)
assert_raises(client_sock.read, 128)
assert_poll(
    client_sock, client_io, _MP_STREAM_POLL_RD, None, _MP_STREAM_POLL_NVAL
)  # Did not go to the socket

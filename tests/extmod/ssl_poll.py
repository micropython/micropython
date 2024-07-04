try:
    import select
    import ssl
    import io
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
# They has been generated using the following commands:
# $ openssl req -x509 -newkey rsa:1024 -keyout rsa_key.pem -out rsa_cert.pem -days 3650 -nodes -subj '/CN=micropython.local/O=MicroPython/C=AU'
# $ openssl pkey -in rsa_key.pem -out rsa_key.der -outform DER
# $ openssl x509 -in rsa_cert.pem -out rsa_cert.der -outform DER
key = bytes.fromhex(
    "3082025d02010002818100eca28b2f8230237ae45e7a77ef495c05a786f423cc65caf6bc"
    "1813d50eacf9d2d011a0e43a20fde947ff957075e4b3b6ded46c33f63af42597aac1c4cb"
    "bb2d1a6aad91755707d8fc560e222b38c940a480da89ee849049613bd88d1ab283423aba"
    "ba591c4fcc8ce89d19646e6b9f80de4efa7bbda68c569a5cbdb4235c3fc9990203010001"
    "0281810089d4df978b90388a534c88af252ca8b20e7377ef0616609338da196b27422fbd"
    "d03e04660b49be3bcc191dd5448632fb986d489d3795fd318c5704c879168c5cd0fa8551"
    "f7f86073b95b092ffdb4f39c867a306a02f94cf3009df7055dab1f9277dd8914268d53a2"
    "bd4de2cdf2ac90d8cd248b48868cb911781779750c344ae5024100f8c0d2cebfaccbdb1b"
    "d8bc7519c84889cc6d5dff8fb994cf1a9492881289de66d689afb942d10dc0dc0f65464e"
    "7cca7a53e451e6a8cfab0069d05065d56ba4bf024100f38757e2fc7f786e1a653c8a8b51"
    "5b06a1d85db31998090fc4d52a88b8c5557e0a7bac10995e7e76cef6bfb59b67c01cbba7"
    "edcfa7d3d623615a92459d07efa702407b149579fcf717caeb455b4229a6a2d5a3d3bd7d"
    "d4f4833fd22c0f30cad372bab98e58e736bb4fadcc74c5ac7aeb5e1816e852e9e93f0fa2"
    "5db8d7fefb118eeb024100caf0a0a6c1b02055f09d28c473b10a600e8356222853f04939"
    "c84237c97278fa1e164d9f4f8fd56780b553b12d9e5a1417b5ab91fed3a381bc6153bfbb"
    "4a9fc5024043eb4ff7e4dd6c12c6d6dc50977ee5d5f9730af4469d1f642321320fb4b969"
    "90ed841e41bedda49ff89a0c28acf132e4af5f3ac406b1548d0f135aff6bf23ef1"
)
cert = bytes.fromhex(
    "3082025a308201c3a003020102021475fd479918562f4c6cb08f63e8afbdfa3b884f8630"
    "0d06092a864886f70d01010b0500303f311a301806035504030c116d6963726f70797468"
    "6f6e2e6c6f63616c31143012060355040a0c0b4d6963726f507974686f6e310b30090603"
    "55040613024155301e170d3234303730323030353931355a170d33343036333030303539"
    "31355a303f311a301806035504030c116d6963726f707974686f6e2e6c6f63616c311430"
    "12060355040a0c0b4d6963726f507974686f6e310b300906035504061302415530819f30"
    "0d06092a864886f70d010101050003818d0030818902818100eca28b2f8230237ae45e7a"
    "77ef495c05a786f423cc65caf6bc1813d50eacf9d2d011a0e43a20fde947ff957075e4b3"
    "b6ded46c33f63af42597aac1c4cbbb2d1a6aad91755707d8fc560e222b38c940a480da89"
    "ee849049613bd88d1ab283423ababa591c4fcc8ce89d19646e6b9f80de4efa7bbda68c56"
    "9a5cbdb4235c3fc9990203010001a3533051301d0603551d0e04160414409545477a659a"
    "16da174810ba9ad192ef962089301f0603551d23041830168014409545477a659a16da17"
    "4810ba9ad192ef962089300f0603551d130101ff040530030101ff300d06092a864886f7"
    "0d01010b0500038181007182e78cecceef00f98d0ee117cd9dc2f9fc84d581e7b1d9d43b"
    "74db45e188368e264f79628e2bda89a545d50cd9925ad50f8e25decc9130164bdb9220c8"
    "f49776d784511e9c4b94305cc2cb3eaf8204e42d31ba8aabd1d296b9ef51035b6df1ab75"
    "89681f0026073ccac6bed5d8bd9235a4bb717b696ce518de4e35b751fa13"
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
client_sock = ssl.wrap_socket(client_io, do_handshake=False)

server_sock = ssl.wrap_socket(server_io, key=key, cert=cert, server_side=True, do_handshake=False)

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
client_sock = ssl.wrap_socket(client_io, do_handshake=False)
client_sock.close()
assert_poll(
    client_sock, client_io, _MP_STREAM_POLL_RD, None, _MP_STREAM_POLL_NVAL
)  # Did not go to the socket


# Errors propagates to poll:
client_io, server_io = _Pipe.new_pair()
client_sock = ssl.wrap_socket(client_io, do_handshake=False)

# The server returns garbage:
server_io.write(b"fooba")  # Needs to be exactly 5 bytes

assert_poll(client_sock, client_io, _MP_STREAM_POLL_RD, _MP_STREAM_POLL_RD, _MP_STREAM_POLL_RD)
assert_raises(client_sock.read, 128)
assert_poll(
    client_sock, client_io, _MP_STREAM_POLL_RD, None, _MP_STREAM_POLL_NVAL
)  # Did not go to the socket

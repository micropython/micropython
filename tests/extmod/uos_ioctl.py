import uos
import uio

class TestIO(uio.IOBase):
    def __init__(self):
        self._read_rdy = False
        self._write_rdy = False

    def ioctl(self, req, arg):
        if req == uos.STREAM_POLL:
            ret = 0
            if arg & uos.STREAM_POLL_RD and self._read_rdy:
                ret |= uos.STREAM_POLL_RD
            if arg & uos.STREAM_POLL_WR and self._write_rdy:
                ret |= uos.STREAM_POLL_WR
            return ret

        raise Exception("Foo")


f = TestIO()

def _assert_ioctl_poll(expected):
    assert f.ioctl(uos.STREAM_POLL, uos.STREAM_POLL_RD | uos.STREAM_POLL_WR) == expected

_assert_ioctl_poll(0)
f._read_rdy = True
_assert_ioctl_poll(uos.STREAM_POLL_RD)
f._write_rdy = True
_assert_ioctl_poll(uos.STREAM_POLL_RD | uos.STREAM_POLL_WR)

try:
    uos.ioctl(f, 666, 0)
except Exception as exc:
    assert repr(exc) == "Exception('Foo',)"
else:
    assert False

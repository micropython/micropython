try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    import uio as io
except ImportError:
    try:
        import io
    except ImportError:
        print("SKIP")
        raise SystemExit

MP_STREAM_POLL_RD = 1
MP_STREAM_POLL = 3
MP_STREAM_ERROR = -1


class MyDriver(io.IOBase):
    def __init__(self):
        pass

    def readinto(self, buf):
        buf[0] = 99
        return 1

    def ioctl(self, req, arg):
        ret = MP_STREAM_ERROR
        if req == MP_STREAM_POLL:
            ret = 0
            if arg & MP_STREAM_POLL_RD:
                ret |= MP_STREAM_POLL_RD
        return ret


async def main():
    print("start")

    driver = MyDriver()
    sreader = asyncio.StreamReader(driver)
    buf = bytearray(1)
    num_bytes_from_readinto = await sreader.readinto(buf)
    print(num_bytes_from_readinto)
    print(buf[0])

    print("finish")


asyncio.run(main())

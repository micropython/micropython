# AIOESPNow module for MicroPython on ESP32
# MIT license; Copyright (c) 2022 Glenn Moloney @glenn20

import uasyncio
from esp import espnow


# Modelled on the uasyncio.Stream class (extmod/stream/stream.py)
# NOTE: Relies on internal implementation of uasyncio.core (_io_queue)
class AIOESPNow(espnow.ESPNow):
    def __init__(self, e=None):
        super().__init__()

    # Read one ESPNow message
    async def arecv(self):
        yield uasyncio.core._io_queue.queue_read(self)
        return self.recv(0)

    async def airecv(self):
        yield uasyncio.core._io_queue.queue_read(self)
        return self.irecv(0)

    async def asend(self, mac, msg=None, sync=True):
        if msg is None:
            msg, mac = mac, None  # If msg is None: swap mac and msg
        yield uasyncio.core._io_queue.queue_write(self)
        return self.send(mac, msg, sync)

    # async for support
    def __aiter__(self):
        return self

    async def __anext__(self):
        return await self.airecv()


# Convenience function to support:
#   import aioespnow as espnow
def ESPNow():
    return AIOESPNow()


MAX_DATA_LEN = espnow.MAX_DATA_LEN
KEY_LEN = espnow.KEY_LEN
MAX_TOTAL_PEER_NUM = espnow.MAX_TOTAL_PEER_NUM
MAX_ENCRYPT_PEER_NUM = espnow.MAX_ENCRYPT_PEER_NUM

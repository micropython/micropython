# espnow module for MicroPython on ESP8266
# MIT license; Copyright (c) 2022 Glenn Moloney @glenn20

from _espnow import *
from uselect import poll, POLLIN


class ESPNow(ESPNowBase):
    # Static buffers for alloc free receipt of messages with ESPNow.irecv().
    _data = [bytearray(ADDR_LEN), bytearray(MAX_DATA_LEN)]
    _none_tuple = (None, None)

    def __init__(self):
        super().__init__()
        self._poll = poll()  # For any() method below...
        self._poll.register(self, POLLIN)

    def irecv(self, timeout_ms=None):
        n = self.recvinto(self._data, timeout_ms)
        return self._data if n else self._none_tuple

    def recv(self, timeout_ms=None):
        n = self.recvinto(self._data, timeout_ms)
        return [bytes(x) for x in self._data] if n else self._none_tuple

    def __iter__(self):
        return self

    def __next__(self):
        return self.irecv()  # Use alloc free irecv() method

    def any(self):  # For the ESP8266 which does not have ESPNow.any()
        try:
            next(self._poll.ipoll(0))
            return True
        except StopIteration:
            return False

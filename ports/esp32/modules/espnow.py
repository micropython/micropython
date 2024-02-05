# espnow module for MicroPython on ESP32
# MIT license; Copyright (c) 2022 Glenn Moloney @glenn20

from _espnow import *


class ESPNow(ESPNowBase):
    # Static buffers for alloc free receipt of messages with ESPNow.irecv().
    _data = [None, bytearray(MAX_DATA_LEN)]
    _none_tuple = (None, None)

    def __init__(self):
        super().__init__()

    def irecv(self, timeout_ms=None):
        n = self.recvinto(self._data, timeout_ms)
        return self._data if n else self._none_tuple

    def recv(self, timeout_ms=None):
        n = self.recvinto(self._data, timeout_ms)
        return [bytes(x) for x in self._data] if n else self._none_tuple

    def irq(self, callback):
        super().irq(callback, self)

    def __iter__(self):
        return self

    def __next__(self):
        return self.irecv()  # Use alloc free irecv() method

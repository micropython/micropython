from observer import Subject, Observer
from log import Log
import pyvesc
from pyvesc.VESC.messages import GetValues_mp


class VESC(Subject):

    response = None
    _uart = None
    _get_values_msg = None
    m_get_values_pkt = None
    _count = 0
    _buffer = None
    _observers: List[Observer] = []

    def __init__(self, uart):
        self._uart = uart
        self._get_values_msg = GetValues_mp()
        self.m_get_values_pkt = pyvesc.encode_request_mp(self._get_values_msg)
        Log(self.m_get_values_pkt)

    def attach(self, observer: Observer) -> None:
        Log("VESC: Attached an observer.")
        self._observers.append(observer)

    def detach(self, observer: Observer) -> None:
        self._observers.remove(observer)

    def notify(self) -> None:
        """
        Trigger an update in each subscriber.
        """

        Log(4,"Subject: Notifying observers...")
        for observer in self._observers:
            observer.update(self)

    def update(self) -> int:
        res = 1
        if self._count < 1:
            self._uart.write(self.m_get_values_pkt)
            self._count = 10
            # Log("Write usrt")

        length = self._uart.any()
        # Log(str(len))
        if length > 61:
            self._buffer = self._uart.read(length)
            if len(self._buffer) > 61:
                try:
                    (self.response, consumed) = pyvesc.decode_mp(self._get_values_msg, self._buffer)
                except OSError:
                    Log("failed to decode_mp")

                if self.response:
                    self.notify()
                    self._count = 0
        else:
            self._count -= 1
            res = 0
        return res

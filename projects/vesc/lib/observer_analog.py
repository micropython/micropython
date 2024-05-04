
from observer import Subject, Observer
from machine import Pin, ADC
from random import randrange
from log import Log


class AnalogPins(Subject):
    """
    The Subject owns some important state and notifies observers when the state
    changes.
    """
   
    _state: int = None
    _values = []
    
    """
    For the sake of simplicity, the Subject's state, essential to all
    subscribers, is stored in this variable.
    """

    _observers: List[Observer] = []
    _converters = []
    _pins = []

    """
    List of subscribers. In real life, the list of subscribers can be stored
    more comprehensively (categorized by event type, etc.).
    """
    def interruption_handler(self, timer):
        self.update()
        
    def __init__(self, pins):
        self._pins = pins
        for idx, pin in enumerate(pins):
            Log(f"idx={idx} pin={pin}")
            self._converters.append(ADC(Pin(pin)))
            self._values.append(0)
            
        for converter in self._converters:
            converter.atten(converter.ATTN_11DB)

    def attach(self, observer: Observer) -> None:
        Log("Analog: Attached an observer.")
        self._observers.append(observer)

    def detach(self, observer: Observer) -> None:
        self._observers.remove(observer)

    """
    The subscription management methods.
    """

    def notify(self) -> None:
        """
        Trigger an update in each subscriber.
        """

        Log(4,"Subject: Notifying observers...")
        for observer in self._observers:
            observer.update(self)

    def update(self) -> None:
        """
        Usually, the subscription logic is only a fraction of what a Subject can
        really do. Subjects commonly hold some important business logic, that
        triggers a notification method whenever something important is about to
        happen (or after it).
        """

        self._state = randrange(0, 10)
        for idx, converter in enumerate(self._converters):
            self._values[idx] = converter.read_uv()
           
        """
        Log(f"Subject: My state has just changed to: {self._state}")
        for idx,value in enumerate(self._values):
            Log(f"Subject: My value {self._pins[idx]} has just changed to: {value}")
        """
        self.notify()


class ConcreteAnalogObserver(Observer):
    def update(self, subject: Subject) -> None:
        for idx, value in enumerate(subject._values):
            Log(f"Observer: My value for pin {subject._pins[idx]} has just changed to: {value}")

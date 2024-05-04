# This example demonstrates a UART periperhal.

import bluetooth
import random
import struct
from ble_advertising import advertising_payload
from observer import Subject, Observer
from log import Log

from micropython import const

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_MTU_EXCHANGED = const(21)

_FLAG_READ = const(0x0002)
_FLAG_WRITE_NO_RESPONSE = const(0x0004)
_FLAG_WRITE = const(0x0008)
_FLAG_NOTIFY = const(0x0010)

_VOLTAGE_IO_UUID = bluetooth.UUID("26b93bf8-5051-11ee-be56-0242ac120002")
_VOLTAGE_IN = (bluetooth.UUID("26b93bf8-5051-11ee-be56-0242ac120003"),_FLAG_READ|_FLAG_NOTIFY,)
_VOLTAGE_OUT = (bluetooth.UUID("26b93bf8-5051-11ee-be56-0242ac120004"),_FLAG_WRITE|_FLAG_WRITE_NO_RESPONSE,)

_NEOPIXEL = (bluetooth.UUID("26b93bf8-5051-11ee-be56-0242ac120005"), _FLAG_WRITE|_FLAG_WRITE_NO_RESPONSE,)
_IIC = (bluetooth.UUID("26b93bf8-5051-11ee-be56-0242ac120006"), _FLAG_WRITE| _FLAG_WRITE_NO_RESPONSE,)

_UART_UUID = bluetooth.UUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
_UART_TX = (
    bluetooth.UUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E"),
    _FLAG_READ | _FLAG_NOTIFY,
)
_UART_RX = (
    bluetooth.UUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E"),
    _FLAG_WRITE | _FLAG_WRITE_NO_RESPONSE,
)
_UART_SERVICE = (
    _UART_UUID,
    (_UART_TX, _UART_RX),
)

_VOLTAGE_IO_SERVICE = (_VOLTAGE_IO_UUID, (_VOLTAGE_IN, _VOLTAGE_OUT, _NEOPIXEL, _IIC),)


class BLESimplePeripheral(Observer):
    def __init__(self, ble, name="mpy-uart"):
        self._ble = ble
        self._ble.active(True)
        self._ble.config(mtu=500)
        # self._ble.config(rxbuf=512)
        self._ble.irq(self._irq)
        self.ao = self.analog_observer(self)
        self.vo = self.vesc_observer(self)
    
        ((self._handle_tx, self._handle_rx,), (self._handle_i, self._handle_o, self._handle_np, self._handle_iic), ) = self._ble.gatts_register_services((_UART_SERVICE, _VOLTAGE_IO_SERVICE, ))
        self._connections = set()
        self._write_uart_callback = None
        self._write_neopixel_callback = None
        self._payload = advertising_payload(name=name, services=[_UART_UUID])
        self._advertise()

    def _irq(self, event, data):
        # Track connections so we can send notifications.
        if event == _IRQ_CENTRAL_CONNECT:
            conn_handle, _, _ = data
            Log("New connection", conn_handle)
            self._connections.add(conn_handle)
            self._advertise()
        elif event == _IRQ_CENTRAL_DISCONNECT:
            conn_handle, _, _ = data
            Log("Disconnected", conn_handle)
            self._connections.remove(conn_handle)
            # Start advertising again to allow a new connection.
            self._advertise()
        elif event == _IRQ_GATTS_WRITE:
            conn_handle, value_handle = data
            value = self._ble.gatts_read(value_handle)
            if value_handle == self._handle_rx and self._write_uart_callback:
                self._write_uart_callback(value)
            elif value_handle == self._handle_np and self._write_neopixel_callback:
                self._write_neopixel_callback(value)
            elif value_handle == self._handle_iic and self._write_iic_callback:
                self._write_iic_callback(value)
        elif event == _IRQ_MTU_EXCHANGED:
            # ATT MTU exchange complete (either initiated by us or the remote device).
            conn_handle, mtu = data
            print("mtu="+str(data))
           
    def send_uart(self, data):
        for conn_handle in self._connections:
            self._ble.gatts_notify(conn_handle, self._handle_tx, data)   

    def is_connected(self):
        return len(self._connections) > 0

    def _advertise(self, interval_us=500000):
        Log("Starting advertising")
        self._ble.gap_advertise(interval_us, adv_data=self._payload)

    def on_write_uart(self, callback):
        self._write_uart_callback = callback

    def on_write_neopixel(self, callback):
        self._write_neopixel_callback = callback

    def on_write_iic(self, callback):
        self._write_iic_callback = callback

    def update(self, subject: Subject) -> None:
        # (f"ble Observer: My value for pin {subject._pins[idx]} has just changed to: {value}")
        data = struct.pack("<ll", subject._values[0], subject._values[1])
        for conn_handle in self._connections:
            try:
                self._ble.gatts_notify(conn_handle, self._handle_i, data)
            except OSError:
                Log(1, "failed to update with gatts_notify")

    class analog_observer(Observer):

        def __init__(self, parent):
            self.parent = parent
        
        def update(self, subject: Subject) -> None:
            data = struct.pack("<ll", subject._values[0], subject._values[1])

            for conn_handle in self.parent._connections:               
                try:
                    self.parent._ble.gatts_notify(conn_handle, self.parent._handle_i, data)
                except OSError:
                    Log(1, "failed to update with gatts_notify")

    class vesc_observer(Observer):

        def __init__(self, parent):
            self.parent = parent
            
        def update(self, subject: Subject) -> None:
            buffer = subject._buffer
            self.parent.send_uart(buffer)
               

def ble_main(hwuart, sa=0, np=0, iic=0):
    ble = bluetooth.BLE()
    p = BLESimplePeripheral(ble)
   
    if sa != 0:
        sa.attach(p)

    def on_rx(v):
        Log(4, "RX", v)
        hwuart.write(v)

    def on_n(v):
        if np != 0:
            np.buf = v
            np.write()

    def on_iic(var):
        if iic:
            unpacked = bytearray(var)
            iic.writeto(unpacked[0],unpacked[1:])
            #print(unpacked)

    p.on_write_iic(on_iic)

    p.on_write_uart(on_rx)
    
    p.on_write_neopixel(on_n)

    return p

    
        



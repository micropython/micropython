# Test calling BLE methods from within the BLE.irq event handler.

from micropython import const
import struct
import time
import bluetooth

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_DESCRIPTOR_RESULT = const(13)
_IRQ_GATTC_DESCRIPTOR_DONE = const(14)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_MTU_EXCHANGED = const(21)
_IRQ_GET_SECRET = const(29)
_IRQ_SET_SECRET = const(30)

EVENT_NAMES = {
    1: "_IRQ_CENTRAL_CONNECT",
    2: "_IRQ_CENTRAL_DISCONNECT",
    3: "_IRQ_GATTS_WRITE",
    4: "_IRQ_GATTS_READ_REQUEST",
    7: "_IRQ_PERIPHERAL_CONNECT",
    8: "_IRQ_PERIPHERAL_DISCONNECT",
    9: "_IRQ_GATTC_SERVICE_RESULT",
    10: "_IRQ_GATTC_SERVICE_DONE",
    11: "_IRQ_GATTC_CHARACTERISTIC_RESULT",
    12: "_IRQ_GATTC_CHARACTERISTIC_DONE",
    13: "_IRQ_GATTC_DESCRIPTOR_RESULT",
    14: "_IRQ_GATTC_DESCRIPTOR_DONE",
    15: "_IRQ_GATTC_READ_RESULT",
    16: "_IRQ_GATTC_READ_DONE",
    17: "_IRQ_GATTC_WRITE_DONE",
    18: "_IRQ_GATTC_NOTIFY",
    21: "_IRQ_MTU_EXCHANGED",
}

_ADV_TYPE_FLAGS = const(0x01)
_ADV_TYPE_NAME = const(0x09)
_ADV_TYPE_UUID128_COMPLETE = const(0x7)

_NOTIFY_ENABLE = const(1)

ACCESSORY_UUID = bluetooth.UUID("a5a5a5a5-ffff-9999-1111-5a5a5a5a5a5a")
STATE_UUID = bluetooth.UUID("a5a5a5a5-eeee-9999-1111-5a5a5a5a5a5a")
CCC_UUID = bluetooth.UUID(0x2902)

STATE_CHARACTERISTIC = (
    STATE_UUID,
    bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY,
)

ACCESSORY_SERVICE = (ACCESSORY_UUID, (STATE_CHARACTERISTIC,))


def recursive_function():
    recursive_function()


def test_deep_calls(n):
    if n > 0:
        # Test that a few nested calls will succeed.
        test_deep_calls(n - 1)
    else:
        # Test that a Python stack overflow is detected.
        try:
            recursive_function()
        except RuntimeError:
            print("test_deep_calls finished")


class Central:
    def __init__(self):
        self.done = False
        self._conn_handle = None
        self._service = None
        self._characteristic_handle = None
        self._cccd_handle = None
        self._reads_remaining = None
        ble.active(1)
        ble.irq(self._ble_event_handler)
        ble.gap_connect(*BDADDR)

    def _ble_event_handler(self, event, data):
        print(EVENT_NAMES[event])

        if event == _IRQ_PERIPHERAL_CONNECT:
            conn_handle, _, _ = data
            self._conn_handle = conn_handle
            ble.gattc_discover_services(self._conn_handle, ACCESSORY_UUID)
            # Test deep Python calls from within this BLE IRQ handler.
            test_deep_calls(5)

        elif event == _IRQ_PERIPHERAL_DISCONNECT:
            conn_handle, _, addr = data
            assert self._conn_handle == conn_handle
            self._conn_handle = None
            print("connection closed")

        elif event == _IRQ_GATTC_SERVICE_RESULT:
            _, first_handle, last_handle, uuid = data
            print("service found:", last_handle - first_handle, uuid)
            if uuid == ACCESSORY_UUID:
                assert self._service is None
                self._service = (first_handle, last_handle)

        elif event == _IRQ_GATTC_SERVICE_DONE:
            print("service handle range:", self._service[1] - self._service[0])
            start_handle, end_handle = self._service
            ble.gattc_discover_characteristics(self._conn_handle, start_handle, end_handle)

        elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
            _, end_handle, value_handle, properties, uuid = data
            assert uuid == STATE_UUID
            print("characteristic found:", uuid)
            assert self._characteristic_handle is None
            self._characteristic_handle = value_handle

        elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
            start_handle, end_handle = self._service
            ble.gattc_discover_descriptors(self._conn_handle, start_handle, end_handle)

        elif event == _IRQ_GATTC_DESCRIPTOR_RESULT:
            _, dsc_handle, uuid = data
            if uuid == CCC_UUID:
                print("CCCD found:", uuid)
                assert self._cccd_handle is None
                self._cccd_handle = dsc_handle

        elif event == _IRQ_GATTC_DESCRIPTOR_DONE:
            # Discovery complete, proceed to MTU exchange.
            ble.gattc_exchange_mtu(self._conn_handle)

        elif event == _IRQ_MTU_EXCHANGED:
            # MTU exchanged, proceed to enable CCCD.
            print("CCCD write")
            ble.gattc_write(
                self._conn_handle, self._cccd_handle, struct.pack("<h", _NOTIFY_ENABLE), 1
            )

        elif event == _IRQ_GATTC_WRITE_DONE:
            conn_handle, _, result = data
            print("CCCD write result:", result)
            print("issue gattc_read")
            self._reads_remaining = 2
            ble.gattc_read(self._conn_handle, self._characteristic_handle)

        elif event == _IRQ_GATTC_READ_RESULT:
            _, _, char_data = data
            print("gattc_read result:", bytes(char_data))

        elif event == _IRQ_GATTC_READ_DONE:
            self._reads_remaining -= 1
            if self._reads_remaining > 0:
                ble.gattc_read(self._conn_handle, self._characteristic_handle)
            else:
                self.done = True
                ble.gap_disconnect(self._conn_handle)


class Peripheral:
    def __init__(self):
        self.done = False
        ble.active(1)
        ble.irq(self._ble_event_handler)
        ble.gatts_register_services((ACCESSORY_SERVICE,))
        add_payload = self.advertising_payload("acc", (ACCESSORY_UUID,))
        ble.gap_advertise(500000, add_payload)

    def advertising_payload(self, name, services):
        payload = bytearray()

        def _append(adv_type, value):
            nonlocal payload
            payload.extend(struct.pack("BB", len(value) + 1, adv_type) + value)

        _append(_ADV_TYPE_FLAGS, struct.pack("B", 0x02 + 0x04))
        _append(_ADV_TYPE_NAME, name)

        for uuid in services:
            b = bytes(uuid)
            assert len(b) == 16
            _append(_ADV_TYPE_UUID128_COMPLETE, b)

        return payload

    def _ble_event_handler(self, event, data):
        if event not in (_IRQ_GET_SECRET, _IRQ_SET_SECRET):
            print(EVENT_NAMES[event])
        if event == _IRQ_CENTRAL_DISCONNECT:
            self.done = True


# Acting in peripheral role.
def instance0():
    print("peripheral start")
    peripheral = Peripheral()
    multitest.globals(BDADDR=ble.config("mac"))
    multitest.next()
    while not peripheral.done:
        time.sleep_ms(100)
    multitest.broadcast("finished")
    ble.active(0)


# Acting in central role.
def instance1():
    print("central start")
    multitest.next()
    central = Central()
    while not central.done:
        time.sleep_ms(100)
    multitest.wait("finished")
    ble.active(0)


ble = bluetooth.BLE()

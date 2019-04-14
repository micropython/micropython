# -*- coding: utf-8 -*-
"""
Example usage on ESP8266:

    import g201s
    device = g201s.G201S("E5:FB:01:09:F7:B")
    device.turn_on()
    #device.turn_off()
    #device.set_temperature(50)
    #device.get_temperature()
"""

import upygatt
from binascii import hexlify
import time
import machine
import gc

#ADDRESS_TYPE = pygatt.BLEAddressType.random

class G201S():
    def __init__(self, address):
        self.address = address
        self.adapter = upygatt.GATTToolBackend()
        self.index = 0
        self.cur_temp = 0
        self.bt_is_started = False

    def write_handle(self, handle, value, increment=True):
        val = []
        if handle == 0x000e:
            val.append(0x55)
            val.append(self.index)
            for i in value:
                val.append(i)
            val.append(0xAA)
        else:
            for i in value:
                val.append(i)

        val = bytearray(val)
        self.adapter.char_write_handle(handle=handle, value=val, wait_for_response=True)
        if increment: self.index += 1

    def turn_on(self):
        self.index = 0
        try:
            self.adapter.start()
            time.sleep_ms(500)
            self.bt_is_started = True

            while self.adapter.connect(self.address) != 0:
                pass

            self.write_handle(0x000e, [0xFF, 0xDF, 0x24, 0x0E, 0xC6, 0x94, 0xD1, 0x97, 0x43], False)
            self.write_handle(0x000c, [0x01, 0x00])
            self.write_handle(0x000e, [0x01])
            self.write_handle(0x000e, [0x05, 0x00, 0x00, int(hex(95), 16), 0x00])
            self.write_handle(0x000e, [0x03])
            self.adapter.disconnect(self.address)
            self.adapter.stop()

        except Exception as e:
            machine.reset()

        finally:
            time.sleep_ms(500)
            gc.collect()

    def turn_off(self):
        self.index = 0
        try:
            self.adapter.start()
            time.sleep_ms(500)
            self.bt_is_started = True

            while self.adapter.connect(self.address) != 0:
                pass

            self.write_handle(0x000e, [0xFF, 0xDF, 0x24, 0x0E, 0xC6, 0x94, 0xD1, 0x97, 0x43], False)
            self.write_handle(0x000c, [0x01, 0x00])
            self.write_handle(0x000e, [0x01])
            self.write_handle(0x000e, [0x04])
            #self.cur_temp = self.get_temperature()
            #print("Tried to switch off, returned {}".format(self.cur_temp))
            self.adapter.disconnect(self.address)
            self.adapter.stop()

        except Exception as e:
            machine.reset()

        finally:
            time.sleep_ms(500)
            gc.collect()

    def set_temperature(self, value):
        self.index = 0
        print("set temperature {}°C".format(value))
        # try:
        #     adapter.start()
        #     time.sleep_ms(500)
        #     device = adapter.connect(self.address, address_type=ADDRESS_TYPE)
        #     self.write_handle(device, 0x000e, [0xFF, 0xDF, 0x24, 0x0E, 0xC6, 0x94, 0xD1, 0x97, 0x43], False)
        #     self.write_handle(device, 0x000c, [0x01, 0x00])
        #     self.write_handle(device, 0x000e, [0x01])
        #     self.write_handle(device, 0x000e, [0x05, 0x00, 0x00, int(hex(value), 16), 0x00])
        #     self.write_handle(device, 0x000e, [0x03])
        #     self.get_temperature()
        # except Exception as e:
        #     machine.reset()
        # finally:
        #     adapter.stop()
        #     time.sleep_ms(500)
        #     gc.collect()
        if self.get_temperature() < value:
            self.turn_on()
            while True:
                if self.get_temperature() == value:
                    self.turn_off()
                    break
        else:
            print('{}°C'.format(self.get_temperature()))

    def get_temperature(self):
        self.index = 0
        try:
            self.adapter.start()
            time.sleep_ms(500)
            self.bt_is_started = True

            while self.adapter.connect(self.address) != 0:
                pass

            self.write_handle(0x000e, [0xFF, 0xDF, 0x24, 0x0E, 0xC6, 0x94, 0xD1, 0x97, 0x43], False)
            self.write_handle(0x000c, [0x01, 0x00])
            self.write_handle(0x000e, [0x01])
            self.write_handle(0x000e, [0x06])
            time.sleep_ms(500)
            value = self.adapter.char_read(uuid="6e400003-b5a3-f393-e0a9-e50e24dcca9e", value_handle=0x000b)
            value = hexlify(value)
            value = value[16:18]
            self.cur_temp = int(value, 16)
            print("Current temperature of kettle is {}°C".format(self.cur_temp))
            self.adapter.disconnect(self.address)
            self.adapter.stop()

        except Exception as e:
            machine.reset()

        finally:
            time.sleep_ms(500)
            gc.collect()
            return self.cur_temp

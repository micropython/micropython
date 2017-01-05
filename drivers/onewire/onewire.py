"""
OneWire library ported to MicroPython by Jason Hildebrand.


TODO: 
  * implement and test parasite-power mode (as an init option)
  * port the crc checks

The original upstream copyright and terms follow.
------------------------------------------------------------------------------

Copyright (c) 2007, Jim Studt (original old version - many contributors since)

OneWire has been maintained by Paul Stoffregen (paul@pjrc.com) since
January 2010.

26 Sept 2008 -- Robin James

Jim Studt's original library was modified by Josh Larios.

Tom Pollard, pollard@alum.mit.edu, contributed around May 20, 2008

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Much of the code was inspired by Derek Yerger's code, though I don't
think much of that remains.  In any event that was..
    (copyleft) 2006 by Derek Yerger - Free to distribute freely.
"""

import pyb
from pyb import disable_irq
from pyb import enable_irq

class OneWire:
    def __init__(self, pin):
        """
        Pass the data pin connected to your one-wire device(s), for example Pin('X1').
        The one-wire protocol allows for multiple devices to be attached.
        """
        self.data_pin = pin
        self.write_delays = (1, 40, 40, 1)
        self.read_delays = (1, 1, 40)

        # cache a bunch of methods and attributes.  This is necessary in _write_bit and 
        # _read_bit to achieve the timing required by the OneWire protocol.
        self.cache = (pin.init, pin.value, pin.OUT_PP, pin.IN, pin.PULL_UP)

        pin.init(pin.IN, pin.PULL_UP)

    def reset(self):
        """
        Perform the onewire reset function.
        Returns 1 if a device asserted a presence pulse, 0 otherwise.

        If you receive 0, then check your wiring and make sure you are providing
        power and ground to your devices.
        """
        retries = 25
        self.data_pin.init(self.data_pin.IN, self.data_pin.PULL_UP)

        # We will wait up to 250uS for
        # the bus to come high, if it doesn't then it is broken or shorted
        # and we return a 0;

        # wait until the wire is high... just in case
        while True:
            if self.data_pin.value():
                break
            retries -= 1
            if retries == 0:
                raise OSError("OneWire pin didn't go high")
            pyb.udelay(10)

        #  pull the bus low for at least 480us
        self.data_pin.low()
        self.data_pin.init(self.data_pin.OUT_PP)
        pyb.udelay(480)

        # If there is a slave present, it should pull the bus low within 60us
        i = pyb.disable_irq()
        self.data_pin.init(self.data_pin.IN, self.data_pin.PULL_UP)
        pyb.udelay(70)
        presence = not self.data_pin.value()
        pyb.enable_irq(i)
        pyb.udelay(410)
        return presence

    def write_bit(self, value):
        """
        Write a single bit.
        """
        pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache
        self._write_bit(value, pin_init, pin_value, Pin_OUT_PP)

    def _write_bit(self, value, pin_init, pin_value, Pin_OUT_PP):
        """
        Write a single bit - requires cached methods/attributes be passed as arguments.
        See also write_bit()
        """
        d0, d1, d2, d3 = self.write_delays
        udelay = pyb.udelay
        if value:
            # write 1
            i = disable_irq()
            pin_value(0)
            pin_init(Pin_OUT_PP)
            udelay(d0)
            pin_value(1)
            enable_irq(i)
            udelay(d1)
        else:
            # write 0
            i = disable_irq()
            pin_value(0)
            pin_init(Pin_OUT_PP)
            udelay(d2)
            pin_value(1)
            enable_irq(i)
            udelay(d3)

    def write_byte(self, value):
        """
        Write a byte.  The pin will go tri-state at the end of the write to avoid
        heating in a short or other mishap.
        """
        pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache
        for i in range(8):
            self._write_bit(value & 1, pin_init, pin_value, Pin_OUT_PP)
            value >>= 1
        pin_init(Pin_IN, Pin_PULL_UP)

    def write_bytes(self, bytestring):
        """
        Write a sequence of bytes.
        """
        for byte in bytestring:
            self.write_byte(byte)

    def _read_bit(self, pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP):
        """
        Read a single bit - requires cached methods/attributes be passed as arguments.
        See also read_bit()
        """
        d0, d1, d2 = self.read_delays
        udelay = pyb.udelay
        pin_init(Pin_IN, Pin_PULL_UP) # TODO why do we need this?
        i = disable_irq()
        pin_value(0)
        pin_init(Pin_OUT_PP)
        udelay(d0)
        pin_init(Pin_IN, Pin_PULL_UP)
        udelay(d1)
        value = pin_value()
        enable_irq(i)
        udelay(d2)
        return value

    def read_bit(self):
        """
        Read a single bit.
        """
        pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache
        return self._read_bit(pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP)

    def read_byte(self):
        """
        Read a single byte and return the value as an integer.
        See also read_bytes()
        """
        pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache
        value = 0
        for i in range(8):
            bit = self._read_bit(pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP)
            value |= bit << i
        return value

    def read_bytes(self, count):
        """
        Read a sequence of N bytes.
        The bytes are returned as a bytearray.
        """
        s = bytearray(count)
        for i in range(count):
            s[i] = self.read_byte()
        return s

    def select_rom(self, rom):
        """
        Select a specific device to talk to.  Pass in rom as a bytearray (8 bytes).
        """
        assert len(rom) == 8, "ROM must be 8 bytes"
        self.reset()
        self.write_byte(0x55)  # ROM MATCH
        self.write_bytes(rom)

    def read_rom(self):
        """
        Read the ROM - this works if there is only a single device attached.
        """
        self.reset()
        self.write_byte(0x33)   # READ ROM
        rom = self.read_bytes(8)
        # TODO: check CRC of the ROM
        return rom

    def skip_rom(self):
        """
        Send skip-rom command - this works if there is only one device attached.
        """
        self.write_byte(0xCC)   # SKIP ROM

    def depower(self):
        self.data_pin.init(self.data_pin.IN, self.data_pin.PULL_NONE)

    def scan(self):
        """
        Return a list of ROMs for all attached devices. 
        Each ROM is returned as a bytes object of 8 bytes.
        """
        devices = []
        self._reset_search()
        while True:
            rom = self._search()
            if not rom:
                return devices
            devices.append(rom)

    def _reset_search(self):
        self.last_discrepancy = 0
        self.last_device_flag = False
        self.last_family_discrepancy = 0
        self.rom = bytearray(8) 
        
    def _search(self):
        # initialize for search
        id_bit_number = 1
        last_zero = 0
        rom_byte_number = 0
        rom_byte_mask = 1
        search_result = 0
        pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache

        # if the last call was not the last one
        if not self.last_device_flag:
            # 1-Wire reset
            if not self.reset():
                self._reset_search()
                return None

            # issue the search command
            self.write_byte(0xF0)

            # loop to do the search
            while rom_byte_number < 8:  # loop until through all ROM bytes 0-7
                # read a bit and its complement
                id_bit = self._read_bit(pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP)
                cmp_id_bit = self._read_bit(pin_init, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP)

                # check for no devices on 1-wire
                if (id_bit == 1) and (cmp_id_bit == 1):
                    break
                else:
                    # all devices coupled have 0 or 1
                    if (id_bit != cmp_id_bit):
                       search_direction = id_bit  # bit write value for search
                    else:
                       # if this discrepancy if before the Last Discrepancy
                       # on a previous next then pick the same as last time
                       if (id_bit_number < self.last_discrepancy):
                          search_direction = (self.rom[rom_byte_number] & rom_byte_mask) > 0
                       else:
                          # if equal to last pick 1, if not then pick 0
                          search_direction = (id_bit_number == self.last_discrepancy)
    
                       # if 0 was picked then record its position in LastZero
                       if search_direction == 0:
                           last_zero = id_bit_number
    
                           # check for Last discrepancy in family
                           if last_zero < 9:
                               self.last_family_discrepancy = last_zero

                    # set or clear the bit in the ROM byte rom_byte_number
                    # with mask rom_byte_mask
                    if search_direction == 1:
                        self.rom[rom_byte_number] |= rom_byte_mask
                    else:
                        self.rom[rom_byte_number] &= ~rom_byte_mask

                # serial number search direction write bit
                #print('sd', search_direction)
                self.write_bit(search_direction)

                # increment the byte counter id_bit_number
                # and shift the mask rom_byte_mask
                id_bit_number += 1
                rom_byte_mask <<= 1

                # if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
                if rom_byte_mask == 0x100:
                    rom_byte_number += 1
                    rom_byte_mask = 1

        # if the search was successful then
        if not (id_bit_number < 65):
            # search successful so set last_discrepancy,last_device_flag,search_result
            self.last_discrepancy = last_zero

            # check for last device
            if self.last_discrepancy == 0:
                self.last_device_flag = True
            search_result = True

        # if no device found then reset counters so next 'search' will be like a first
        if not search_result or not self.rom[0]:
            self._reset_search()
            return None
        else:
            return bytes(self.rom)

import pyb
from pyb import Pin
from pyb import Timer
from pyb import udelay
from pyb import disable_irq
from pyb import enable_irq

"""
OneWire library ported to MicroPython by Jason Hildebrand.

TODO: 
  * fix search() - which is currently broken.  This is needed to support multiple devices on a single bus
  * implement and test parasite-power mode (as an init option)
  * port the crc checks


The original upstream copyright and terms follow.
-------------------------------------------------------------------------------

Copyright (c) 2007, Jim Studt  (original old version - many contributors since)

The latest version of this library may be found at:
  http://www.pjrc.com/teensy/td_libs_OneWire.html

OneWire has been maintained by Paul Stoffregen (paul@pjrc.com) since
January 2010.  At the time, it was in need of many bug fixes, but had
been abandoned the original author (Jim Studt).  None of the known
contributors were interested in maintaining OneWire.  Paul typically
works on OneWire every 6 to 12 months.  Patches usually wait that
long.  If anyone is interested in more actively maintaining OneWire,
please contact Paul.

Version 2.2:
  Teensy 3.0 compatibility, Paul Stoffregen, paul@pjrc.com
  Arduino Due compatibility, http://arduino.cc/forum/index.php?topic=141030
  Fix DS18B20 example negative temperature
  Fix DS18B20 example's low res modes, Ken Butcher
  Improve reset timing, Mark Tillotson
  Add const qualifiers, Bertrik Sikken
  Add initial value input to crc16, Bertrik Sikken
  Add target_search() function, Scott Roberts

Version 2.1:
  Arduino 1.0 compatibility, Paul Stoffregen
  Improve temperature example, Paul Stoffregen
  DS250x_PROM example, Guillermo Lovato
  PIC32 (chipKit) compatibility, Jason Dangel, dangel.jason AT gmail.com
  Improvements from Glenn Trewitt:
  - crc16() now works
  - check_crc16() does all of calculation/checking work.
  - Added read_bytes() and write_bytes(), to reduce tedious loops.
  - Added ds2408 example.
  Delete very old, out-of-date readme file (info is here)

Version 2.0: Modifications by Paul Stoffregen, January 2010:
http://www.pjrc.com/teensy/td_libs_OneWire.html
  Search fix from Robin James
    http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295/27#27
  Use direct optimized I/O in all cases
  Disable interrupts during timing critical sections
    (this solves many random communication errors)
  Disable interrupts during read-modify-write I/O
  Reduce RAM consumption by eliminating unnecessary
    variables and trimming many to 8 bits
  Optimize both crc8 - table version moved to flash

Modified to work with larger numbers of devices - avoids loop.
Tested in Arduino 11 alpha with 12 sensors.
26 Sept 2008 -- Robin James
http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295/27#27

Updated to work with arduino-0008 and to include skip() as of
2007/07/06. --RJL20

Modified to calculate the 8-bit CRC directly, avoiding the need for
the 256-byte lookup table to be loaded in RAM.  Tested in arduino-0010
-- Tom Pollard, Jan 23, 2008

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

debug = False

class OneWire:
    
    def __init__(self, pin):
        """
        Pass the name of the data pin connected to your one-wire device(s), for example 'PD2'.
        The one-wire protocol allows for multiple devices to be attached, so you have a bus with
        multiple one-wire devices attached.
        """
        self.data = Pin(pin, Pin.IN, Pin.PULL_UP)

        # cache a bunch of methods and attributes.  This is necessary in _write_bit and 
        # read_byte to achieve the timing required by the OneWire protocol.
        self.cache = (self.data, self.data.init, self.data.low, self.data.high, self.data.value,
                      Pin.OUT_PP, Pin.IN, Pin.PULL_UP)
        self.write_delays = (1, 60, 60, 1)
        self.read_delays = (1, 1, 40)
        if debug:
            # TODO: pass timer ID as __init__ argument
            self.micros = Timer(2, prescaler=83, period=0x3fffffff) # 1MHz ~ 1uS
        self.LastDiscrepancy = 0
        self.LastDeviceFlag = False
        self.LastFamilyDiscrepancy = 0

    def reset(self):
        """
        Perform the onewire reset function.
        Returns 1 if a device asserted a presence pulse, 0 otherwise.

        If you receive 0, then check your wiring and make sure you are providing
        power and ground to your devices.
        """
        retries = 25
        self.data.init(Pin.IN, Pin.PULL_UP)

        # We will wait up to 250uS for
        # the bus to come high, if it doesn't then it is broken or shorted
        # and we return a 0;

        # wait until the wire is high... just in case
        while True:
            if self.data.value():
                break
            retries -= 1
            if retries == 0:
                print("pin didn't go high...")
                return 0
            udelay(10)

        #  pull the bus low for at least 480µs
        i = disable_irq()
        self.data.low()
        self.data.init(Pin.OUT_PP)
        enable_irq(i)
        udelay(480)

        # If there is a slave present, it should pull the bus low within 60Âµs
        i = disable_irq()
        self.data.init(Pin.IN, Pin.PULL_UP)
        udelay(70)
        presence = not self.data.value()
        enable_irq(i)
        udelay(410)
        return presence


    def write_bit(self, value):
        """
        Write a single bit.
        """
        pin, pin_init, pin_low, pin_high, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache
        self._write_bit(value, pin, pin_init, pin_low, pin_high, Pin_OUT_PP)

    def _write_bit(self, value, pin, pin_init, pin_low, pin_high, Pin_OUT_PP):
        """
        Write a single bit - requires cached methods/attributes be passed as arguments.
        See also write_bit()
        """
        delays = self.write_delays
        if value > 0:
            # write 1
            i = disable_irq()
            pin_low()
            pin_init(Pin_OUT_PP)
            udelay(delays[0])
            pin_high()
            enable_irq(i)
            udelay(delays[1])
        else:
            # write 0
            i = disable_irq()
            pin_low()
            pin_init(Pin_OUT_PP)
            udelay(delays[2])
            pin_high()
            enable_irq(i)
            udelay(delays[3])

    def write_byte(self, value):
        """
        Write a byte.  The pin will go tri-state at the end of the write to avoid
        heating in a short or other mishap.
        """
        #print ('write_byte: %x' % value)
        delays = self.write_delays
        pin, pin_init, pin_low, pin_high, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache
        if debug:
            counter = self.micros.counter
        bitmask = 0x01
        if debug:
            t1 = counter()
        while bitmask < 0x100:
            self._write_bit(value & bitmask, pin, pin_init, pin_low, pin_high, Pin_OUT_PP)
            bitmask <<= 1
        if debug:
            t2 = counter()
            print(t2 - t1)
        pin_init(Pin_IN, Pin_PULL_UP)
        pin_low()

    def write_bytes(self, bytestring):
        """
        Write a sequence of bytes.
        """
        for byte in bytestring:
            self.write_byte(byte)

    def _read_bit(self, pin_in, pin_value, pin_init, pin_low, Pin_PULL_UP, Pin_OUT_PP):
        """
        Read a single bit - requires cached methods/attributes be passed as arguments.
        See also read_bit()
        """
        one, two, three = self.read_delays
        i = disable_irq()
        pin_low()
        pin_init(Pin_OUT_PP)
        udelay(one)
        pin_init(pin_in, Pin_PULL_UP)
        udelay(two)
        value = pin_value()
        enable_irq(i)
        udelay(three)
        return value

    def read_bit(self):
        """
        Read a single bit.
        """
        pin, pin_init, pin_low, pin_high, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache
        return self._read_bit(Pin_IN, pin_value, pin_init, pin_low, Pin_PULL_UP, Pin_OUT_PP)

    def _read_byte(self, Pin_IN, pin_value, pin_init, pin_low, Pin_PULL_UP, Pin_OUT_PP):
        """
        Read a single byte - requires cached methods/attributes be passed as arguments.
        See also read_bytes()
        """
        byte = 0x00
        bitmask = 0x01
        while bitmask < 0x100:
            value = self._read_bit(Pin_IN, pin_value, pin_init, pin_low, Pin_PULL_UP, Pin_OUT_PP)
            if value:
                byte |= bitmask
            bitmask <<= 1
        return byte

    def read_bytes(self, count):
        """
        Read a sequence of N bytes.
        The bytes are returned as a bytearray.
        """
        i = 0
        s = bytearray()
        pin, pin_init, pin_low, pin_high, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache
        if debug:
            counter = self.micros.counter
            t1 = counter()
        _read_byte = self._read_byte
        while i < count:
            s.append(_read_byte(Pin_IN, pin_value, pin_init, pin_low, Pin_PULL_UP, Pin_OUT_PP))
            i += 1
        if debug:
            print (counter() - t1)
        return s

    def select(self, rom):
        """
        Select a specific device to talk to.  Pass in rom as a bytearray (8 bytes).
        """
        assert len(rom) == 8, "ROM must be 8 bytes"
        self.reset()
        self.write(0x55)  # ROM MATCH
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
        self.data.init(Pin.IN, Pin.PULL_NONE)

    def devices(self):
        """
        Return a list of ROMs for all attached devices. 
        Each ROM is returned as a bytearray of 8 bytes.
        """
        devices = []
        self.reset_search()
        while True:
            retval, rom = self.search()
            if not retval:
                break
            devices.append(rom)
        return devices

    def reset_search(self):
        self.LastDiscrepancy = 0
        self.LastDeviceFlag = False
        self.LastFamilyDiscrepancy = 0
        self.rom = bytearray(8) 
        
    def search(self):
        # initialize for search
        id_bit_number = 1
        last_zero = 0
        rom_byte_number = 0
        rom_byte_mask = 1
        search_result = 0
        pin, pin_init, pin_low, pin_high, pin_value, Pin_OUT_PP, Pin_IN, Pin_PULL_UP = self.cache

        # if the last call was not the last one
        if not self.LastDeviceFlag:
            # 1-Wire reset
            if not self.reset():
                self.reset_search()
                return False, None

            # issue the search command
            self.write_byte(0xF0)

            # loop to do the search
            while rom_byte_number < 8:  # loop until through all ROM bytes 0-7
                # read a bit and its complement
                id_bit = self._read_bit(Pin_IN, pin_value, pin_init, pin_low, Pin_PULL_UP, Pin_OUT_PP)
                cmp_id_bit = self._read_bit(Pin_IN, pin_value, pin_init, pin_low, Pin_PULL_UP, Pin_OUT_PP)

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
                       if (id_bit_number < self.LastDiscrepancy):
                          search_direction = (self.rom[rom_byte_number] & rom_byte_mask) > 0
                       else:
                          # if equal to last pick 1, if not then pick 0
                          search_direction = (id_bit_number == self.LastDiscrepancy)
    
                       # if 0 was picked then record its position in LastZero
                       if search_direction == 0:
                           last_zero = id_bit_number
    
                           # check for Last discrepancy in family
                           if last_zero < 9:
                               self.LastFamilyDiscrepancy = last_zero

                    # set or clear the bit in the ROM byte rom_byte_number
                    # with mask rom_byte_mask
                    if search_direction == 1:
                        self.rom[rom_byte_number] |= rom_byte_mask
                    else:
                        self.rom[rom_byte_number] &= ~rom_byte_mask

                # serial number search direction write bit
                self.write_bit(search_direction)

                # increment the byte counter id_bit_number
                # and shift the mask rom_byte_mask
                id_bit_number += 1
                rom_byte_mask <<= 1

                # if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
                if rom_byte_mask == 0:
                    rom_byte_number += 1
                    rom_byte_mask = 1

        # if the search was successful then
        if not (id_bit_number < 65):
            # search successful so set LastDiscrepancy,LastDeviceFlag,search_result
            self.LastDiscrepancy = last_zero

            # check for last device
            if self.LastDiscrepancy == 0:
                self.LastDeviceFlag = True
            search_result = True

        # if no device found then reset counters so next 'search' will be like a first
        if not search_result or not self.rom[0]:
            self.LastDiscrepancy = 0
            self.LastDeviceFlag = False
            self.LastFamilyDiscrepancy = 0
            search_result = False

        retval = bytearray(8)
        i = 0
        while i < 8:
            retval[i] = self.rom[i]
            i += 1
        return search_result, retval

            

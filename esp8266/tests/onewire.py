import time
import pyb
import _onewire as _ow

class OneWire:
    CMD_SEARCHROM = const(0xf0)
    CMD_READROM = const(0x33)
    CMD_MATCHROM = const(0x55)
    CMD_SKIPROM = const(0xcc)

    def __init__(self, pin):
        self.pin = pin
        self.pin.init(pin.OPEN_DRAIN, pin.PULL_NONE)

    def reset(self):
        return _ow.reset(self.pin)

    def read_bit(self):
        return _ow.readbit(self.pin)

    def read_byte(self):
        return _ow.readbyte(self.pin)

    def read_bytes(self, count):
        buf = bytearray(count)
        for i in range(count):
            buf[i] = _ow.readbyte(self.pin)
        return buf

    def write_bit(self, value):
        return _ow.writebit(self.pin, value)

    def write_byte(self, value):
        return _ow.writebyte(self.pin, value)

    def write_bytes(self, buf):
        for b in buf:
            _ow.writebyte(self.pin, b)

    def select_rom(self, rom):
        self.reset()
        self.write_byte(CMD_MATCHROM)
        self.write_bytes(rom)

    def scan(self):
        devices = []
        diff = 65
        rom = False
        for i in range(0xff):
            rom, diff = self._search_rom(rom, diff)
            if rom:
                devices += [rom]
            if diff == 0:
                break
        return devices

    def _search_rom(self, l_rom, diff):
        if not self.reset():
            return None, 0
        self.write_byte(CMD_SEARCHROM)
        if not l_rom:
            l_rom = bytearray(8)
        rom = bytearray(8)
        next_diff = 0
        i = 64
        for byte in range(8):
            r_b = 0
            for bit in range(8):
                b = self.read_bit()
                if self.read_bit():
                    if b: # there are no devices or there is an error on the bus
                        return None, 0
                else:
                    if not b: # collision, two devices with different bit meaning
                        if diff > i or ((l_rom[byte] & (1 << bit)) and diff != i):
                            b = 1
                            next_diff = i
                self.write_bit(b)
                if b:
                    r_b |= 1 << bit
                i -= 1
            rom[byte] = r_b
        return rom, next_diff

    def crc8(self, data):
        return _ow.crc8(data)

class DS18B20:
    THERM_CMD_CONVERTTEMP = const(0x44)
    THERM_CMD_RSCRATCHPAD = const(0xbe)

    def __init__(self, onewire):
        self.ow = onewire
        self.roms = []

    def scan(self):
        self.roms = []
        for rom in self.ow.scan():
            if rom[0] == 0x28:
                self.roms += [rom]
        return self.roms

    def start_measure(self):
        if not self.ow.reset():
            return False
        self.ow.write_byte(CMD_SKIPROM)
        self.ow.write_byte(THERM_CMD_CONVERTTEMP)
        return True

    def get_temp(self, rom):
        if not self.ow.reset():
            return None

        self.ow.select_rom(rom)
        self.ow.write_byte(THERM_CMD_RSCRATCHPAD)

        buf = self.ow.read_bytes(9)
        if self.ow.crc8(buf):
            return None

        return self._convert_temp(buf)

    def _convert_temp(self, data):
        temp_lsb = data[0]
        temp_msb = data[1]
        return (temp_msb << 8 | temp_lsb) / 16

# connect 1-wire temp sensors to GPIO12 for this test
def test():
    dat = pyb.Pin(12)
    ow = OneWire(dat)

    ds = DS18B20(ow)
    roms = ow.scan()
    print('found devices:', roms)

    for i in range(4):
        print('temperatures:', end=' ')
        ds.start_measure()
        time.sleep_ms(750)
        for rom in roms:
            print(ds.get_temp(rom), end=' ')
        print()

#pyb.freq(80000000)
#pyb.freq(160000000)
test()

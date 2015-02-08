import pyb
from pyb import Pin
from pyb import Timer
from pyb import udelay
from pyb import disable_irq
from pyb import enable_irq

# Thanks to inspiration/examples from:
#   https://www.pjrc.com/teensy/td_libs_OneWire.html
#   https://github.com/kurik/uPython-DHT22

# Does not support devices running in parasite power mode. 

class OneWire:
    
    def __init__(self, pin):
        # TODO: need timer ID?
        self.data = Pin(pin, Pin.IN, Pin.PULL_UP)
        self.micros = Timer(2, prescaler=83, period=0x3fffffff) # 1MHz ~ 1uS
        self.write_delays = (1, 60, 60, 1)
        self.read_delays = (1, 1, 40)

    def reset(self):
        """
        Perform the onewire reset function.
        Returns 1 if a device asserted a presence pulse, 0 otherwise.
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


    def write_bit(self, value, pin, pin_init, pin_out, pin_low, pin_high, counter, delay_until=None):
        """
        Write a bit.
        """
        #print ('write bit: %d' % value)
        if value > 0:
            # write 1
            i = disable_irq()
            pin_low()
            pin_init(pin_out)
            udelay(5)
            pin_high()
            enable_irq(i)
            udelay(55)
        else:
            # write 0
            i = disable_irq()
            pin_low()
            pin_init(pin_out)
            udelay(60)
            pin_high()
            enable_irq(i)
            #udelay(5)

    def write_byte(self, value):
        """
        Write a byte.  The pin will go tri-state at the end of the write to avoid
        heating in a short or other mishap.
        """
        #print ('write_byte: %x' % value)
        delays = self.write_delays
        pin = self.data
        pin_init = self.data.init
        pin_out = Pin.OUT_PP
        pin_low = self.data.low
        pin_high = self.data.high
        counter = self.micros.counter
        bitmask = 0x01
        t1 = counter()
        write_bit = self.write_bit
        while bitmask < 0x100:
            if value & bitmask > 0:
                # write 1
                i = disable_irq()
                pin_low()
                pin_init(pin_out)
                udelay(delays[0])
                pin_high()
                enable_irq(i)
                udelay(delays[1])
            else:
                # write 0
                i = disable_irq()
                pin_low()
                pin_init(pin_out)
                udelay(delays[2])
                pin_high()
                enable_irq(i)
                udelay(delays[3])
            bitmask <<= 1
        t2 = counter()
        pin_init(Pin.IN, Pin.PULL_UP)
        pin_low()
        print(t2 - t1)

    def write_bytes(self, bytestring):
        """
        Write a sequence of bytes.
        """
        for byte in bytestring:
            self.write_byte(byte)
        #i = pyb.disable_irq()
        #self.data.init(Pin.IN, Pin.PULL_NONE)
        #pyb.enable_irq(i)

    def read_byte(self, pin_in, pin_value, pin_init, pin_low, pull_up, pin_out):
        one, two, three = self.read_delays
        byte = 0x00
        bitmask = 0x01
        while bitmask < 0x100:
            i = disable_irq()
            pin_low()
            pin_init(pin_out)
            udelay(one)
            pin_init(pin_in, pull_up)
            udelay(two)
            value = pin_value()
            enable_irq(i)
            udelay(three)
            if value:
                byte |= bitmask
            bitmask <<= 1
        return byte

    def read_bytes(self, count):
        i = 0
        s = bytearray()

        pin = self.data
        pin_in = Pin.IN
        pull_up = Pin.PULL_UP
        counter = self.micros.counter
        pin_value = pin.value
        pin_init = pin.init
        pin_low = pin.low
        pin_out = Pin.OUT_PP
        counter = self.micros.counter
        t1 = counter()
        read_byte = self.read_byte
        while i < count:
            s.append(read_byte(pin_in, pin_value, pin_init, pin_low, pull_up, pin_out))
            i += 1
        print (counter() - t1)
        return s

    def select(self, rom):
        self.write(0x55)  # ROM MATCH
        i = 0
        while i < 8:
            self.write(rom[i])
            i += 1

    def read_rom(self):
        self.reset()
        self.write_byte(0x33)   # READ ROM
        rom = self.read_bytes(8)
        # TODO: check CRC of the ROM
        return rom

    def skip_rom(self):
        self.write_byte(0xCC)   # SKIP ROM
        

            
'''

//
// Do a ROM skip
//
void OneWire::skip()
{
    write(0xCC);           // Skip ROM
}

void OneWire::depower()
{
	noInterrupts();
	DIRECT_MODE_INPUT(baseReg, bitmask);
	interrupts();
}

#if ONEWIRE_SEARCH

//
// You need to use this function to start a search again from the beginning.
// You do not need to do it for the first search, though you could.
//
void OneWire::reset_search()
{
  // reset the search state
  LastDiscrepancy = 0;
  LastDeviceFlag = FALSE;
  LastFamilyDiscrepancy = 0;
  for(int i = 7; ; i--) {
    ROM_NO[i] = 0;
    if ( i == 0) break;
  }
}

// Setup the search to find the device type 'family_code' on the next call
// to search(*newAddr) if it is present.
//
void OneWire::target_search(uint8_t family_code)
{
   // set the search state to find SearchFamily type devices
   ROM_NO[0] = family_code;
   for (uint8_t i = 1; i < 8; i++)
      ROM_NO[i] = 0;
   LastDiscrepancy = 64;
   LastFamilyDiscrepancy = 0;
   LastDeviceFlag = FALSE;
}

//
// Perform a search. If this function returns a '1' then it has
// enumerated the next device and you may retrieve the ROM from the
// OneWire::address variable. If there are no devices, no further
// devices, or something horrible happens in the middle of the
// enumeration then a 0 is returned.  If a new device is found then
// its address is copied to newAddr.  Use OneWire::reset_search() to
// start over.
//
// --- Replaced by the one from the Dallas Semiconductor web site ---
//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
uint8_t OneWire::search(uint8_t *newAddr)
{
   uint8_t id_bit_number;
   uint8_t last_zero, rom_byte_number, search_result;
   uint8_t id_bit, cmp_id_bit;

   unsigned char rom_byte_mask, search_direction;

   // initialize for search
   id_bit_number = 1;
   last_zero = 0;
   rom_byte_number = 0;
   rom_byte_mask = 1;
   search_result = 0;

   // if the last call was not the last one
   if (!LastDeviceFlag)
   {
      // 1-Wire reset
      if (!reset())
      {
         // reset the search
         LastDiscrepancy = 0;
         LastDeviceFlag = FALSE;
         LastFamilyDiscrepancy = 0;
         return FALSE;
      }

      // issue the search command
      write(0xF0);

      // loop to do the search
      do
      {
         // read a bit and its complement
         id_bit = read_bit();
         cmp_id_bit = read_bit();

         // check for no devices on 1-wire
         if ((id_bit == 1) && (cmp_id_bit == 1))
            break;
         else
         {
            // all devices coupled have 0 or 1
            if (id_bit != cmp_id_bit)
               search_direction = id_bit;  // bit write value for search
            else
            {
               // if this discrepancy if before the Last Discrepancy
               // on a previous next then pick the same as last time
               if (id_bit_number < LastDiscrepancy)
                  search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (id_bit_number == LastDiscrepancy);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = id_bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     LastFamilyDiscrepancy = last_zero;
               }
            }

            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (search_direction == 1)
              ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
              ROM_NO[rom_byte_number] &= ~rom_byte_mask;

            // serial number search direction write bit
            write_bit(search_direction);

            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            id_bit_number++;
            rom_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
            if (rom_byte_mask == 0)
            {
                rom_byte_number++;
                rom_byte_mask = 1;
            }
         }
      }
      while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7

      // if the search was successful then
      if (!(id_bit_number < 65))
      {
         // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
         LastDiscrepancy = last_zero;

         // check for last device
         if (LastDiscrepancy == 0)
            LastDeviceFlag = TRUE;

         search_result = TRUE;
      }
   }

   // if no device found then reset counters so next 'search' will be like a first
   if (!search_result || !ROM_NO[0])
   {
      LastDiscrepancy = 0;
      LastDeviceFlag = FALSE;
      LastFamilyDiscrepancy = 0;
      search_result = FALSE;
   }
   for (int i = 0; i < 8; i++) newAddr[i] = ROM_NO[i];
   return search_result;
  }
'''

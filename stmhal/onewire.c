/*
OneWire library ported to MicroPython by Jason Hildebrand.

TODO: 
  * implement and test parasite-power mode (as an init option)
  * port the crc checks and expose python methods to access them
  * refactor code and move to ../drivers
  * add switches to make it conditionally compile (mpconfigport.h?)
  * document the python object


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

The CRC code was excerpted and inspired by the Dallas Semiconductor
sample code bearing this copyright.
//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//--------------------------------------------------------------------------
*/

#include "onewire.h"
#include <stdbool.h>
#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/nlr.h"
#include "pin.h"
#include "mphal.h"
#include "std.h"

#define FALSE 0
#define TRUE  1

typedef struct _onewire_obj_t {
    mp_obj_base_t   base;
    const pin_obj_t *pin;
    GPIO_InitTypeDef initpin;
    unsigned char ROM_NO[8];
    uint8_t LastDiscrepancy;
    uint8_t LastFamilyDiscrepancy;
    uint8_t LastDeviceFlag;
} onewire_obj_t;

#define ONEWIRE_INPUT		\
    self->initpin.Mode = GPIO_MODE_INPUT; \
    self->initpin.Pull = GPIO_PULLUP; \
    HAL_GPIO_Init(self->pin->gpio, &(self->initpin))

#define ONEWIRE_OUTPUT      \
    self->initpin.Mode = GPIO_MODE_OUTPUT_PP; \
    HAL_GPIO_Init(self->pin->gpio, &(self->initpin))
    
#define ONEWIRE_LOW  HAL_GPIO_WritePin(self->pin->gpio, self->pin->pin_mask, 0);

#define ONEWIRE_HIGH HAL_GPIO_WritePin(self->pin->gpio, self->pin->pin_mask, 1);

#define ONEWIRE_READPIN() HAL_GPIO_ReadPin(self->pin->gpio, self->pin->pin_mask);
    
// Returns True if a device asserted a presence pulse, False otherwise.
// First, we wait up to 250uS for the bus to come high, 
// if it doesn't then it is broken or shorted and we return a 0;
STATIC mp_obj_t onewire_reset(mp_obj_t self_in)
{
    int irq_orig;
	uint8_t retries = 125;
    uint8_t r;
    onewire_obj_t *self = self_in;

    // Mode Input
    irq_orig = disable_irq();
    ONEWIRE_INPUT;
    enable_irq(irq_orig);

    do { 
        if (--retries == 0) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Trying to reset, but pin %s didn't come high.  This indicates a possible short - check your wiring.", qstr_str(self->pin->name)));
        }
        sys_tick_udelay(2);
    } while ( !HAL_GPIO_ReadPin(self->pin->gpio, self->pin->pin_mask));

    irq_orig = disable_irq();
    ONEWIRE_LOW;
    ONEWIRE_OUTPUT;
    enable_irq(irq_orig);

	sys_tick_udelay(480);
	irq_orig = disable_irq();
    ONEWIRE_INPUT;
	sys_tick_udelay(70);
	r = !ONEWIRE_READPIN();
	enable_irq(irq_orig);
	sys_tick_udelay(410);
	return MP_BOOL(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_reset_obj, onewire_reset);


// Write a bit. Port and bit is used to cut lookup time and provide
// more certain timing.
//
void onewire_write_bit(onewire_obj_t *self, uint8_t v)
{
    int irq_orig;

	if (v & 1) {
        irq_orig = disable_irq();
        ONEWIRE_LOW;
        ONEWIRE_OUTPUT;
		sys_tick_udelay(10);
        ONEWIRE_HIGH;
        enable_irq(irq_orig);
		sys_tick_udelay(55);
	} else {
        irq_orig = disable_irq();
        ONEWIRE_LOW;
        ONEWIRE_OUTPUT;
		sys_tick_udelay(65);
        ONEWIRE_HIGH;
        enable_irq(irq_orig);
		sys_tick_udelay(5);
	}
}

// Read one bit.
//
uint8_t onewire_read_bit(onewire_obj_t *self)
{
    int irq_orig;
	uint8_t r;

    irq_orig = disable_irq();
    ONEWIRE_LOW;
    ONEWIRE_OUTPUT;
	sys_tick_udelay(2);
    ONEWIRE_INPUT;
	sys_tick_udelay(13);
    r = ONEWIRE_READPIN();
    enable_irq(irq_orig);
	sys_tick_udelay(40);
	return r;
}



//
// Write a byte. 
// If you need power after the write (e.g. DS18S20 in parasite power mode)
// then set 'power' to 1, otherwise the pin will go tri-state at the end 
// of the write to avoid heating in a short or other mishap.
void onewire_write_byte(onewire_obj_t *self, uint8_t v, uint8_t power) {
    uint8_t bitmask;

    for (bitmask = 0x01; bitmask; bitmask <<= 1) {
        onewire_write_bit(self, (bitmask & v)?1:0);
    }
    if ( !power) {
        int irq_orig;

        irq_orig = disable_irq();
        ONEWIRE_INPUT;
        ONEWIRE_LOW;  
        enable_irq(irq_orig);
    }
}

void onewire_write_bytes(onewire_obj_t *self, const uint8_t *buf, uint16_t count, bool power) {
    for (uint16_t i = 0 ; i < count ; i++) {
        onewire_write_byte(self, buf[i], power);
    }
    if (!power) {
        int irq_orig;

        irq_orig = disable_irq();
        ONEWIRE_INPUT;
        ONEWIRE_LOW;
        enable_irq(irq_orig);
    }
}

//
// Read a byte
//
uint8_t onewire_read_byte(onewire_obj_t *self) {
    uint8_t bitmask;
    uint8_t r = 0;

    for (bitmask = 0x01; bitmask; bitmask <<= 1) {
        if ( onewire_read_bit(self)) {
            r |= bitmask;
        }
    }
    return r;
}

void onewire_read_bytes(onewire_obj_t *self, uint8_t *buf, uint16_t count) {
    for (uint16_t i = 0 ; i < count ; i++) {
        buf[i] = onewire_read_byte(self);
    }
}

mp_obj_t onewire_write_bytearray(mp_obj_t self_in, mp_obj_t bytearray) {
    onewire_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    uint8_t byte;
    uint8_t i;

    mp_get_buffer(bytearray, &bufinfo, 0);

    if (!MP_OBJ_IS_TYPE(bytearray, &mp_type_bytearray)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "write() requires a bytearray."));
    }
    for (i = 0; i < bufinfo.len; i++) {
        byte = ((uint8_t *)(bufinfo.buf))[i];
        onewire_write_byte(self, byte, 0);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(onewire_write_bytearray_obj, onewire_write_bytearray);

mp_obj_t onewire_read_single_bit(mp_obj_t self_in) {
    onewire_obj_t *self = self_in;
    return mp_obj_new_int(onewire_read_bit(self)); 
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_read_single_bit_obj, onewire_read_single_bit);

mp_obj_t onewire_read_n_bytes(mp_obj_t self_in, mp_obj_t num_bytes) {
    onewire_obj_t *self = self_in;
    uint8_t buf[255];
    uint32_t n;

    if (!MP_OBJ_IS_INT(num_bytes)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "read() requires an integer argument."));
    }
    n = mp_obj_get_int(num_bytes);
    if (n >= 255) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "read() cannot read more than 255 bytes per call."));
    }
    onewire_read_bytes(self, buf, n);
    return mp_obj_new_bytearray(n, buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(onewire_read_n_bytes_obj, onewire_read_n_bytes);

mp_obj_t onewire_select(mp_obj_t self_in, mp_obj_t bytearray) {
    onewire_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    uint8_t byte;
    uint8_t i;

    onewire_reset(self);
    onewire_write_byte(self, 0x55, 0);
    mp_get_buffer(bytearray, &bufinfo, 0);

    if (!MP_OBJ_IS_TYPE(bytearray, &mp_type_bytearray)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "select() requires a bytearray containing the ROM to select."));
    }
    if (bufinfo.len != 8 || bufinfo.buf == NULL) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Bytearray must be exactly 8 bytes."));
    }
    for (i = 0; i < 8; i++) {
        byte = ((uint8_t *)(bufinfo.buf))[i];
        onewire_write_byte(self, byte, 0);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(onewire_select_obj, onewire_select);


mp_obj_t onewire_read_rom(mp_obj_t self_in) {
    onewire_obj_t *self = self_in;
    uint8_t buf[10];

    onewire_reset(self);
    onewire_write_byte(self, 0x33, 0);  // READ ROM
    onewire_read_bytes(self, buf, 8);
    return mp_obj_new_bytearray(8, buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_read_rom_obj, onewire_read_rom);

mp_obj_t onewire_skip(mp_obj_t self_in) {
    onewire_obj_t *self = self_in;

    onewire_reset(self);
    onewire_write_byte(self, 0xCC, 0);  // Skip ROM
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_skip_obj, onewire_skip);

mp_obj_t onewire_depower(mp_obj_t self_in) {
    onewire_obj_t *self = self_in;

    self->initpin.Mode = GPIO_MODE_INPUT;
    self->initpin.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(self->pin->gpio, &(self->initpin));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_depower_obj, onewire_depower);



//
// You need to use this function to start a search again from the beginning.
// You do not need to do it for the first search, though you could.
//
void onewire_reset_search(onewire_obj_t *self)
{
    // reset the search state
    self->LastDiscrepancy = 0;
    self->LastDeviceFlag = FALSE;
    self->LastFamilyDiscrepancy = 0;
    for(int i = 7; ; i--) {
        self->ROM_NO[i] = 0;
        if ( i == 0) {
            break;
        }
    }
}


//
// Perform a search. If this function returns a '1' then it has
// enumerated the next device and you may retrieve the ROM from the
// OneWire_address variable. If there are no devices, no further
// devices, or something horrible happens in the middle of the
// enumeration then a 0 is returned.  If a new device is found then
// its address is copied to newAddr.  Use onewire_reset_search() to
// start over.
//
// --- Replaced by the one from the Dallas Semiconductor web site ---
//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
uint8_t onewire_search(onewire_obj_t *self, uint8_t *newAddr)
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
   if (!self->LastDeviceFlag)
   {
      // 1-Wire reset
      if (!onewire_reset(self))
      {
         // reset the search
         self->LastDiscrepancy = 0;
         self->LastDeviceFlag = FALSE;
         self->LastFamilyDiscrepancy = 0;
         return FALSE;
      }

      // issue the search command
      onewire_write_byte(self, 0xF0, 0);

      // loop to do the search
      do
      {
         // read a bit and its complement
         id_bit = onewire_read_bit(self);
         cmp_id_bit = onewire_read_bit(self);

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
               if (id_bit_number < self->LastDiscrepancy)
                  search_direction = ((self->ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (id_bit_number == self->LastDiscrepancy);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = id_bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     self->LastFamilyDiscrepancy = last_zero;
               }
            }

            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (search_direction == 1)
              self->ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
              self->ROM_NO[rom_byte_number] &= ~rom_byte_mask;

            // serial number search direction write bit
            onewire_write_bit(self, search_direction);

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
         self->LastDiscrepancy = last_zero;

         // check for last device
         if (self->LastDiscrepancy == 0)
            self->LastDeviceFlag = TRUE;

         search_result = TRUE;
      }
   }

   // if no device found then reset counters so next 'search' will be like a first
   if (!search_result || !self->ROM_NO[0])
   {
      self->LastDiscrepancy = 0;
      self->LastDeviceFlag = FALSE;
      self->LastFamilyDiscrepancy = 0;
      search_result = FALSE;
   }
   for (int i = 0; i < 8; i++) {
      newAddr[i] = self->ROM_NO[i];
   }
   return search_result;
}

mp_obj_t onewire_devices(mp_obj_t self_in) {
    onewire_obj_t *self = self_in;
    uint8_t buf[8];
    mp_obj_t list; 

    list = mp_obj_new_list(0, NULL);
    onewire_reset_search(self);
    while(TRUE) {
        if (onewire_search(self, buf)) {
            mp_obj_list_append(list, mp_obj_new_bytearray(8, buf));
        } else {
            break;
        }
    }
    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_devices_obj, onewire_devices);


#if NOT_TRUE

#if ONEWIRE_CRC
// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
//

#if ONEWIRE_CRC8_TABLE
// This table comes from Dallas sample code where it is freely reusable,
// though Copyright (C) 2000 Dallas Semiconductor Corporation
static const uint8_t PROGMEM dscrc_table[] = {
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

//
// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (note: this might better be done without to
// table, it would probably be smaller and certainly fast enough
// compared to all those delayMicrosecond() calls.  But I got
// confused, so I use this table from the examples.)
//
uint8_t OneWire_crc8(const uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;

	while (len--) {
		crc = pgm_read_byte(dscrc_table + (crc ^ *addr++));
	}
	return crc;
}
#else
//
// Compute a Dallas Semiconductor 8 bit CRC directly.
// this is much slower, but much smaller, than the lookup table.
//
uint8_t OneWire_crc8(const uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;
	
	while (len--) {
		uint8_t inbyte = *addr++;
		for (uint8_t i = 8; i; i--) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}
#endif

#if ONEWIRE_CRC16
bool OneWire_check_crc16(const uint8_t* input, uint16_t len, const uint8_t* inverted_crc, uint16_t crc)
{
    crc = ~crc16(input, len, crc);
    return (crc & 0xFF) == inverted_crc[0] && (crc >> 8) == inverted_crc[1];
}

uint16_t OneWire_crc16(const uint8_t* input, uint16_t len, uint16_t crc)
{
    static const uint8_t oddparity[16] =
        { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

    for (uint16_t i = 0 ; i < len ; i++) {
      // Even though we're just copying a byte from the input,
      // we'll be doing 16-bit computation with it.
      uint16_t cdata = input[i];
      cdata = (cdata ^ crc) & 0xff;
      crc >>= 8;

      if (oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4])
          crc ^= 0xC001;

      cdata <<= 6;
      crc ^= cdata;
      cdata <<= 1;
      crc ^= cdata;
    }
    return crc;
}
#endif

#endif

#endif

STATIC mp_obj_t onewire_test(mp_obj_t pin_obj) 
{
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef slave;
    GPIO_InitTypeDef master;

    // PC12
    slave.Pin = GPIO_PIN_12;
    slave.Mode = GPIO_MODE_INPUT;
    slave.Pull = GPIO_PULLUP;
    slave.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOC, &slave);   // set as input
    sys_tick_udelay(1);

    printf("PC12 = %d\n", HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12));   // expect 1

    // PD0
    master.Pin = GPIO_PIN_0;
    master.Mode = GPIO_MODE_OUTPUT_PP;
    master.Pull = GPIO_NOPULL;
    master.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, 1);
    printf("GPIO_PIN_0: %d\n", GPIO_PIN_0);
    HAL_GPIO_Init(GPIOD, &master);   // set as output

    printf("PC12 = %d\n", HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12));   // expect 1

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, 0);
    printf("PC12 = %d\n", HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12));   // expect 0

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, 1);
    printf("PC12 = %d\n", HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12));   // expect 1
    printf("\n");

    master.Mode = GPIO_MODE_INPUT;
    master.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &master);   // set as input

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 1);
    slave.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOC, &slave);   // set as output

    printf("PD0 = %d\n", HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0));   // expect 1
    return mp_obj_new_int(0); 


/*
    const pin_obj_t *pin = NULL;
    uint8_t buf[10];
    //uint32_t t1, t2;

    pin = pin_find(pin_obj);
    __GPIOD_CLK_ENABLE();
    onewire_reset_internal(pin);
    onewire_write_byte(pin, 0x33, 0);  // READ ROM
    //t1 = sys_tick_get_microseconds();
    //onewire_read_bit(pin);
    onewire_read_bytes(pin, buf, 8);
    //t2 = sys_tick_get_microseconds();
    return mp_obj_new_bytearray(8, buf);
    //return mp_obj_new_int(t2 - t1); 
    */
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(onewire_test_obj, onewire_test);



/// \classmethod \constructor(pin)
/// Create a OneWire object associated with the given pin:
///
///   - `pin` is a Pin Object or a pin name (as a string)
STATIC mp_obj_t onewire_obj_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    const pin_obj_t *pin = NULL;
    int irq_orig;

    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    
    // look up pin and create OneWire object
    pin = pin_find(args[0]);
    onewire_obj_t *self = m_new_obj(onewire_obj_t);
    self->base.type = type_in;
    self->pin = pin;
    self->initpin.Pin = pin->pin_mask;
    self->initpin.Mode = GPIO_MODE_INPUT;
    self->initpin.Pull = GPIO_PULLUP;
    self->initpin.Speed = GPIO_SPEED_FAST;
    
    // initialize the pin
    irq_orig = disable_irq();
    HAL_GPIO_Init(pin->gpio, &(self->initpin));
    enable_irq(irq_orig);

    return self;
}

void onewire_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    onewire_obj_t *self = self_in;
    print(env, "<OneWire pin=Pin.cpu.%s>", qstr_str(self->pin->name));
}

STATIC const mp_map_elem_t onewire_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset), (mp_obj_t)&onewire_reset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_select), (mp_obj_t)&onewire_select_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_rom), (mp_obj_t)&onewire_read_rom_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_skip), (mp_obj_t)&onewire_skip_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_depower), (mp_obj_t)&onewire_depower_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_devices), (mp_obj_t)&onewire_devices_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&onewire_write_bytearray_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&onewire_read_n_bytes_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_bit), (mp_obj_t)&onewire_read_single_bit_obj },
};

STATIC MP_DEFINE_CONST_DICT(onewire_locals_dict, onewire_locals_dict_table);

const mp_obj_type_t onewire_type = {
    { &mp_type_type },
    .name = MP_QSTR_OneWire,
    .print = onewire_obj_print,
    .make_new = onewire_obj_make_new,
    .locals_dict = (mp_obj_t)&onewire_locals_dict,
};

STATIC const mp_map_elem_t mp_module_onewire_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_onewire) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OneWire), (mp_obj_t)&onewire_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_test), (mp_obj_t)&onewire_test_obj},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_onewire_globals, mp_module_onewire_globals_table);

const mp_obj_module_t mp_module_onewire = {
    .base = { &mp_type_module },
    .name = MP_QSTR_onewire,
    .globals = (mp_obj_dict_t*)&mp_module_onewire_globals,
};



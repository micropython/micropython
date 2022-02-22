#
# Driver class for the WM8960 Codec to be used e.g. with MIMXRT_1xxx Boards.
# Derived from the NXP SDK drivers.
#
# Copyright (c) 2015, Freescale Semiconductor, Inc., (C-Code)
# Copyright 2016-2021 NXP, (C-Code)
# All rights reserved.
#
# Translated to MicroPython by Robert Hammelrath, 2022
#
# SPDX-License-Identifier: BSD-3-Clause
#

import array

# Define the register addresses of WM8960.
_LINVOL = const(0x0)
_RINVOL = const(0x1)
_LOUT1 = const(0x2)
_ROUT1 = const(0x3)
_CLOCK1 = const(0x4)
_DACCTL1 = const(0x5)
_DACCTL2 = const(0x6)
_IFACE1 = const(0x7)
_CLOCK2 = const(0x8)
_IFACE2 = const(0x9)
_LDAC = const(0xA)
_RDAC = const(0xB)
_RESET = const(0xF)
_3D = const(0x10)
_ALC1 = const(0x11)
_ALC2 = const(0x12)
_ALC3 = const(0x13)
_NOISEG = const(0x14)
_LADC = const(0x15)
_RADC = const(0x16)
_ADDCTL1 = const(0x17)
# Register _ADDCTL2 = const(0x18)
_POWER1 = const(0x19)
_POWER2 = const(0x1A)
_ADDCTL3 = const(0x1B)
# Register _APOP1 = const(0x1C)
# Register _APOP2 = const(0x1D)
_LINPATH = const(0x20)
_RINPATH = const(0x21)
_LOUTMIX = const(0x22)
_ROUTMIX = const(0x25)
_MONOMIX1 = const(0x26)
_MONOMIX2 = const(0x27)
_LOUT2 = const(0x28)
_ROUT2 = const(0x29)
_MONO = const(0x2A)
_INBMIX1 = const(0x2B)
_INBMIX2 = const(0x2C)
_BYPASS1 = const(0x2D)
_BYPASS2 = const(0x2E)
_POWER3 = const(0x2F)
_ADDCTL4 = const(0x30)
_CLASSD1 = const(0x31)
# Register _CLASSD3 = const(0x33)
_PLL1 = const(0x34)
_PLL2 = const(0x35)
_PLL3 = const(0x36)
_PLL4 = const(0x37)

# WM8960 PLLN range */
_PLL_N_MIN_VALUE = const(6)
_PLL_N_MAX_VALUE = const(12)

# WM8960 CLOCK2 bits
_CLOCK2_BCLK_DIV_MASK = const(0x0F)
_CLOCK2_DCLK_DIV_MASK = const(0x1C0)
_CLOCK2_DCLK_DIV_SHIFT = const(0x06)

# Register _IFACE1
_IFACE1_FORMAT_MASK = const(0x03)
_IFACE1_WL_MASK = const(0x0C)
_IFACE1_WL_SHIFT = const(0x02)
_IFACE1_LRP_MASK = const(0x10)
_IFACE1_MS_MASK = const(0x40)
_IFACE1_DLRSWAP_MASK = const(0x20)
_IFACE1_ALRSWAP_MASK = const(0x100)

# Register _POWER1
_POWER1_VREF_MASK = const(0x40)
_POWER1_VREF_SHIFT = const(0x06)
_POWER1_AINL_MASK = const(0x20)
_POWER1_AINR_MASK = const(0x10)
_POWER1_ADCL_MASK = const(0x08)
_POWER1_ADCR_MASK = const(0x0)
_POWER1_MICB_MASK = const(0x02)
_POWER1_MICB_SHIFT = const(0x01)

# Register _POWER2
_POWER2_DACL_MASK = const(0x100)
_POWER2_DACR_MASK = const(0x80)
_POWER2_LOUT1_MASK = const(0x40)
_POWER2_ROUT1_MASK = const(0x20)
_POWER2_SPKL_MASK = const(0x10)
_POWER2_SPKR_MASK = const(0x08)
_POWER3_LMIC_MASK = const(0x20)
_POWER3_RMIC_MASK = const(0x10)
_POWER3_LOMIX_MASK = const(0x08)
_POWER3_ROMIX_MASK = const(0x04)

# Register _DACCTL1 .. 3
_DACCTL1_MONOMIX_MASK = const(0x10)
_DACCTL1_MONOMIX_SHIFT = const(0x4)
_DACCTL1_DACMU_MASK = const(0x08)
_DACCTL1_DEEM_MASK = const(0x06)
_DACCTL1_DEEM_SHIFT = const(0x01)
_DACCTL2_DACSMM_MASK = const(0x08)
_DACCTL2_DACMR_MASK = const(0x04)
_DACCTL3_ALCSR_MASK = const(0x07)

# _WM8060_ALC1 .. 3
_ALC_CHANNEL_MASK = const(0x180)
_ALC_CHANNEL_SHIFT = const(0x7)
_ALC_MODE_MASK = const(0x100)
_ALC_MODE_SHIFT = const(0x8)
_ALC_GAIN_MASK = const(0x70)
_ALC_GAIN_SHIFT = const(0x4)
_ALC_TARGET_MASK = const(0x0F)
_ALC_ATTACK_MASK = const(0x0F)
_ALC_DECAY_MASK = const(0xF0)
_ALC_DECAY_SHIFT = const(4)
_ALC_HOLD_MASK = const(0xF)

# Register _NOISEG
_NOISEG_LEVEL_SHIFT = const(3)

_I2C_ADDR = const(0x1A)

# WM8960 maximum volume values
_MAX_VOLUME_ADC = const(0xFF)
_MAX_VOLUME_DAC = const(0xFF)
_MAX_VOLUME_HEADPHONE = const(0x7F)
_MAX_VOLUME_LINEIN = const(0x3F)
_MAX_VOLUME_SPEAKER = const(0x7F)

# Config symbol names
# Modules
module_ADC = const(0)  # ADC module in WM8960
module_DAC = const(1)  # DAC module in WM8960
module_VREF = const(2)  # VREF module
module_headphone = const(3)  # Headphone
module_mic_bias = const(4)  # Mic bias
module_mic = const(5)  # Input Mic
module_line_in = const(6)  # Analog in PGA
module_line_out = const(7)  # Line out module
module_speaker = const(8)  # Speaker module
module_omix = const(9)  # Output mixer
module_mono_out = const(10)  # Mono mix

# Route
route_bypass = const(0)  # LINEIN->Headphone.
route_playback = const(1)  #  I2SIN->DAC->Headphone.
route_playback_record = const(2)  # I2SIN->DAC->Headphone, LINEIN->ADC->I2SOUT.
route_record = const(5)  # LINEIN->ADC->I2SOUT.

# Input
input_closed = const(0)  # Input device is closed
input_mic1 = const(1)  # Input as single ended mic, only use L/RINPUT1
input_mic2 = const(2)  # Input as diff. mic, use L/RINPUT1 and L/RINPUT2
input_mic3 = const(3)  # Input as diff. mic, use L/RINPUT1 and L/RINPUT3
input_line2 = const(4)  # Input as line input, only use L/RINPUT2
input_line3 = const(5)  # Input as line input, only use L/RINPUT3

# ADC sync input
sync_adc = const(0)  # Use ADCLRC pin for ADC sync
sync_dac = const(1)  # used DACLRC pin for ADC sync

# Protocol type
bus_I2S = const(2)  # I2S type
bus_left_justified = const(1)  # Left justified mode
bus_right_justified = const(0)  # Right justified mode
bus_PCMA = const(3)  # PCM A mode
bus_PCMB = const(3 | (1 << 4))  # PCM B mode

# Channel swap
swap_none = const(0)
swap_input = const(1)
swap_output = const(2)

# Mute settings
mute_fast = const(0)
mute_slow = const(1)

# ALC settings
alc_off = const(0)
alc_right = const(1)
alc_left = const(2)
alc_stereo = const(3)
alc_mode = const(0)  # ALC mode
alc_limiter = const(1)  # Limiter mode

# Clock Source
sysclk_mclk = const(0)  # sysclk source from external MCLK
sysclk_PLL = const(1)  # sysclk source from internal PLL


# fmt: off
class Regs:
    # register cache of 56 register. Since registers cannot be read back, they are
    # kept in the table for modification
    cache = array.array("H", (
        0x0097, 0x0097, 0x0000, 0x0000, 0x0000, 0x0008, 0x0000,
        0x000a, 0x01c0, 0x0000, 0x00ff, 0x00ff, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x007b, 0x0100, 0x0032, 0x0000,
        0x00c3, 0x00c3, 0x01c0, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0050,
        0x0050, 0x0050, 0x0050, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0040, 0x0000, 0x0000, 0x0050, 0x0050, 0x0000, 0x0002,
        0x0037, 0x004d, 0x0080, 0x0008, 0x0031, 0x0026, 0x00e9
    ))
# fmt: on

    def __init__(self, i2c, i2c_address):
        self.value_buffer = bytearray(2)
        self.i2c = i2c
        self.i2c_address = i2c_address

    def __getitem__(self, reg):
        return self.cache[reg]

    def __setitem__(self, reg, value):
        if type(reg) is tuple:
            if type(value) is tuple:
                self[reg[0]] = value[0]
                self[reg[1]] = value[1]
            else:
                self[reg[0]] = value
                self[reg[1]] = value
        else:
            if type(value) is tuple:
                val = (self.cache[reg] & (~value[0] & 0xFFFF)) | value[1]
            else:
                val = value
            self.cache[reg] = val
            self.value_buffer[0] = (reg << 1) | ((val >> 8) & 0x01)
            self.value_buffer[1] = val & 0xFF
            self.i2c.writeto(self.i2c_address, self.value_buffer)


class WM8960:

    _bit_clock_divider_table = {
        2: 0,
        3: 1,
        4: 2,
        6: 3,
        8: 4,
        11: 5,
        12: 6,
        16: 7,
        22: 8,
        24: 9,
        32: 10,
        44: 11,
        48: 12,
    }

    _dac_divider_table = {
        1.0 * 256: 0b000,
        1.5 * 256: 0b001,
        2 * 256: 0b010,
        3 * 256: 0b011,
        4 * 256: 0b100,
        5.5 * 256: 0b101,
        6 * 256: 0b110,
    }

    _audio_word_length_table = {
        16: 0b00,
        20: 0b01,
        24: 0b10,
        32: 0b11,
    }

    _alc_sample_rate_table = {
        48000: 0,
        44100: 0,
        32000: 1,
        24000: 2,
        22050: 2,
        16000: 3,
        12000: 4,
        11025: 4,
        8000: 5,
    }

    def __init__(
        self,
        i2c,
        sample_rate=16000,
        bits=16,
        swap=swap_none,
        route=route_playback_record,
        left_input=input_mic3,
        right_input=input_mic2,
        sysclk_source=sysclk_mclk,
        mclk_freq=None,
        primary=False,
        adc_sync=sync_dac,
        protocol=bus_I2S,
        i2c_address=_I2C_ADDR,
    ):
        self.regs = regs = Regs(i2c, i2c_address)
        self.sample_rate = sample_rate

        # check parameter consistency and set the sysclk value
        if sysclk_source == sysclk_PLL:
            if sample_rate in (11025, 22050, 44100):
                sysclk = 11289600
            else:
                sysclk = 12288000
            if sysclk < sample_rate * 256:
                sysclk = sample_rate * 256
            if mclk_freq is None:
                mclk_freq = sysclk
        else:  # sysclk_source == sysclk_mclk
            if mclk_freq is None:
                mclk_freq = sample_rate * 256
            sysclk = mclk_freq

        regs[_RESET] = 0x00
        # VMID=50K, Enable VREF, AINL, AINR, ADCL and ADCR
        # I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5), ADC (bit 6) are powered on
        regs[_POWER1] = 0xFE
        # Enable DACL, DACR, LOUT1, ROUT1, PLL down, SPKL, SPKR
        regs[_POWER2] = 0x1F8
        # Enable left and right channel input PGA, left and right output mixer
        regs[_POWER3] = 0x3C

        if adc_sync == sync_adc:
            # ADC and DAC use different Frame Clock Pins
            regs[_IFACE2] = 0x00  # ADCLRC 0x00:Input 0x40:output.
        else:
            # ADC and DAC use the same Frame Clock Pin
            regs[_IFACE2] = 0x40  # ADCLRC 0x00:Input 0x40:output.
        self.set_data_route(route)
        self.set_protocol(protocol)

        if sysclk_source == sysclk_PLL:
            self.set_internal_pll_config(mclk_freq, sysclk)
        if primary:
            self.set_master_clock(sysclk, sample_rate, bits)
            # set master bit.
            self.regs[_IFACE1] = (0, _IFACE1_MS_MASK)

        self.set_speaker_clock(sysclk)

        # swap channels
        if swap & swap_input:
            regs[_IFACE1] = (0, _IFACE1_ALRSWAP_MASK)
        if swap & swap_output:
            regs[_IFACE1] = (0, _IFACE1_DLRSWAP_MASK)

        self.set_left_input(left_input)
        self.set_right_input(right_input)

        regs[_ADDCTL1] = 0x0C0
        regs[_ADDCTL4] = 0x60  # Set GPIO1 to 0.

        regs[_BYPASS1] = regs[_BYPASS2] = 0x0
        # ADC volume, 0dB
        regs[_LADC, _RADC] = 0x1C3
        # Digital DAC volume, 0dB
        regs[_LDAC, _RDAC] = 0x1E0
        # Headphone volume, LOUT1 and ROUT1, 0dB
        regs[_LOUT1, _ROUT1] = 0x16F
        # speaker volume 6dB
        regs[_LOUT2, _ROUT2] = 0x1FF
        # enable class D output
        regs[_CLASSD1] = 0xF7
        # Unmute DAC.
        regs[_DACCTL1] = 0x0000
        # Input PGA volume 0 dB
        regs[_LINVOL, _RINVOL] = 0x117

        self.config_data_format(sysclk, sample_rate, bits)

    def deinit(self):

        self.set_module(module_ADC, False)
        self.set_module(module_DAC, False)
        self.set_module(module_VREF, False)
        self.set_module(module_line_in, False)
        self.set_module(module_line_out, False)
        self.set_module(module_speaker, False)

    def set_internal_pll_config(self, input_mclk, output_clk):
        regs = self.regs
        pllF2 = output_clk * 4
        pll_prescale = 0
        sysclk_div = 1
        frac_mode = 0

        # disable PLL power
        regs[_POWER2] = (1, 0)
        regs[_CLOCK1] = (7, 0)

        pllN = pllF2 // input_mclk
        if pllN < _PLL_N_MIN_VALUE:
            input_mclk //= 2
            pll_prescale = 1
            pllN = pllF2 // input_mclk
            if pllN < _PLL_N_MIN_VALUE:
                sysclk_div = 2
                pllF2 *= 2
                pllN = pllF2 // input_mclk

        if (pllN < _PLL_N_MIN_VALUE) or (pllN > _PLL_N_MAX_VALUE):
            raise ValueError("Invalid MCLK vs. sysclk ratio")

        pllK = ((pllF2 % input_mclk) * (1 << 24)) // input_mclk
        if pllK != 0:
            frac_mode = 1

        regs[_PLL1] = (frac_mode << 5) | (pll_prescale << 4) | (pllN & 0x0F)
        regs[_PLL2] = (pllK >> 16) & 0xFF
        regs[_PLL3] = (pllK >> 8) & 0xFF
        regs[_PLL4] = pllK & 0xFF
        # enable PLL power
        regs[_POWER2] = (1, 1)
        regs[_CLOCK1] = (7, ((0 if sysclk_div == 1 else sysclk_div) << 1) | 1)

    def set_master_clock(self, sysclk, sample_rate, bit_width):
        bit_clock_divider = (sysclk * 2) // (sample_rate * bit_width * 2)
        try:
            reg_divider = self._bit_clock_divider_table[bit_clock_divider]
        except:
            raise ValueError("Invalid ratio of sysclk sample rate and bits")
        # configure the master bit clock divider
        self.regs[_CLOCK2] = (_CLOCK2_BCLK_DIV_MASK, reg_divider)

    def set_speaker_clock(self, sysclk):
        speaker_divider_table = (1.5, 2, 3, 4, 6, 8, 12, 16)
        for val in range(8):
            divider = speaker_divider_table[val]
            f = sysclk / divider
            if 500_000 < f < 1_000_000:
                break
        else:
            val = 7
        self.regs[_CLOCK2] = (
            _CLOCK2_DCLK_DIV_MASK,
            val << _CLOCK2_DCLK_DIV_SHIFT,
        )

    def set_module(self, module, is_enabled):

        is_enabled = 1 if is_enabled else 0
        regs = self.regs

        if module == module_ADC:

            regs[_POWER1] = (
                _POWER1_ADCL_MASK | _POWER1_ADCR_MASK,
                (_POWER1_ADCL_MASK | _POWER1_ADCR_MASK) * is_enabled,
            )

        elif module == module_DAC:

            regs[_POWER2] = (
                _POWER2_DACL_MASK | _POWER2_DACR_MASK,
                (_POWER2_DACL_MASK | _POWER2_DACR_MASK) * is_enabled,
            )

        elif module == module_VREF:

            regs[_POWER1] = (
                _POWER1_VREF_MASK,
                (is_enabled << _POWER1_VREF_SHIFT),
            )

        elif module == module_line_in:

            regs[_POWER1] = (
                _POWER1_AINL_MASK | _POWER1_AINR_MASK,
                (_POWER1_AINL_MASK | _POWER1_AINR_MASK) * is_enabled,
            )
            regs[_POWER3] = (
                _POWER3_LMIC_MASK | _POWER3_RMIC_MASK,
                (_POWER3_LMIC_MASK | _POWER3_RMIC_MASK) * is_enabled,
            )

        elif module == module_line_out:

            regs[_POWER2] = (
                _POWER2_LOUT1_MASK | _POWER2_ROUT1_MASK,
                (_POWER2_LOUT1_MASK | _POWER2_ROUT1_MASK) * is_enabled,
            )

        elif module == module_mic_bias:

            regs[_POWER1] = (
                _POWER1_MICB_MASK,
                (is_enabled << _POWER1_MICB_SHIFT),
            )

        elif module == module_speaker:

            regs[_POWER2] = (
                _POWER2_SPKL_MASK | _POWER2_SPKR_MASK,
                (_POWER2_SPKL_MASK | _POWER2_SPKR_MASK) * is_enabled,
            )
            regs[_CLASSD1] = 0xF7

        elif module == module_omix:

            regs[_POWER3] = (
                _POWER3_LOMIX_MASK | _POWER3_ROMIX_MASK,
                (_POWER3_LOMIX_MASK | _POWER3_ROMIX_MASK) * is_enabled,
            )

        elif module == module_mono_out:

            regs[_MONOMIX1] = regs[_MONOMIX2] = is_enabled << 7
            regs[_MONO] = is_enabled << 6

        else:
            raise ValueError("Invalid module")

    def enable_module(self, module):
        self.set_module(module, True)

    def disable_module(self, module):
        self.set_module(module, False)

    def set_data_route(self, route):
        regs = self.regs
        if route == route_bypass:
            # Bypass means from line-in to HP
            # Left LINPUT3 to left output mixer, LINPUT3 left output mixer volume = 0dB
            # Right RINPUT3 to right output mixer, RINPUT3 right output mixer volume = 0dB
            regs[_LOUTMIX, _ROUTMIX] = 0x80

        elif route == route_playback:
            # Data route I2S_IN-> DAC-> HP
            #
            # Left DAC to left output mixer, LINPUT3 left output mixer volume = 0dB
            # Right DAC to right output mixer, RINPUT3 right output mixer volume = 0dB
            regs[_LOUTMIX, _ROUTMIX] = 0x100
            regs[_POWER3] = 0x0C
            # Set power for DAC
            self.set_module(module_DAC, True)
            self.set_module(module_omix, True)
            self.set_module(module_line_out, True)

        elif route == route_playback_record:
            #
            # Left DAC to left output mixer, LINPUT3 left output mixer volume = 0dB
            # Right DAC to right output mixer, RINPUT3 right output mixer volume = 0dB
            regs[_LOUTMIX, _ROUTMIX] = 0x100
            regs[_POWER3] = 0x3C
            self.set_module(module_DAC, True)
            self.set_module(module_ADC, True)
            self.set_module(module_line_in, True)
            self.set_module(module_omix, True)
            self.set_module(module_line_out, True)

        elif route == route_record:
            # LINE_IN->ADC->I2S_OUT
            # Left and right input boost, LIN3BOOST and RIN3BOOST = 0dB
            regs[_POWER3] = 0x30
            # Power up ADC and AIN
            self.set_module(module_line_in, True)
            self.set_module(module_ADC, True)

        else:
            raise ValueError("Invalid route")

    def set_left_input(self, input):

        regs = self.regs
        if input == input_closed:
            # Disable the input
            regs[_POWER1] = (_POWER1_AINL_MASK | _POWER1_ADCL_MASK, 0)

        elif input == input_mic1:
            # Only LMN1 enabled, LMICBOOST to 13db, LMIC2B enabled
            regs[_POWER1] = (
                0,
                _POWER1_AINL_MASK | _POWER1_ADCL_MASK | _POWER1_MICB_MASK,
            )
            regs[_LINPATH] = 0x138
            regs[_LINVOL] = 0x117

        elif input == input_mic2:
            regs[_POWER1] = (
                0,
                _POWER1_AINL_MASK | _POWER1_ADCL_MASK | _POWER1_MICB_MASK,
            )
            regs[_LINPATH] = 0x178
            regs[_LINVOL] = 0x117

        elif input == input_mic3:
            regs[_POWER1] = (
                0,
                _POWER1_AINL_MASK | _POWER1_ADCL_MASK | _POWER1_MICB_MASK,
            )
            regs[_LINPATH] = 0x1B8
            regs[_LINVOL] = 0x117

        elif input == input_line2:
            regs[_POWER1] = (0, _POWER1_AINL_MASK | _POWER1_ADCL_MASK)
            regs[_INBMIX1] = (0, 0xE)

        elif input == input_line3:
            regs[_POWER1] = (0, _POWER1_AINL_MASK | _POWER1_ADCL_MASK)
            regs[_INBMIX1] = (0, 0x70)

        else:
            raise ValueError("Invalid input name")

    def set_right_input(self, input):

        regs = self.regs
        if input == input_closed:
            # Disable the input
            regs[_POWER1] = (_POWER1_AINR_MASK | _POWER1_ADCR_MASK, 0)

        elif input == input_mic1:
            # Only LMN1 enabled, LMICBOOST to 13db, LMIC2B enabled
            regs[_POWER1] = (
                0,
                _POWER1_AINR_MASK | _POWER1_ADCR_MASK | _POWER1_MICB_MASK,
            )
            regs[_RINPATH] = 0x138
            regs[_RINVOL] = 0x117

        elif input == input_mic2:
            regs[_POWER1] = (
                0,
                _POWER1_AINR_MASK | _POWER1_ADCR_MASK | _POWER1_MICB_MASK,
            )
            regs[_RINPATH] = 0x178
            regs[_RINVOL] = 0x117

        elif input == input_mic3:
            regs[_POWER1] = (
                0,
                _POWER1_AINR_MASK | _POWER1_ADCR_MASK | _POWER1_MICB_MASK,
            )
            regs[_RINPATH] = 0x1B8
            regs[_RINVOL] = 0x117

        elif input == input_line2:
            regs[_POWER1] = (0, _POWER1_AINR_MASK | _POWER1_ADCR_MASK)
            regs[_INBMIX2] = (0, 0xE)

        elif input == input_line3:
            regs[_POWER1] = (0, _POWER1_AINR_MASK | _POWER1_ADCR_MASK)
            regs[_INBMIX2] = (0, 0x70)

        else:
            raise ValueError("Invalid input name")

    def set_protocol(self, protocol):
        self.regs[_IFACE1] = (
            _IFACE1_FORMAT_MASK | _IFACE1_LRP_MASK,
            protocol,
        )

    def config_data_format(self, sysclk, sample_rate, bits):
        #  Compute sample rate divider, dac and adc are the same sample rate
        try:
            divider = self._dac_divider_table[sysclk // sample_rate]
            wl = self._audio_word_length_table[bits]
        except:
            raise ValueError("Invalid ratio sysclk/sample_rate or invalid bit length")

        self.regs[_CLOCK1] = (0x1F8, divider << 6 | divider << 3)
        self.regs[_IFACE1] = (_IFACE1_WL_MASK, wl << _IFACE1_WL_SHIFT)

    def set_volume(self, module, volume, volume_r=None):

        regs = self.regs
        if volume_r is None:
            volume_r = volume

        if module == module_ADC:
            if volume <= _MAX_VOLUME_ADC:
                regs[_LADC, _RADC] = volume | 0x100, volume_r | 0x100

        elif module == module_DAC:
            if volume <= _MAX_VOLUME_DAC:
                regs[_LDAC, _RDAC] = volume | 0x100, volume_r | 0x100

        elif module == module_headphone:
            if volume <= _MAX_VOLUME_HEADPHONE:
                regs[_LOUT1, _ROUT1] = volume | 0x180, volume_r | 0x180

        elif module == module_line_in:
            if volume <= _MAX_VOLUME_LINEIN:
                regs[_LINVOL, _LINVOL] = volume | 0x140, volume_r | 0x140

        elif module == module_speaker:
            if volume <= _MAX_VOLUME_SPEAKER:
                regs[_LOUT2, _LOUT2] = volume | 0x180,  volume_r | 0x180
        else:
            raise ValueError("Invalid module")

    def get_volume(self, module):

        regs = self.regs
        if module == module_ADC:
            volume = (regs[_LADC] & _MAX_VOLUME_ADC, regs[_RADC] & _MAX_VOLUME_ADC)

        elif module == module_DAC:
            volume = (regs[_LDAC] & _MAX_VOLUME_DAC, regs[_RDAC] & _MAX_VOLUME_DAC)

        elif module == module_headphone:
            volume = (regs[_LOUT1] & _MAX_VOLUME_HEADPHONE, regs[_ROUT1] & _MAX_VOLUME_HEADPHONE)

        elif module == module_line_in:
            volume = (regs[_LINVOL] & _MAX_VOLUME_LINEIN, regs[_RINVOL] & _MAX_VOLUME_LINEIN)

        elif module == module_speaker:
            volume = (regs[_LOUT2] & _MAX_VOLUME_SPEAKER, regs[_ROUT2] & _MAX_VOLUME_SPEAKER)

        else:
            volume = 0, 0

        return volume

    def volume(self, module, volume_l=None, volume_r=None):
        if volume_l is None:
            return self.get_volume(module)
        else:
            self.set_volume(module, volume_l, volume_r)

    def mute(self, enable, soft=True, ramp=mute_fast):
        enable = _DACCTL1_DACMU_MASK if enable else 0
        soft = _DACCTL2_DACSMM_MASK if soft else 0
        ramp = _DACCTL2_DACMR_MASK if ramp == mute_slow else 0
        self.regs[_DACCTL1] = (_DACCTL1_DACMU_MASK, enable)
        self.regs[_DACCTL2] = (
            _DACCTL2_DACSMM_MASK | _DACCTL2_DACMR_MASK,
            soft | ramp,
        )

    def expand_3d(self, depth=0):
        if depth > 15:
            depth = 15
        cutoff = 0 if self.sample_rate >= 32000 else 0b1100000
        self.regs[_3D] = cutoff | depth << 1 | (1 if depth > 0 else 0)

    def mono(self, enable):
        enable = 1 if enable else 0
        self.regs[_DACCTL1] = (
            _DACCTL1_MONOMIX_MASK,
            enable << _DACCTL1_MONOMIX_SHIFT,
        )

    def alc_mode(self, channel, mode=alc_mode):
        if mode != alc_mode:
            mode = alc_limiter
        channel &= 3
        self.regs[_ALC1] = (
            _ALC_CHANNEL_MASK,
            channel << _ALC_CHANNEL_SHIFT,
        )
        self.regs[_ALC3] = (_ALC_MODE_MASK, mode << _ALC_MODE_SHIFT)
        try:
            rate = _alc_sample_rate_table[self.sample_rate]
        except:
            rate = 0
        self.regs[_ADDCTL3] = (_DACCTL3_ALCSR_MASK, rate)

    def alc_gain(self, target=-12, max_gain=30, min_gain=-17.25, noise_gate=-78):
        def limit(value, minval, maxval):
            value = int(value)
            if value < minval:
                value = minval
            if value > maxval:
                value = maxval
            return value

        target = limit((16 + (target * 2) // 3), 0, 15)
        max_gain = limit((max_gain + 12) // 6, 0, 7)
        min_gain = limit((min_gain * 4 + 69) // 24, 0, 7)
        noise_gate = limit((noise_gate * 2 + 153) // 3, -1, 31)
        self.regs[_ALC1] = (
            _ALC_GAIN_MASK | _ALC_TARGET_MASK,
            (max_gain << _ALC_GAIN_SHIFT) | target,
        )
        self.regs[_ALC2] = (_ALC_GAIN_MASK, (min_gain << _ALC_GAIN_SHIFT))
        if noise_gate >= 0:
            self.regs[_NOISEG] = noise_gate << _NOISEG_LEVEL_SHIFT | 1
        else:
            self.regs[_NOISEG] = 0

    def alc_time(self, attack=24, decay=192, hold=0):
        def logb(value, limit):
            value = int(value)
            lb = 0
            while value > 1:
                value >>= 1
                lb += 1
            if lb > limit:
                lb = limit
            return lb

        attack = logb(attack / 6, 7)
        decay = logb(decay / 24, 7)
        hold = logb((hold * 3) / 8, 15)
        self.regs[_ALC2] = (_ALC_HOLD_MASK, hold)
        self.regs[_ALC3] = (
            _ALC_DECAY_MASK | _ALC_ATTACK_MASK,
            (decay << _ALC_DECAY_SHIFT) | attack,
        )

    def deemphasis(self, enable):
        deem_table = (32000, 44100, 48000)
        enable = not not enable
        if enable and self.sample_rate in deem_table:
            val = deem_table.index(self.sample_rate) + 1
        else:
            val = 0
        self.regs[_DACCTL1] = (_DACCTL1_DEEM_MASK, val << _DACCTL1_DEEM_SHIFT)

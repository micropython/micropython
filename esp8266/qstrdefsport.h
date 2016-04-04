/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// qstrs specific to this port

Q(help)

// pyb module
Q(pyb)
Q(info)
Q(freq)
Q(millis)
Q(elapsed_millis)
Q(micros)
Q(elapsed_micros)
Q(delay)
Q(udelay)
Q(sync)
Q(unique_id)

// uos module
Q(uos)
Q(os)
Q(uname)
Q(sysname)
Q(nodename)
Q(release)
Q(version)
Q(machine)

Q(esp)
Q(socket)
Q(usocket)
Q(connect)
Q(disconnect)
Q(wifi_mode)
Q(phy_mode)
Q(sleep_type)
Q(deepsleep)
Q(adc)
Q(vdd33)
Q(chip_id)
Q(flash_id)
Q(flash_read)
Q(flash_write)
Q(flash_erase)
Q(sdk_version)
Q(getaddrinfo)
Q(send)
Q(sendto)
Q(recv)
Q(recvfrom)
Q(listen)
Q(accept)
Q(bind)
Q(settimeout)
Q(setblocking)
Q(setsockopt)
Q(close)
Q(protocol)
Q(getpeername)
Q(onconnect)
Q(onrecv)
Q(onsent)
Q(ondisconnect)
Q(neopixel_write)
Q(MODE_11B)
Q(MODE_11G)
Q(MODE_11N)
Q(SLEEP_NONE)
Q(SLEEP_LIGHT)
Q(SLEEP_MODEM)
Q(STA_MODE)
Q(AP_MODE)
Q(STA_AP_MODE)

// network module
Q(network)
Q(WLAN)
Q(active)
Q(scan)
Q(status)
Q(isconnected)
Q(mac)
Q(config)
Q(ifconfig)
Q(STA_IF)
Q(AP_IF)
Q(STAT_IDLE)
Q(STAT_CONNECTING)
Q(STAT_WRONG_PASSWORD)
Q(STAT_NO_AP_FOUND)
Q(STAT_CONNECT_FAIL)
Q(STAT_GOT_IP)
// config keys
Q(essid)

// Pin class
Q(Pin)
Q(init)
Q(mode)
Q(pull)
Q(value)
Q(low)
Q(high)
Q(IN)
Q(OUT)
Q(OPEN_DRAIN)
Q(PULL_NONE)
Q(PULL_UP)
Q(PULL_DOWN)

// PWM class
Q(PWM)
Q(init)
Q(deinit)
Q(freq)
Q(duty)

// RTC
Q(RTC)
Q(datetime)
Q(memory)

// ADC
Q(ADC)
Q(read)

// I2C
Q(I2C)
Q(init)
Q(scl)
Q(sda)
Q(freq)
Q(readfrom)
Q(writeto)
Q(stop)
Q(buf)
Q(addr)
Q(n)

// SPI
Q(SPI)
Q(init)
Q(baudrate)
Q(phase)
Q(polarity)
Q(sck)
Q(mosi)
Q(miso)
Q(read)
Q(readinto)
Q(write)
Q(write_readinto)

// utime
Q(utime)
Q(localtime)
Q(mktime)
Q(sleep)
Q(sleep_ms)
Q(sleep_us)
Q(ticks_ms)
Q(ticks_us)
Q(ticks_cpu)
Q(ticks_diff)
Q(time)

// machine
Q(reset)
Q(Timer)
Q(callback)
Q(deinit)
Q(init)
Q(mode)
Q(period)
Q(ONE_SHOT)
Q(PERIODIC)

// onewire
Q(_onewire)
Q(onewire)
Q(timings)
Q(reset)
Q(readbit)
Q(readbyte)
Q(writebit)
Q(writebyte)
Q(crc8)

Q(json)

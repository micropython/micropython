/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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
Q(__name__)
Q(help)
Q(pyb)
Q(info)
Q(reset)
Q(main)
Q(sync)
Q(gc)
Q(rng)
Q(delay)
Q(time)
Q(open)
Q(on)
Q(off)
Q(toggle)
Q(write)
Q(read)
Q(readall)
Q(readline)
Q(input)
Q(os)
Q(freq)
Q(repl_info)
Q(disable_irq)
Q(enable_irq)
Q(millis)
Q(micros)
Q(elapsed_millis)
Q(elapsed_micros)
Q(udelay)
Q(version)
Q(flush)
Q(FileIO)
Q(mkdisk)
Q(enable)
Q(disable)
// Entries for sys.path
Q(/flash)
Q(/flash/lib)
#if MICROPY_HW_HAS_SDCARD
Q(/sd)
Q(/sd/lib)
#endif

// for module weak links
Q(re)
Q(json)
Q(heapq)

// for os module
Q(uos)
Q(os)
Q(/)
Q(flash)
#if MICROPY_HW_HAS_SDCARD
Q(sd)
#endif
Q(chdir)
Q(getcwd)
Q(listdir)
Q(mkdir)
Q(remove)
Q(rmdir)
Q(unlink)
Q(sep)
Q(stat)
Q(urandom)

// for file class
Q(seek)
Q(tell)

// for Pin class
Q(Pin)
Q(init)
Q(value)
Q(low)
Q(high)
Q(name)
Q(port)
Q(pin)
Q(cpu)
Q(mode)
Q(pull)
Q(index)
Q(strength)
Q(af)
Q(intenable)
Q(intdisable)
Q(intmode)
Q(swint)
Q(IN)
Q(OUT)
Q(STD)
Q(STD_PU)
Q(STD_PD)
Q(OD)
Q(OD_PU)
Q(OD_PD)
Q(INT_RISING)
Q(INT_FALLING)
Q(INT_RISING_FALLING)
Q(INT_LOW_LEVEL)
Q(INT_HIGH_LEVEL)
Q(S2MA)
Q(S4MA)
Q(S6MA)

// for UART class
Q(UART)
Q(baudrate)
Q(bits)
Q(stop)
Q(parity)
Q(init)
Q(deinit)
Q(all)
Q(writechar)
Q(readchar)
Q(readinto)
Q(read_buf_len)
Q(timeout)
Q(timeout_char)
Q(repl_uart)
Q(flow)
Q(FLOW_NONE)
Q(FLOW_TX)
Q(FLOW_RX)
Q(FLOW_TXRX)

// for I2C class
Q(I2C)
Q(addr)
Q(data)
Q(memaddr)
Q(addr_size)
Q(init)
Q(deinit)
Q(is_ready)
Q(scan)
Q(send)
Q(recv)
Q(mem_read)
Q(mem_write)

// for ADC class
Q(ADC)
Q(read)

#if MICROPY_HW_HAS_SDCARD
// for SD class
Q(SD)
Q(enable)
Q(disable)
#endif

// for RTC class
Q(RTC)
Q(datetime)

// for time class
Q(utime)
Q(localtime)
Q(mktime)
Q(sleep)
Q(time)

// for select class
Q(select)
Q(uselect)
Q(register)
Q(unregister)
Q(modify)
Q(poll)

// for socket class
Q(socket)
Q(usocket)
Q(getaddrinfo)
Q(family)
Q(type)
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
Q(AF_INET)
Q(AF_INET6)
Q(SOCK_STREAM)
Q(SOCK_DGRAM)
Q(SOCK_RAW)
Q(IPPROTO_SEC)
Q(IPPROTO_TCP)
Q(IPPROTO_UDP)
Q(IPPROTO_RAW)

// for network class
Q(network)
Q(route)
Q(start_server)
Q(stop_server)
Q(server_enabled)
Q(server_login)

// for WLAN class
Q(WLAN)
Q(key)
Q(security)
Q(ssid)
Q(bssid)
Q(scan)
Q(connect)
Q(isconnected)
Q(disconnect)
Q(channel)
Q(ifconfig)
Q(urn)
Q(mode)
Q(config_ip)
Q(ip)
Q(subnet)
Q(gateway)
Q(dns)
Q(STA)
Q(AP)
Q(P2P)
Q(OPEN)
Q(WEP)
Q(WPA_WPA2)
Q(WPA_ENT)
Q(WPS_PBC)
Q(WPS_PIN)
Q(DYNAMIC)
Q(STATIC)

// for WDT class
Q(WDT)
Q(enable)
Q(kick)

// for HeartBeat class
Q(HeartBeat)
Q(enable)
Q(disable)

// for callback class
Q(init)
Q(enable)
Q(disable)
Q(callback)
Q(handler)
Q(intmode)
Q(value)
Q(priority)
Q(wakes)

// for Sleep class
Q(Sleep)
Q(idle)
Q(suspend)
Q(hibernate)
Q(reset_cause)
Q(wake_reason)
Q(ACTIVE)
Q(SUSPENDED)
Q(HIBERNATING)
Q(PWR_ON_RESET)
Q(HARD_RESET)
Q(WDT_RESET)
Q(HIB_RESET)
Q(SOFT_RESET)
Q(WLAN_WAKE)
Q(PIN_WAKE)
Q(RTC_WAKE)

// for SPI class
Q(SPI)
Q(baudrate)
Q(bits)
Q(submode)
Q(cs)
Q(init)
Q(deinit)
Q(send)
Q(recv)
Q(send_recv)
Q(ACTIVE_LOW)
Q(ACTIVE_HIGH)


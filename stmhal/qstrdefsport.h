/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
Q(pyb)
Q(unique_id)
Q(bootloader)
Q(hard_reset)
Q(info)
Q(sd_test)
Q(present)
Q(power)
Q(wfi)
Q(disable_irq)
Q(enable_irq)
Q(stop)
Q(standby)
Q(main)
Q(opt)
Q(sync)
Q(gc)
Q(repl_info)
Q(delay)
Q(udelay)
Q(servo)
Q(pwm)
Q(read)
Q(readall)
Q(readline)
Q(write)
Q(repl_uart)
Q(time)
Q(rng)
Q(SD)
Q(SDcard)
Q(FileIO)
Q(flush)
Q(boot.py)
Q(main.py)
// Entries for sys.path
Q(/flash)
Q(/flash/lib)
Q(/sd)
Q(/sd/lib)
Q(millis)
Q(micros)
Q(elapsed_millis)
Q(elapsed_micros)

// for user-mountable block devices
Q(mount)
Q(readonly)
Q(mkfs)
Q(readblocks)
Q(writeblocks)
Q(sync)
Q(count)

// for module weak links
Q(binascii)
Q(re)
Q(zlib)
Q(json)
Q(heapq)
Q(hashlib)

// for USB configuration
Q(usb_mode)
Q(mode)
Q(vid)
Q(pid)
Q(hid)
Q(hid_mouse)
Q(hid_keyboard)

// for usb modes
Q(host)
Q(VCP)
Q(MSC)
Q(HID)
Q(MSC+HID)
Q(VCP+MSC)
Q(VCP+HID)
// CDC is a synonym for VCP for backwards compatibility
Q(CDC)
Q(CDC+MSC)
Q(CDC+HID)

// for USB VCP class
Q(USB_VCP)
Q(setinterrupt)
Q(isconnected)
Q(have_cdc)
Q(any)
Q(send)
Q(recv)
Q(timeout)

// for USB HID class
Q(USB_HID)
Q(any)
Q(send)
Q(recv)

// for RTC class
Q(RTC)
Q(info)
Q(datetime)
Q(wakeup)
Q(calibration)

// for Pin class
Q(Pin)
Q(PinAF)
Q(PinNamed)
Q(init)
Q(value)
Q(low)
Q(high)
Q(name)
Q(names)
Q(af)
Q(af_list)
Q(port)
Q(pin)
Q(gpio)
Q(mapper)
Q(dict)
Q(debug)
Q(board)
Q(cpu)
Q(af)
Q(mode)
Q(pull)
Q(index)
Q(reg)
Q(IN)
Q(OUT_PP)
Q(OUT_OD)
Q(AF_PP)
Q(AF_OD)
Q(ANALOG)
Q(PULL_NONE)
Q(PULL_UP)
Q(PULL_DOWN)

// for LED object
Q(LED)
Q(on)
Q(off)
Q(toggle)
Q(intensity)

// for Switch class
Q(Switch)
Q(callback)

// for UART class
Q(UART)
Q(baudrate)
Q(bits)
Q(stop)
Q(parity)
Q(flow)
Q(read_buf_len)
Q(buf)
Q(len)
Q(timeout)
Q(timeout_char)
Q(init)
Q(deinit)
Q(any)
Q(writechar)
Q(readchar)
Q(readinto)
Q(sendbreak)
Q(RTS)
Q(CTS)

// for CAN class
#if MICROPY_HW_ENABLE_CAN
Q(CAN)
Q(prescaler)
Q(init)
Q(deinit)
Q(all)
Q(send)
Q(recv)
Q(addr)
Q(fifo)
Q(timeout)
Q(extframe)
Q(sjw)
Q(bs1)
Q(bs2)
Q(bank)
Q(params)
Q(initfilterbanks)
Q(clearfilter)
Q(setfilter)
Q(rxcallback)
Q(rtr)
Q(NORMAL)
Q(LOOPBACK)
Q(SILENT)
Q(SILENT_LOOPBACK)
Q(MASK16)
Q(LIST16)
Q(MASK32)
Q(LIST32)
#endif

// for Timer class
Q(Timer)
Q(init)
Q(deinit)
Q(channel)
Q(counter)
Q(source_freq)
Q(prescaler)
Q(period)
Q(callback)
Q(freq)
Q(mode)
Q(div)
Q(UP)
Q(DOWN)
Q(CENTER)
Q(IC)
Q(PWM)
Q(PWM_INVERTED)
Q(OC_TIMING)
Q(OC_ACTIVE)
Q(OC_INACTIVE)
Q(OC_TOGGLE)
Q(OC_FORCED_ACTIVE)
Q(OC_FORCED_INACTIVE)
Q(ENC_A)
Q(ENC_B)
Q(ENC_AB)
Q(HIGH)
Q(LOW)
Q(RISING)
Q(FALLING)
Q(BOTH)

// for TimerChannel class
Q(TimerChannel)
Q(pulse_width)
Q(pulse_width_percent)
Q(compare)
Q(capture)
Q(polarity)
Q(deadtime)

// for ExtInt class
Q(ExtInt)
Q(pin)
Q(mode)
Q(pull)
Q(callback)
Q(line)
Q(enable)
Q(disable)
Q(swint)
Q(regs)
Q(IRQ_RISING)
Q(IRQ_FALLING)
Q(IRQ_RISING_FALLING)
Q(EVT_RISING)
Q(EVT_FALLING)
Q(EVT_RISING_FALLING)

// for I2C object
Q(I2C)
Q(mode)
Q(addr)
Q(baudrate)
Q(gencall)
Q(data)
Q(memaddr)
Q(addr_size)
Q(timeout)
Q(init)
Q(deinit)
Q(is_ready)
Q(scan)
Q(send)
Q(recv)
Q(mem_read)
Q(mem_write)

// for SPI class
Q(SPI)
Q(init)
Q(deinit)
Q(send)
Q(recv)
Q(send_recv)
Q(mode)
Q(baudrate)
Q(polarity)
Q(phase)
Q(dir)
Q(bits)
Q(nss)
Q(firstbit)
Q(ti)
Q(crc)
Q(MASTER)
Q(SLAVE)
Q(MSB)
Q(LSB)

// for Accel object
Q(Accel)
Q(x)
Q(y)
Q(z)
Q(tilt)
Q(filtered_xyz)

// for ADC object
Q(ADC)
Q(ADCAll)
Q(read_timed)
Q(read_channel)
Q(read_core_temp)
Q(read_core_vbat)
Q(read_core_vref)

// for DAC class
Q(DAC)
Q(noise)
Q(triangle)
Q(write)
Q(write_timed)
Q(data)
Q(freq)
Q(mode)
Q(NORMAL)
Q(CIRCULAR)

// for Servo object
Q(Servo)
Q(pulse_width)
Q(calibration)
Q(angle)
Q(speed)

// for os module
Q(uos)
Q(os)
Q(/)
Q(flash)
Q(sd)
Q(sysname)
Q(nodename)
Q(release)
Q(version)
Q(machine)
Q(uname)
Q(chdir)
Q(getcwd)
Q(listdir)
Q(mkdir)
Q(rename)
Q(remove)
Q(rmdir)
Q(unlink)
Q(sep)
Q(stat)
Q(urandom)

// for time module
Q(utime)
Q(time)
Q(localtime)
Q(mktime)
Q(sleep)

// for select module
Q(uselect)
Q(select)
Q(poll)
Q(register)
Q(unregister)
Q(modify)

// for input
Q(input)

// for LCD class
Q(LCD)
Q(command)
Q(contrast)
Q(light)
Q(fill)
Q(get)
Q(pixel)
Q(text)
Q(show)

// for usocket module
Q(usocket)
Q(socket)
Q(getaddrinfo)
Q(bind)
Q(listen)
Q(accept)
Q(connect)
Q(send)
Q(recv)
Q(sendto)
Q(recvfrom)
Q(setblocking)
Q(setsockopt)
Q(settimeout)
Q(close)
Q(AF_INET)
Q(AF_INET6)
Q(SOCK_STREAM)
Q(SOCK_DGRAM)
Q(SOCK_RAW)

// for network module
Q(network)
Q(route)

// for WIZNET5K class
#if MICROPY_PY_WIZNET5K
Q(WIZNET5K)
Q(regs)
Q(ifconfig)
Q(ipaddr)
#endif

// for CC3K class
#if MICROPY_PY_CC3K
Q(CC3K)
Q(connect)
Q(disconnect)
Q(isconnected)
Q(ifconfig)
Q(patch_version)
Q(patch_program)
Q(WEP)
Q(WPA)
Q(WPA2)
Q(ssid)
Q(key)
Q(security)
Q(bssid)
#endif

// for stm module
Q(stm)
Q(mem)
Q(mem8)
Q(mem16)
Q(mem32)

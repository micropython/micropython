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

Q(Test)

Q(fileno)
Q(makefile)

Q(FileIO)
Q(flush)

Q(uos)
Q(stat)
#if MICROPY_PY_OS_STATVFS
Q(statvfs)
#endif
Q(system)
Q(unlink)
Q(getenv)
Q(mkdir)
Q(ilistdir)
Q(errno)
#if MICROPY_FSUSERMOUNT
Q(vfs_mount)
Q(vfs_umount)
Q(vfs_mkfs)
#endif
#if MICROPY_VFS_FAT
Q(VfsFat)
#endif
#if MICROPY_PY_OS_DUPTERM
Q(dupterm)
#endif

Q(uselect)
Q(poll)
Q(register)
Q(unregister)
Q(modify)
Q(POLLIN)
Q(POLLOUT)
Q(POLLERR)
Q(POLLHUP)

Q(ffi)
Q(ffimod)
Q(ffifunc)
Q(fficallback)
Q(ffivar)
Q(as_bytearray)
Q(callback)
Q(addr)
Q(func)
Q(var)
Q(get)
Q(set)

Q(input)
Q(utime)
Q(time)
Q(clock)
Q(sleep)
Q(sleep_ms)
Q(sleep_us)
Q(ticks_ms)
Q(ticks_us)
Q(ticks_diff)
Q(strftime)

Q(socket)
Q(sockaddr)
Q(htons)
Q(inet_pton)
Q(inet_ntop)
Q(gethostbyname)
Q(getaddrinfo)
Q(usocket)
Q(connect)
Q(bind)
Q(listen)
Q(accept)
Q(recv)
Q(recvfrom)
Q(sendto)
Q(setsockopt)
Q(setblocking)

Q(AF_UNIX)
Q(AF_INET)
Q(AF_INET6)
Q(SOCK_STREAM)
Q(SOCK_DGRAM)
Q(SOCK_RAW)

Q(MSG_DONTROUTE)
Q(MSG_DONTWAIT)

Q(SOL_SOCKET)
Q(SO_BROADCAST)
Q(SO_ERROR)
Q(SO_KEEPALIVE)
Q(SO_LINGER)
Q(SO_REUSEADDR)

#if MICROPY_PY_TERMIOS
Q(termios)
Q(tcgetattr)
Q(tcsetattr)
Q(setraw)
Q(TCSANOW)
Q(B9600)
Q(B57600)
Q(B115200)
#endif

#if MICROPY_PY_JNI
Q(jni)
Q(cls)
Q(env)
Q(jclass)
Q(jobject)
Q(jmethod)
#endif

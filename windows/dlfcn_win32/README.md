dlfcn-win32 [![Build status](https://ci.appveyor.com/api/projects/status/celmbkqj0k41fk04/branch/master?svg=true)](https://ci.appveyor.com/project/dlfcn-win32/dlfcn-win32/branch/master)
===========

dlfcn-win32 is an implementation of dlfcn for Windows.

dlfcn is a set of functions that allows runtime dynamic library loading. It is
standardized in the POSIX. Windows also provide similar routines, but not in a
POSIX-compatible way. This library attempts to implement a wrapper around the
Windows functions to make programs written for POSIX that use dlfcn work in
Windows without any modifications.

It follows the standard as described here:

http://www.opengroup.org/onlinepubs/009695399/basedefs/dlfcn.h.html
http://www.opengroup.org/onlinepubs/009695399/functions/dlerror.html
http://www.opengroup.org/onlinepubs/009695399/functions/dlsym.html
http://www.opengroup.org/onlinepubs/009695399/functions/dlclose.html
http://www.opengroup.org/onlinepubs/009695399/functions/dlopen.html

Using This Library
------------------

This library uses the Process Status API in Windows (`psapi.lib`). If you are
linking to the static `dl.lib` or `libdl.a`, then you would need to explicitly
add `psapi.lib` or `-lpsapi` to your linking command, depending on if MinGW is
used.

Author
------

Written by Ramiro Polla in 2007.
Maintained by Tiancheng "Timothy" Gu from 2013.

License
-------

dlfcn-win32 is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

dlfcn-win32 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with dlfcn-win32; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

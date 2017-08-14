The internal filesystem
=======================

If your devices has 1Mbyte or more of storage then it will be set up (upon first
boot) to contain a filesystem.  This filesystem uses the FAT format and is
stored in the flash after the MicroPython firmware.

Creating and reading files
--------------------------

MicroPython on the ESP8266 supports the standard way of accessing files in
Python, using the built-in ``open()`` function.

To create a file try::

    >>> f = open('data.txt', 'w')
    >>> f.write('some data')
    9
    >>> f.close()

The "9" is the number of bytes that were written with the ``write()`` method.
Then you can read back the contents of this new file using::

    >>> f = open('data.txt')
    >>> f.read()
    'some data'
    >>> f.close()

Note that the default mode when opening a file is to open it in read-only mode,
and as a text file.  Specify ``'wb'`` as the second argument to ``open()`` to
open for writing in binary mode, and ``'rb'`` to open for reading in binary
mode.

Listing file and more
---------------------

The os module can be used for further control over the filesystem.  First
import the module::

    >>> import os

Then try listing the contents of the filesystem::

    >>> os.listdir()
    ['boot.py', 'port_config.py', 'data.txt']

You can make directories::

    >>> os.mkdir('dir')

And remove entries::

    >>> os.remove('data.txt')

Start up scripts
----------------

There are two files that are treated specially by the ESP8266 when it starts up:
boot.py and main.py.  The boot.py script is executed first (if it exists) and
then once it completes the main.py script is executed.  You can create these
files yourself and populate them with the code that you want to run when the
device starts up.

Accessing the filesystem via WebREPL
------------------------------------

You can access the filesystem over WebREPL using the web client in a browser
or via the command-line tool. Please refer to Quick Reference and Tutorial
sections for more information about WebREPL.

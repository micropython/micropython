Linux kernel MicroPython port
=============================

This is an experimental port of MicroPython that runs as a Linux kernel
module. MicroPython runs as a kernel thread and is able to access all kernel
functions and data.

Build for a kernel
------------------

You need an evironment for building kernel modules. In many Linux distributions,
you'll have to install a ``linux-headers`` package matching your kernel version.

Once your environment is set up, it's as easy as:

.. code-block:: bash

    $ make -C ports/linux-kernel

You'll get ``build/mpy.ko``, which can be loaded on the current machine with ``insmod``.

Build for a specific kernel version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can pass the ``KDIR`` variable to ``make`` to build against a specific kernel tree.

.. code-block:: bash

    $ make -C ports/linux-kernel KDIR=/path/to/some/other/kernel


Using it
--------

Currently, the only exposed interface is a Python REPL. (More will be supported in the future).
After being loaded, the module creates a server listening on ``0.0.0.0:9999``.

The following ``socat`` command will start a full interactive shell:

.. code-block:: bash

    $ socat file:`tty`,raw,echo=0,escape=0xc tcp:<IP>:9999

You can exit with Ctrl+D which will also kill the shell (it will be restarted upon the next
connection).
If the shell hangs, you can exit with Ctrl+L.

Features
--------

The Python interpreter allows quick testing and prototyping with kernel APIs.

Kernel symbols
^^^^^^^^^^^^^^

Upon access, any name that doesn't exist globally will be searched using ``find_symbol``
(exported symbols). If not found, an attempt will be made at ``kallsyms_lookup_name``, which
covers unexported symbols as well.

If found in any of them, the result is a ``Symbol`` object, which:

* Can be called as if it was a function; Don't use on non functions though, as you'll probably
  crash.
* Can be manipulated as an integer in expressions (address calculation, etc...)

Access kernel memory
^^^^^^^^^^^^^^^^^^^^

The ``kernel_ffi`` module provides the following helpers:

``str(p)`` - Reads a null terminated string from memory into a Python ``str``..

``bytes(p, n)`` - Reads ``n`` bytes from memory into a Python ``bytes``.

``p64/p32/p16/p8(p)`` - Dereference a pointer value and give the result.

I recommend to import them this way for faster typing.

.. code-block:: python

    from kernel_ffi import str as s, bytes as b; from kernel_ffi import *

For example:

.. code-block:: python

    >>> p64(jiffies_64)
    4295353334
    >>> p64(jiffies_64)
    4295353840

Also, the good old ``umachine`` module and its ``mem8/mem16/mem32`` objects are available.

Call into the kernel
^^^^^^^^^^^^^^^^^^^^

``Symbol`` objects can be called as variadic functions. ``int``, ``bool``, ``None``, ``str`` and
``bytes`` are automatically translated from Python to their native representation.

.. code-block:: python

    >>> printk("so.. %s %d %d %d\n", "hello", 123, None, True)
    18
    >>> # in dmesg: "so.. hello 123 0 1"

Messing with ``ifconfig``:

.. code-block:: python

    >>> rtnl_lock()
    0
    >>> rtnl_is_locked()
    1
    >>> # now try ifconfig
    >>> rtnl_unlock()
    0

Count network namespaces:

.. code-block:: python

    def count_net_ns():
        down_read(net_rwsem)

        first = p64(net_namespace_list)  # list_head.next
        n = 1

        cur = first
        while p64(cur) != first:  # list_head.next
            cur = p64(cur)
            n += 1

        up_read(net_rwsem)

        return n - 1  # -1 for the net_namespace_list itself

The value returned by the called function is given back as an integer. If the function has no
return value, it will be garbage and it should be ignored.

Modify memory
^^^^^^^^^^^^^

The ``p64/p32/p16/p8(p)`` functions can receive a 2nd argument in which case they write that
value instead of reading.

.. code-block:: python

    >>> p64(jiffies_64, 0)  # oh boy

Access structs
^^^^^^^^^^^^^^

You can use the ``uctypes`` module.

TODO automatic struct access (without manual declaration)

Python callbacks
^^^^^^^^^^^^^^^^

TODO

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

You'll get ``ports/linux-kernel/build/mpy.ko``, which can be loaded on the current machine with ``insmod``.

Build for a specific kernel version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can pass the ``KDIR`` variable to ``make`` to build against a specific kernel tree.

.. code-block:: bash

    $ make -C ports/linux-kernel KDIR=/path/to/some/other/kernel


Using it
--------

The exposed interface is a Python REPL. After being loaded, the module creates a server listening locally. The default
address the server binds on is ``0.0.0.0:9999``. If you want to change the default (in order to restrict access on a
specific network interface, or if you just want to change the port) you can pass ``server_addr=ip:port`` when
calling ``insmod``, for example ``insmod mpy.ko server_addr=127.0.0.1:1234``. For further access restriction you can
use ``iptables`` to filter access on the bound port.

The following ``socat`` command will start a full interactive shell:

.. code-block:: bash

    $ socat file:`tty`,raw,echo=0,escape=0xc tcp:<IP>:9999

You can exit with Ctrl+D which will also kill the shell (it will be restarted upon the next
connection).
If the shell hangs, you can exit with Ctrl+L.

When you want to load a larger piece of code, you can:

1. Paste it in the raw mode of the REPL (like IPython's ``%cpaste``). Hit Ctrl+E, paste your code and then Ctrl+D.
2. ``import`` from the filesystem works - you just have to configure ``sys.path`` accordingly, and you'll be able
   to import python files.

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

``kmalloc(n)`` - Calls ``kmalloc(n, GFP_KERNEL)`` and raises an exception on failed allocation.

``current()`` - Returns the ``current`` pointer (a ``task_struct``).

I recommend to import them this way for faster typing.

.. code-block:: python

    from kernel_ffi import str as s, bytes as b, p8, p16, p32, p64

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

Count network namespaces: (this shows direct pointers access; examples of proper struct accessing
will be given later)

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

I have written a supplementary project that provides a Pythonic way to access kernel structs.
You can find it `here <https://github.com/Jongy/struct_layout>`_.

To use it, you need to pass ``STRUCT_LAYOUT=1`` to ``make`` when building the module. It's optional
because it's an external dependency, yet it is highly recommended you use it ;)

To make a struct from a pointer, you can create "struct casting" functions:

.. code-block:: python

    # partial_struct is available globally
    net_device = partial_struct("net_device")
    task_struct = partial_struct("task_struct")

Now, working with it:

.. code-block:: python

    d = net_device(dev_get_by_name(init_net, "eth0"))

    # should match "ifconfig eth0"
    print(d.stats.rx_bytes)

    # you can write
    d.stats.rx_bytes = 1 << 63


.. code-block:: python

    t = task_struct(init_task)

    next = task_struct(container_of(int(t), "task_struct", "tasks"))
    next.comm[0] = ord('h')
    next.comm[1] = ord('i')
    next.comm[2] = 0

    # now find it in ps

    # arrays can also be written this way
    next.comm = "myawesomecomm\0"

    # it will guard you from overflows
    next.comm = "this is longer than TASK_COMM_LEN"
    # ValueError: Buffer overflow!

You can always ``int(..)`` any struct object to get its address.


You can also use the ``uctypes`` module.

Python callbacks
^^^^^^^^^^^^^^^^

``kernel_ffi.callback`` wraps a Python function and gives you a pointer
that can be called by native code.

Make ``/dev/null`` readable:

.. code-block:: python

    file_operations = partial_struct("file_operations")
    null_fops = file_operations(null_fops)

    from kernel_ffi import callback

    def my_read_null(file, buf, count, ppos):
        pos = p64(ppos)
        b = "who said /dev/null must be empty?\n"[pos:]
        l = min(len(b), count)
        # copy_to_user might be helpful here...
        memcpy(buf, b, l)
        p64(ppos, pos + l)
        return l

    c = callback(my_read_null)
    # null_fops requires CONFIG_KALLSYMS_ALL in your kernel
    null_fops.read = c.ptr()

    # now try "cat /dev/null"

    # to revert:
    null_fops.read = int(read_null)

Hook kernel code
^^^^^^^^^^^^^^^^

Two hooking mechanisms are available to Python: Kprobes and ftrace (if your kernel supports them,
of course).

Kprobes
~~~~~~~

Based on the kernel's "kprobe" mechanism, you can hook arbitrary kernel code
and run your Python code instead (or along).

``kernel_ffi.kprobe`` accepts 3 arguments:

* kprobe target - can be an address, a name or a Symbol object.
* kprobe type - will be explained below.
* handler function - Python function.

Kprobe type is any of:

* ``kernel_ffi.KP_ARGS_WATCH``

    Prototype: ``def my_probe(arg1, arg2, ...)``.

    Can be used when you kprobe onto functions. In this case, the function arguments will
    be passed to your Python callback in the same order (as many arguments as you accept
    in your callback).

    Return value is ignored and execution continues in the probed function.

* ``kernel_ffi.KP_ARGS_MODIFY``

    Prototype: ``def my_probe(call_ptr, arg1, arg2, ...)``.

    Like ``KP_ARGS_WATCH`` but the probed function is not called.

    You can use the ``call_ptr`` object (a ``Symbol``) to call the real probed function.
    TODO: ``call_ptr`` calls directly onto the probed function again, but I'm not positive
    yet that kprobes prevents the recursion.

    Return value is used instead of calling the probed function.

* ``kernel_ffi.KP_REGS_WATCH``

    Prototype: ``def my_probe(pt_regs)``.

    You get the ``pt_regs`` to inspect. Useful when not probing directly on a function
    (so "function arguments" don't mean much).

    Return value is ignored and execution continues in the probed function.

* ``kernel_ffi.KP_REGS_MODIFY``

    Prototype: ``def my_probe(pt_regs)``.

    Like ``KP_REGS_WATCH``, you get the ``struct pt_regs``, and this time any modifications
    you make to registers are applied (including modifications to the instruction
    pointer).

    Return value is ignored and execution continues as specified in the ``pt_regs``.

``WATCH`` kprobes might be eligble for kprobes optimization (see the
`kprobes docs <https://www.kernel.org/doc/Documentation/kprobes.txt>`_) so prefer to use
them when you don't need to modify anything.

If your probe handler raises an exception, it will be disabled for future calls and the
particular invocation will be handled as ``WATCH`` (that is, no modifications are applied).

Example 1: Printing all files opened on the system:

.. code-block:: python

    from kernel_ffi import kprobe, KP_ARGS_WATCH, str as s
    from struct_access import partial_struct

    filename = partial_struct("filename")

    def do_filp_open_hook(dfd, fn):  # don't have to receive all args if you don't need
        print("do_filp_open: fd {} name {!r}".format(dfd, s(int(filename(fn).name))))

    kp = kprobe("do_filp_open", KP_ARGS_WATCH, do_filp_open_hook)

    # when you're done:
    kp.rm()
    # if kp goes out of scope, the gc finalizer will also remove it.

Example 2: TODO example with regs

ftrace
~~~~~~

ftrace allows for convenient and efficient function hooking. What you get is practically equivalent
to kprobes's ``KP_ARGS_MODIFY`` - you get called instead of the function, and you can call the "original"
if you wish.

.. note:: Technically speaking, on new kernels - kprobes placed on functions are optimized to be based on
          ftrace. The kprobes ``ARGS`` probe types are kept around, so they can be used in kernels w/o ftrace.

Same rules apply as discussed in the kprobes section (return values, exceptions etc).

Hides all processes with an even pid (yeah, it's a stupid trick that can be bypassed in many ways,
but it shows the point and it works on BusyBox ps :)

.. code-block:: python

    filename = partial_struct("filename")
    from kernel_ffi import str as s, ftrace
    from uerrno import ENOENT


    def ERR_PTR(err):
        return (1 << 64) - err


    def _do_filp_open(orig, dfd, pathname, op):
        fn = s(int(filename(pathname).name))

        if fn.startswith("/proc/"):
            rest = fn.lstrip("/proc/")

            if '/' in rest:
                rest = rest[:rest.find('/')]

            try:
                pid = int(rest)
                if pid % 2 == 0:
                    return ERR_PTR(ENOENT)
            except ValueError:
                # not a process directory
                pass

        return orig(dfd, pathname, op)


    ft = ftrace("do_filp_open", _do_filp_open)

    # ...

    ft.rm()

SMP and Multithreading
^^^^^^^^^^^^^^^^^^^^^^

By default, this port compiles with ``MICROPY_PY_THREAD`` which enables multithreading.
Multithreading is also required to get the hooks and callbacks to behave properly.

Furthermore, it compiles without ``MICROPY_PY_THREAD_GIL``, to allow for real concurrency & SMP.
This means you have to protect globals with synchronization primitives as will be shown later.

Without threading enabled, MicroPython manages a single exception stack, so it's impossible
to run code that uses this stack concurrently (if pushes and pops to the exception stack don't
happen in their exact reversed order, threads might incorrectly swap contexts).
Also, shared core resources (the heap, the qstr pool) are not protected from concurrent access.

With threading enabled, we:
1. Keep a separate exception stack for each thread running Python (be it a thread created by Python, or
a thread running a hook / callback).
2. Protect core resources.
3. Traverse all threads' stacks on each GC collect operation.

.. note:: Being completely free of data races for pieces of Python code running in kernel hook points
          is hard. I've put this port through some stress testings on SMP systems, but multithreading is still
          the Achilles heel of it.

          Don't push it too hard if you don't have to ;)

Starting Python threads
~~~~~~~~~~~~~~~~~~~~~~~

Use MicroPython's ``_thread`` module.

.. code-block:: python

    from _thread import start_new_thread

    def my_thread(arg):
        print("i'm up!!")
        print("i'll sleep for {}ms now".format(arg))
        msleep(arg)
        print("i'm out!!")

    start_new_thread(my_thread, (1500, ))

Synchronization primitives
~~~~~~~~~~~~~~~~~~~~~~~~~~

``_thread.lock`` are backed up by spinlocks.

.. code-block:: python

    from _thread import allocate_lock

    my_lock = allocate_lock()

    with lock:
        # do stuff
        printk("i got this\n")

Since these are spinlock-based, you shouldn't use them in the REPL, which is a normal thread performing
socket I/O. It might seem to work on an SMP system, but on a uniprocessor system this will certainly deadlock,
since spinlocks are meant to be used in only in atomic contexts.

If you need more saner primitives, you can use the kernel's semaphores as a mutex. Or you can write anything
else you need, based on kernel primitives, since you have access to everything.

.. code-block:: python

    from kernel_ffi import kmalloc
    from struct_access import sizeof
    from _thread import start_new_thread

    semaphore_s = partial_struct("semaphore")

    def new_mutex():
        x = semaphore_s(kmalloc(sizeof("semaphore")))

        # gotta do what you gotta do
        # if CONFIG_DEBUG_LOCK_ALLOC / CONFIG_DEBUG_SPINLOCK are enabled, more work has to
        # be done.
        x.count = 1
        x.lock.raw_lock.val.counter = 0
        x.wait_list.next = int(x.wait_list)
        x.wait_list.prev = int(x.wait_list)

        return int(x)


    def wait_and_print(x):
        print("calling down()...")
        down(x)
        print("got it!")
        up(x)

    x = new_mutex()
    down(x)

    start_new_thread(wait_and_print, (x, ))

    # ....
    up(x)

    # now you'll see the prints

Future TODOs
------------

* Python in interrupt contexts. This already works, somewhat, but there are still some known possible races that
  can e.g lead to a deadlock on a UP system.
* Optimize the threads list - use another mean of TLS (like stack-based) and perhaps something better
  than a linked list, like an array with atomic indexing.
* Type checking for functions and globals - parse function declarations and such from the kernel headers
  and encode this information in the Python, providing function type checking, perhaps auto-suggestion
  for arguments...

  * This will also allow to use descriptors for globals accessing - instead of ``p64(some_global, 0)`` you could
    just do ``some_global = 0`` and it'd figure the ``p64`` out.

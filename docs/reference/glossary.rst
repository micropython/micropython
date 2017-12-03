Glossary
========

.. glossary::

    baremetal
        A system without (full-fledged) OS, like an :term:`MCU`. When
        running on a baremetal system, MicroPython effectively becomes
        its user-facing OS with a command interpreter (REPL).

    board
        A PCB board. Oftentimes, the term is used to denote a particular
        model of an :term:`MCU` system. Sometimes, it is used to actually
        refer to :term:`MicroPython port` to a particular board (and then
        may also refer to "boardless" ports like
        :term:`Unix port <MicroPython Unix port>`).

    callee-owned tuple
        A tuple returned by some builtin function/method, containing data
        which is valid for a limited time, usually until next call to the
        same function (or a group of related functions). After next call,
        data in the tuple may be changed. This leads to the following
        restriction on the usage of callee-owned tuples - references to
        them cannot be stored. The only valid operation is extracting
        values from them (including making a copy). Callee-owned tuples
        is a MicroPython-specific construct (not available in the general
        Python language), introduced for memory allocation optimization.
        The idea is that callee-owned tuple is allocated once and stored
        on the callee side. Subsequent calls don't require allocation,
        allowing to return multiple values when allocation is not possible
        (e.g. in interrupt context) or not desirable (because allocation
        inherently leads to memory fragmentation). Note that callee-owned
        tuples are effectively mutable tuples, making an exception to
        Python's rule that tuples are immutable. (It may be interesting
        why tuples were used for such a purpose then, instead of mutable
        lists - the reason for that is that lists are mutable from user
        application side too, so a user could do things to a callee-owned
        list which the callee doesn't expect and could lead to problems;
        a tuple is protected from this.)

    CPython
        CPython is the reference implementation of Python programming
        language, and the most well-known one, which most of the people
        run. It is however one of many implementations (among which
        Jython, IronPython, PyPy, and many more, including MicroPython).
        As there is no formal specification of the Python language, only
        CPython documentation, it is not always easy to draw a line
        between Python the language and CPython its particular
        implementation. This however leaves more freedom for other
        implementations. For example, MicroPython does a lot of things
        differently than CPython, while still aspiring to be a Python
        language implementation.

    GPIO
        General-purpose input/output. The simplest means to control
        electrical signals. With GPIO, user can configure hardware
        signal pin to be either input or output, and set or get
        its digital signal value (logical "0" or "1"). MicroPython
        abstracts GPIO access using :class:`machine.Pin` and :class:`machine.Signal`
        classes.

    GPIO port
        A group of :term:`GPIO` pins, usually based on hardware
        properties of these pins (e.g. controllable by the same
        register).

    MCU
        Microcontroller. Microcontrollers usually have much less resources
        than a full-fledged computing system, but smaller, cheaper and
        require much less power. MicroPython is designed to be small and
        optimized enough to run on an average modern microcontroller.

    micropython-lib
        MicroPython is (usually) distributed as a single executable/binary
        file with just few builtin modules. There is no extensive standard
        library comparable with :term:`CPython`. Instead, there is a related, but
        separate project
        `micropython-lib <https://github.com/micropython/micropython-lib>`_
        which provides implementations for many modules from CPython's
        standard library. However, large subset of these modules require
        POSIX-like environment (Linux, MacOS, Windows may be partially
        supported), and thus would work or make sense only with
        `MicroPython Unix port`. Some subset of modules is however usable
        for `baremetal` ports too.

        Unlike monolithic :term:`CPython` stdlib, micropython-lib modules
        are intended to be installed individually - either using manual
        copying or using :term:`upip`.

    MicroPython port
        MicroPython supports different :term:`boards <board>`, RTOSes,
        and OSes, and can be relatively easily adapted to new systems.
        MicroPython with support for a particular system is called a
        "port" to that system. Different ports may have widely different
        functionality. This documentation is intended to be a reference
        of the generic APIs available across different ports ("MicroPython
        core"). Note that some ports may still omit some APIs described
        here (e.g. due to resource constraints). Any such differences,
        and port-specific extensions beyond MicroPython core functionality,
        would be described in the separate port-specific documentation.

    MicroPython Unix port
        Unix port is one of the major :term:`MicroPython ports <MicroPython port>`.
        It is intended to run on POSIX-compatible operating systems, like
        Linux, MacOS, FreeBSD, Solaris, etc. It also serves as the basis
        of Windows port. The importance of Unix port lies in the fact
        that while there are many different :term:`boards <board>`, so
        two random users unlikely have the same board, almost all modern
        OSes have some level of POSIX compatibility, so Unix port serves
        as a kind of "common ground" to which any user can have access.
        So, Unix port is used for initial prototyping, different kinds
        of testing, development of machine-independent features, etc.
        All users of MicroPython, even those which are interested only
        in running MicroPython on :term:`MCU` systems, are recommended
        to be familiar with Unix (or Windows) port, as it is important
        productivity helper and a part of normal MicroPython workflow.

    port
        Either :term:`MicroPython port` or :term:`GPIO port`. If not clear
        from context, it's recommended to use full specification like one
        of the above.

    stream
        Also known as a "file-like object". An object which provides sequential
        read-write access to the underlying data. A stream object implements
        a corresponding interface, which consists of methods like ``read()``,
        ``write()``, ``readinto()``, ``seek()``, ``flush()``, ``close()``, etc.
        A stream is an important concept in MicroPython, many I/O objects
        implement the stream interface, and thus can be used consistently and
        interchangeably in different contexts. For more information on
        streams in MicroPython, see `uio` module.

    upip
        (Literally, "micro pip"). A package manage for MicroPython, inspired
        by :term:`CPython`'s pip, but much smaller and with reduced functionality.
        upip runs both on :term:`Unix port <MicroPython Unix port>` and on
        :term:`baremetal` ports (those which offer filesystem and networking
        support).

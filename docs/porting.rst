We love CircuitPython and would love to see it come to more microcontroller
platforms. With 3.0 we've reworked CircuitPython to make it easier than ever to
add support. While there are some major differences between ports, this page
covers the similarities that make CircuitPython what it is and how that core
fits into a variety of microcontrollers.

Architecture
============

There are three core pieces to CircuitPython:

The first is the Python VM that the awesome MicroPython devs have created.
These VMs are written to be portable so there is not much needed when moving to
a different microcontroller, especially if it is ARM based.

The second is the infrastructure around those VMs which provides super basic
operating system functionality such as initializing hardware, running USB,
prepping file systems and automatically running user code on boot. In
CircuitPython we've dubbed this component the supervisor because it monitors
and facilitates the VMs which run user Python code. Porting involves the
supervisor because many of the tasks it does while interfacing with the
hardware. Once its going though, the REPL works and debugging can migrate to a
Python based approach rather than C.

The third core piece is the plethora of low level APIs that CircuitPython
provides as the foundation for higher level libraries including device drivers.
These APIs are called from within the running VMs through the Python interfaces
defined in ``shared-bindings``. These bindings rely on the underlying
``common_hal`` C API to implement the functionality needed for the Python API.
By splitting the two, we work to ensure standard functionality across which
means that libraries and examples apply across ports with minimal changes.

Porting
=======

Step 1: Getting building
------------------------
The first step to porting to a new microcontroller is getting a build running.
The primary goal of it should be to get ``main.c`` compiling with the assistance
of the ``supervisor/supervisor.mk`` file. Port specific code should be isolated
to the port's directory (in the top level until the ``ports`` directory is
present). This includes the Makefile and any C library resources. Make sure
these resources are compatible with the MIT License of the rest of the code!

Step 2: Init
--------------
Once your build is setup, the next step should be to get your clocks going as
you expect from the supervisor. The supervisor calls ``port_init`` to allow for
initialization at the beginning of main. This function also has the ability to
request a safe mode state which prevents the supervisor from running user code
while still allowing access to the REPL and other resources.

The core port initialization and reset methods are defined in
``supervisor/port.c`` and should be the first to be implemented. Its required
that they be implemented in the ``supervisor`` directory within the port
directory. That way, they are always in the expected place.

The supervisor also uses three linker variables, ``_ezero``, ``_estack`` and
``_ebss`` to determine memory layout for stack overflow checking.

Step 3: REPL
------------
Getting the REPL going is a huge step. It involves a bunch of initialization to
be done correctly and is a good sign you are well on your porting way. To get
the REPL going you must implement the functions and definitions from
``supervisor/serial.h`` with a corresponding ``supervisor/serial.c`` in the port
directory. This involves sending and receiving characters over some sort of
serial connection. It could be UART or USB for example.

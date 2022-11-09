.. _mpremote:

MicroPython remote control: mpremote
====================================

The ``mpremote`` command line tool provides an integrated set of utilities to
remotely interact with and automate a MicroPython device over a serial
connection.

To use mpremote install it via ``pip``:

.. code-block:: bash

    $ pip install mpremote

The simplest way to use this tool is just by invoking it without any arguments:

.. code-block:: bash

    mpremote

This command automatically detects and connects to the first available serial
device and provides an interactive REPL.  Serial ports are opened in exclusive
mode, so running a second (or third, etc) instance of ``mpremote`` will connect
to subsequent serial devices, if any are available.


Commands
--------

For REPL access, running ``mpremote`` without any arguments is usually all that
is needed.  ``mpremote`` also supports a set of commands given at the command
line which will perform various actions on remote MicroPython devices.

For commands that support multiple arguments (e.g. a list of files), the
argument list can be terminated with ``+``.

The full list of supported commands are:

- connect to a specified device via a device-name shortcut:

  .. code-block:: bash

      $ mpremote <device-shortcut>

- connect to specified device via name:

  .. code-block:: bash

      $ mpremote connect <device>

  ``<device>`` may be one of:

  - ``list``: list available devices
  - ``auto``: connect to the first available device
  - ``id:<serial>``: connect to the device with USB serial number
    ``<serial>`` (the second entry in the output from the ``connect list``
    command)
  - ``port:<path>``: connect to the device with the given path
  - any valid device name/path, to connect to that device

- disconnect current device:

  .. code-block:: bash

      $ mpremote disconnect

  After a disconnect, auto soft-reset is enabled.

- resume a previous ``mpremote`` session:

  .. code-block:: bash

      $ mpremote resume

  This disables auto soft-reset.

- perform a soft-reset of the device:

  .. code-block:: bash

      $ mpremote soft-reset

  This will clear out the Python heap and restart the interpreter.  It also
  disables auto soft-reset.

- enter the REPL on the connected device:

   .. code-block:: bash

      $ mpremote repl [options]

  Options are:

  - ``--capture <file>``, to capture output of the REPL session to the given
    file
  - ``--inject-code <string>``, to specify characters to inject at the REPL when
    Ctrl-J is pressed
  - ``--inject-file <file>``, to specify a file to inject at the REPL when
    Ctrl-K is pressed

- evaluate and print the result of a Python expression on the connected device:

  .. code-block:: bash

      $ mpremote eval [-e] <string>

- execute the given Python code on the connected device:

  .. code-block:: bash

      $ mpremote exec [-e] <string>

  If the -e option is used, the string can contain one or more expressions enclosed in ``{{ }}``. These {{expressions}} are evaluated on the 
  host, and replaced by the result(s) before sending the command to the device. 
  This is used by the setrtc command to set the RTC from the host system time, but can also be used for other purposes.

  The eval is executed with a limited scope, so that the following variables are available:
    - os, sys, time, datetime
    - device_name, the name  of the serial port used to connect to the device 
    - mount_dir, the local directory mounted to the device (if any)

  .. code-block:: bash

      $ mpremote mount ./data  exec -e "import os; print(f'port {{device_name}} connects to {os.uname()[1:3]}, mounted directory: {{host_dir}}')"
      # prints the host port and the hosts's mounted directory 
      # import os; print(f'port COM6 connects to {os.uname()}.\nHost directory: ./data')
      # port COM6 connects to ('pyboard', '1.19.1'), mounted directory: ./data

      $ mpremote exec -e "import time,os; print(os.uname()[1:3],'\n', time.localtime(),'\n', {{time.localtime()[0:8] }})"
      # compare local time with RTC time

  .. code-block:: bash

      $ mpremote eval -e "f'port {{device_name}} is connected'"      

- set the real time clock (machine.RTC) to the hosts's current time:

  .. code-block:: bash

      $ mpremote setrtc

- run a script from the local filesystem:

  .. code-block:: bash

      $ mpremote run <file>

- execute filesystem commands on the device:

  .. code-block:: bash

      $ mpremote fs <command>

  ``<command>`` may be:

  - ``cat <file..>`` to show the contents of a file or files on the device
  - ``ls`` to list the current directory
  - ``ls <dirs...>`` to list the given directories
  - ``cp [-r] <src...> <dest>`` to copy files; use ":" as a prefix to specify
    a file on the device
  - ``rm <src...>`` to remove files on the device
  - ``mkdir <dirs...>`` to create directories on the device
  - ``rmdir <dirs...>`` to remove directories on the device
  - ``touch <file..>`` to create the files (if they don't already exist)

- edit a file on the device:

  .. code-block:: bash

      $ mpremote edit <files...>

  The ``edit`` command will copy each file from the device to a local temporary
  directory and then launch your editor for each file (defined by the environment
  variable ``$EDITOR``). If the editor exits successfully, the updated file will
  be copied back to the device.

- install packages from :term:`micropython-lib` (or GitHub) using the ``mip`` tool:

  .. code-block:: bash

      $ mpremote mip install <packages...>

  See :ref:`packages` for more information.

- mount the local directory on the remote device:

  .. code-block:: bash

      $ mpremote mount [options] <local-dir>

  During usage, Ctrl-D will soft-reboot and normally reconnect the mount automatically.
  If the unit has a main.py running at startup however the remount cannot occur.
  In this case a raw mode soft reboot can be used: Ctrl-A Ctrl-D to reboot,
  then Ctrl-B to get back to normal repl at which point the mount will be ready.

  Options are:

  - ``-l``, ``--unsafe-links``: By default an error will be raised if the device
    accesses a file or directory which is outside (up one or more directory levels) the
    local directory that is mounted.  This option disables this check for symbolic
    links, allowing the device to follow symbolic links outside of the local directory.

- unmount the local directory from the remote device:

  .. code-block:: bash

      $ mpremote umount

Multiple commands can be specified and they will be run sequentially.


Auto connection and soft-reset
------------------------------

Connection and disconnection will be done automatically at the start and end of
the execution of the tool, if such commands are not explicitly given.  Automatic
connection will search for the first available serial device. If no action is
specified then the REPL will be entered.

Once connected to a device, ``mpremote`` will automatically soft-reset the
device if needed.  This clears the Python heap and restarts the interpreter,
making sure that subsequent Python code executes in a fresh environment.  Auto
soft-reset is performed the first time one of the following commands are
executed: ``mount``, ``eval``, ``exec``, ``run``, ``fs``.  After doing a
soft-reset for the first time, it will not be done again automatically, until a
``disconnect`` command is issued.

Auto soft-reset behaviour can be controlled by the ``resume`` command.  And the
``soft-reset`` command can be used to perform an explicit soft reset.


Shortcuts
---------

Shortcuts can be defined using the macro system.  Built-in shortcuts are::

- ``devs``: list available devices (shortcut for ``connect list``)

- ``a0``, ``a1``, ``a2``, ``a3``: connect to /dev/ttyACM?

- ``u0``, ``u1``, ``u2``, ``u3``: connect to /dev/ttyUSB?

- ``c0``, ``c1``, ``c2``, ``c3``: connect to COM?

- ``cat``, ``ls``, ``cp``, ``rm``, ``mkdir``, ``rmdir``, ``touch``, ``df``:
  filesystem commands

- ``reset``: reset the device

- ``bootloader``: make the device enter its bootloader

Any user configuration, including user-defined shortcuts, can be placed in the file
``.config/mpremote/config.py``. For example:

.. code-block:: python3

    commands = {
        "c33": "connect id:334D335C3138",
        "bl": "bootloader",
        "double x=4": "eval x*2",  # x is an argument, with default 4
        "wl_scan": ["exec", """
    import network
    wl = network.WLAN()
    wl.active(1)
    for ap in wl.scan():
        print(ap)
    """,],
        "test": ["mount", ".", "exec", "import test"],
    }


Examples
--------

.. code-block:: bash

  mpremote

  mpremote a1

  mpremote connect /dev/ttyUSB0 repl

  mpremote ls

  mpremote a1 ls

  mpremote exec "import micropython; micropython.mem_info()"

  mpremote eval 1/2 eval 3/4

  mpremote mount .

  mpremote mount . exec "import local_script"

  mpremote ls

  mpremote cat boot.py

  mpremote cp :main.py .

  mpremote cp main.py :

  mpremote cp :a.py :b.py

  mpremote cp -r dir/ :

  mpremote cp a.py b.py : + repl

  mpremote mip install aioble

  mpremote mip install github:org/repo@branch

  mpremote mip install --target /flash/third-party functools

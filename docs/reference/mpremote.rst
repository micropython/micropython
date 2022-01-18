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

- evaluate and print the result of a Python expression:

  .. code-block:: bash

      $ mpremote eval <string>

- execute the given Python code:

  .. code-block:: bash

      $ mpremote exec <string>

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

- mount the local directory on the remote device:

  .. code-block:: bash

      $ mpremote mount <local-dir>

Multiple commands can be specified and they will be run sequentially.
Connection and disconnection will be done automatically at the start and end of
the execution of the tool, if such commands are not explicitly given.  Automatic
connection will search for the first available serial device. If no action is
specified then the REPL will be entered.

Shortcuts
---------

Shortcuts can be defined using the macro system.  Built-in shortcuts are::

- ``devs``: list available devices (shortcut for ``connect list``)

- ``a0``, ``a1``, ``a2``, ``a3``: connect to /dev/ttyACM?

- ``u0``, ``u1``, ``u2``, ``u3``: connect to /dev/ttyUSB?

- ``c0``, ``c1``, ``c2``, ``c3``: connect to COM?

- ``cat``, ``ls``, ``cp``, ``rm``, ``mkdir``, ``rmdir``, ``df``: filesystem
  commands

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

  mpremote cp -r dir/ :

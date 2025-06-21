.. _mpremote:

MicroPython remote control: mpremote
====================================

The ``mpremote`` command line tool provides an integrated set of utilities to
remotely interact with, manage the filesystem on, and automate a MicroPython
device over a serial connection.

To use mpremote, first install it via ``pip``:

.. code-block:: bash

    $ pip install --user mpremote

Or via `pipx <https://pypa.github.io/pipx/>`_:

.. code-block:: bash

    $ pipx install mpremote

The simplest way to use this tool is just by invoking it without any arguments:

.. code-block:: bash

    $ mpremote

This command automatically detects and connects to the first available USB
serial device and provides an interactive terminal that you can use to access
the REPL and your program's output.  Serial ports are opened in exclusive mode,
so running a second (or third, etc) instance of ``mpremote`` will connect to
subsequent serial devices, if any are available.

Additionally ``pipx`` also allows you to directly run ``mpremote`` without
installing first:

.. code-block:: bash

    $ pipx run mpremote ...args

Commands
--------

``mpremote`` supports being given a series of commands given at the command line
which will perform various actions in sequence on a remote MicroPython device.
See the :ref:`examples section <mpremote_examples>` below to get an idea of how
this works and for some common combinations of commands.

Each command is of the form ``<command name> [--options] [args...]``. For commands
that support multiple arguments (e.g. a list of files), the argument list can
be terminated with ``+``.

If no command is specified, the default command is ``repl``. Additionally, if
any command needs to access the device, and no earlier ``connect`` has been
specified, then an implicit ``connect auto`` is added.

In order to get the device into a known state for any action command
(except ``repl``), once connected ``mpremote`` will stop any running program
and soft-reset the device before running the first command. You can control
this behavior using the ``resume`` and ``soft-reset`` commands.
See :ref:`auto-connection and auto-soft-reset <mpremote_reset>` for more details.

Multiple commands can be specified and they will be run sequentially.

The full list of supported commands are:

- `connect <mpremote_command_connect>`
- `disconnect <mpremote_command_disconnect>`
- `resume <mpremote_command_resume>`
- `soft_reset <mpremote_command_soft_reset>`
- `repl <mpremote_command_repl>`
- `eval <mpremote_command_eval>`
- `exec <mpremote_command_exec>`
- `run <mpremote_command_run>`
- `fs <mpremote_command_fs>`
- `df <mpremote_command_df>`
- `edit <mpremote_command_edit>`
- `mip <mpremote_command_mip>`
- `mount <mpremote_command_mount>`
- `unmount <mpremote_command_unmount>`
- `romfs <mpremote_command_romfs>`
- `rtc <mpremote_command_rtc>`
- `sleep <mpremote_command_sleep>`
- `reset <mpremote_command_reset>`
- `bootloader <mpremote_command_bootloader>`

.. _mpremote_command_connect:

- **connect** -- connect to specified device via name:

  .. code-block:: bash

      $ mpremote connect <device>

  ``<device>`` may be one of:

  - ``list``: list available devices
  - ``auto``: connect to the first available USB serial port
  - ``id:<serial>``: connect to the device with USB serial number
    ``<serial>`` (the second column from the ``connect list``
    command output)
  - ``port:<path>``: connect to the device with the given path (the first column
    from the ``connect list`` command output
  - ``rfc2217://<host>:<port>``: connect to the device using serial over TCP
    (e.g. a networked serial port based on RFC2217)
  - any valid device name/path, to connect to that device

  **Note:** Instead of using the ``connect`` command, there are several
  :ref:`pre-defined shortcuts <mpremote_shortcuts>` for common device paths. For
  example the ``a0`` shortcut command is equivalent to
  ``connect /dev/ttyACM0`` (Linux), or ``c0`` for ``COM0`` (Windows).

  **Note:** The ``auto`` option will only detect USB serial ports, i.e. a serial
  port that has an associated USB VID/PID (i.e. CDC/ACM or FTDI-style
  devices). Other types of serial ports will not be auto-detected.

.. _mpremote_command_disconnect:

- **disconnect** -- disconnect current device:

  .. code-block:: bash

      $ mpremote disconnect

  After a disconnect, :ref:`auto-soft-reset <mpremote_reset>` is enabled.

.. _mpremote_command_resume:

- **resume** -- maintain existing interpreter state for subsequent commands:

  .. code-block:: bash

      $ mpremote resume

  This disables :ref:`auto-soft-reset <mpremote_reset>`. This is useful if you
  want to run a subsequent command on a board without first soft-resetting it.

.. _mpremote_command_soft_reset:

- **soft-reset** -- perform a soft-reset of the device:

  .. code-block:: bash

      $ mpremote soft-reset

  This will clear out the Python heap and restart the interpreter.  It also
  prevents the subsequent command from triggering :ref:`auto-soft-reset <mpremote_reset>`.

.. _mpremote_command_repl:

- **repl** -- enter the REPL on the connected device:

   .. code-block:: bash

      $ mpremote repl [--options]

  Options are:

  - ``--escape-non-printable``, to print non-printable bytes/characters as their hex code
  - ``--capture <file>``, to capture output of the REPL session to the given
    file
  - ``--inject-code <string>``, to specify characters to inject at the REPL when
    ``Ctrl-J`` is pressed. This allows you to automate a common command.
  - ``--inject-file <file>``, to specify a file to inject at the REPL when
    ``Ctrl-K`` is pressed. This allows you to run a file (e.g. containing some
    useful setup code, or even the program you are currently working on).

  While the ``repl`` command running, you can use ``Ctrl-]`` or ``Ctrl-x`` to
  exit.

  **Note:** The name "REPL" here reflects that the common usage of this command
  to access the Read Eval Print Loop that is running on the MicroPython
  device. Strictly, the ``repl`` command is just functioning as a terminal
  (or "serial monitor") to access the device. Because this command does not
  trigger the :ref:`auto-reset behavior <mpremote_reset>`, this means that if
  a program is currently running, you will first need to interrupt it with
  ``Ctrl-C`` to get to the REPL, which will then allow you to access program
  state. You can also use ``mpremote soft-reset repl`` to get a "clean" REPL
  with all program state cleared.

.. _mpremote_command_eval:

- **eval** -- evaluate and print the result of a Python expression:

  .. code-block:: bash

      $ mpremote eval <string>

.. _mpremote_command_exec:

- **exec** -- execute the given Python code:

  .. code-block:: bash

      $ mpremote exec <string>

  By default, ``mpremote exec`` will display any output from the expression until it
  terminates. The ``--no-follow`` flag can be specified to return immediately and leave
  the device running the expression in the background.

.. _mpremote_command_run:

- **run** -- run a script from the local filesystem:

  .. code-block:: bash

      $ mpremote run <file.py>

  This will execute the file directly from RAM on the device without copying it
  to the filesystem. This is a very useful way to iterate on the development of
  a single piece of code without having to worry about deploying it to the
  filesystem.

  By default, ``mpremote run`` will display any output from the script until it
  terminates. The ``--no-follow`` flag can be specified to return immediately and leave
  the device running the script in the background.

.. _mpremote_command_fs:

- **fs** -- execute filesystem commands on the device:

  .. code-block:: bash

      $ mpremote fs <sub-command>

  ``<sub-command>`` may be:

  - ``cat <file..>`` to show the contents of a file or files on the device
  - ``ls`` to list the current directory
  - ``ls <dirs...>`` to list the given directories
  - ``cp [-rf] <src...> <dest>`` to copy files
  - ``rm [-r] <src...>`` to remove files or folders on the device
  - ``mkdir <dirs...>`` to create directories on the device
  - ``rmdir <dirs...>`` to remove directories on the device
  - ``touch <file..>`` to create the files (if they don't already exist)
  - ``sha256sum <file..>`` to calculate the SHA256 sum of files
  - ``tree [-vsh] <dirs...>`` to print a tree of the given directories

  The ``cp`` command uses a convention where a leading ``:`` represents a remote
  path. Without a leading ``:`` means a local path. This is based on the
  convention used by the `Secure Copy Protocol (scp) client
  <https://en.wikipedia.org/wiki/Secure_copy_protocol>`_.

  So for example, ``mpremote fs cp main.py :main.py`` copies ``main.py`` from
  the current local directory to the remote filesystem, whereas
  ``mpremote fs cp :main.py main.py`` copies ``main.py`` from the device back
  to the current directory.

  The ``mpremote rm -r`` command accepts both relative and absolute paths.
  Use ``:`` to refer to the current remote working directory (cwd) to allow a
  directory tree to be removed from the device's default path (eg ``/flash``, ``/``).
  Use ``-v/--verbose`` to see the files being removed.

  For example:

  - ``mpremote rm -r :libs`` will remove the ``libs`` directory and all its
    child items from the device.
  - ``mpremote rm -rv :/sd`` will remove all files from a mounted SDCard and result
    in a non-blocking warning. The mount will be retained.
  - ``mpremote rm -rv :/`` will remove all files on the device, including any
    located in mounted vfs such as ``/sd`` or ``/flash``. After removing all folders
    and files, this will  also return an error to mimic unix ``rm -rf /`` behaviour.

  .. warning::
    There is no supported way to undelete files removed by ``mpremote rm -r :``.
    Please use with caution.

  The ``tree`` command will print a tree of the given directories.
  Using the ``--size/-s`` option will print the size of each file, or use
  ``--human/-h`` to use a more human readable format.
  Note: Directory size is only printed when a non-zero size is reported by the device's filesystem.
  The ``-v`` option  can be used to include the name of the serial device in
  the output.

  All other commands implicitly assume the path is a remote path, but the ``:``
  can be optionally used for clarity.

  All of the filesystem sub-commands take multiple path arguments, so if there
  is another command in the sequence, you must use ``+`` to terminate the
  arguments, e.g.

  .. code-block:: bash

      $ mpremote fs cp main.py :main.py + repl

  This will copy the file to the device then enter the REPL. The ``+`` prevents
  ``"repl"`` being interpreted as a path.

  The ``cp`` command supports the ``-r`` option to make a recursive copy.  By
  default ``cp`` will skip copying files to the remote device if the SHA256 hash
  of the source and destination file matches.  To force a copy regardless of the
  hash use the ``-f`` option.

  **Note:** For convenience, all of the filesystem sub-commands are also
  :ref:`aliased as regular commands <mpremote_shortcuts>`, i.e. you can write
  ``mpremote cp ...`` instead of ``mpremote fs cp ...``.

.. _mpremote_command_df:

- **df** -- query device free/used space

  .. code-block:: bash

      $ mpremote df

  The ``df`` command will print size/used/free statistics for the device
  filesystem, similar to the Unix ``df`` command.

.. _mpremote_command_edit:

- **edit** -- edit a file on the device:

  .. code-block:: bash

      $ mpremote edit <files...>

  The ``edit`` command will copy each file from the device to a local temporary
  directory and then launch your editor for each file (defined by the environment
  variable ``$EDITOR``). If the editor exits successfully, the updated file will
  be copied back to the device.

.. _mpremote_command_mip:

- **mip** -- install packages from :term:`micropython-lib` (or GitHub) using the ``mip`` tool:

  .. code-block:: bash

      $ mpremote mip install <packages...>

  See :ref:`packages` for more information.

.. _mpremote_command_mount:

- **mount** -- mount the local directory on the remote device:

  .. code-block:: bash

      $ mpremote mount [options] <local-dir>

  This allows the remote device to see the local host directory as if it were
  its own filesystem. This is useful for development, and avoids the need to
  copy files to the device while you are working on them.

  The device installs a filesystem driver, which is then mounted in the
  :ref:`device VFS <filesystem>` as ``/remote``, which uses the serial
  connection to ``mpremote`` as a side-channel to access files. The device
  will have its current working directory (via ``os.chdir``) set to
  ``/remote`` so that imports and file access will occur there instead of the
  default filesystem path while the mount is active.

  **Note:** If the ``mount`` command is not followed by another action in the
  sequence, a ``repl`` command will be implicitly added to the end of the
  sequence.

  During usage, Ctrl-D will trigger a soft-reset as normal, but the mount will
  automatically be re-connected. If the unit has a main.py running at startup
  however the remount cannot occur. In this case a raw mode soft reboot can be
  used: Ctrl-A Ctrl-D to reboot, then Ctrl-B to get back to normal repl at
  which point the mount will be ready.

  Options are:

  - ``-l``, ``--unsafe-links``: By default an error will be raised if the device
    accesses a file or directory which is outside (up one or more directory levels) the
    local directory that is mounted.  This option disables this check for symbolic
    links, allowing the device to follow symbolic links outside of the local directory.

.. _mpremote_command_unmount:

- **unmount** -- unmount the local directory from the remote device:

  .. code-block:: bash

      $ mpremote umount

  This happens automatically when ``mpremote`` terminates, but it can be used
  in a sequence to unmount an earlier mount before subsequent command are run.

.. _mpremote_command_romfs:

- **romfs** -- manage ROMFS partitions on the device:

  .. code-block:: bash

      $ mpremote romfs <sub-command>

  ``<sub-command>`` may be:

  - ``romfs query`` to list all the available ROMFS partitions and their size
  - ``romfs [-o <output>] build <source>`` to create a ROMFS image from the given
    source directory; the default output file is the source appended by ``.romfs``
  - ``romfs [-p <partition>] deploy <source>`` to deploy a ROMFS image to the device;
    will also create a temporary ROMFS image if the source is a directory

  The ``build`` and ``deploy`` sub-commands both support the ``-m``/``--mpy`` option
  to automatically compile ``.py`` files to ``.mpy`` when creating the ROMFS image.
  This option is enabled by default, but only works if the ``mpy_cross`` Python
  package has been installed (eg via ``pip install mpy_cross``).  If the package is
  not installed then a warning is printed and ``.py`` files remain as is.  Compiling
  of ``.py`` files can be disabled with the ``--no-mpy`` option.

.. _mpremote_command_rtc:

- **rtc** -- set/get the device clock (RTC):

  .. code-block:: bash

      $ mpremote rtc

  This will query the device RTC for the current time and print it as a datetime
  tuple.

  .. code-block:: bash

      $ mpremote rtc --set

  This will set the device RTC to the host PC's current time.

.. _mpremote_command_sleep:

- **sleep** -- sleep (delay) before executing the next command

  .. code-block:: bash

      $ mpremote sleep 0.5

  This will pause execution of the command sequence for the specified duration
  in seconds, e.g. to wait for the device to do something.

.. _mpremote_command_reset:

- **reset** -- hard reset the device

  .. code-block:: bash

      $ mpremote reset

  **Note:** hard reset is equivalent to :func:`machine.reset`.

.. _mpremote_command_bootloader:

- **bootloader** enter the bootloader

  .. code-block:: bash

      $ mpremote bootloader

  This will make the device enter its bootloader. The bootloader is port- and
  board-specific (e.g. DFU on stm32, UF2 on rp2040/Pico).

.. _mpremote_reset:

Auto connection and soft-reset
------------------------------

Connection and disconnection will be done automatically at the start and end of
the execution of the tool, if such commands are not explicitly given.  Automatic
connection will search for the first available USB serial device.

Once connected to a device, ``mpremote`` will automatically soft-reset the
device if needed.  This clears the Python heap and restarts the interpreter,
making sure that subsequent Python code executes in a fresh environment.  Auto
soft-reset is performed the first time one of the following commands are
executed: ``mount``, ``eval``, ``exec``, ``run``, ``fs``.  After doing a
soft-reset for the first time, it will not be done again automatically, until a
``disconnect`` command is issued.

Auto-soft-reset behaviour can be controlled by the ``resume`` command. This
might be useful to use the ``eval`` command to inspect the state of of the
device.  The ``soft-reset`` command can be used to perform an explicit soft
reset in the middle of a sequence of commands.

.. _mpremote_shortcuts:

Shortcuts
---------

Shortcuts can be defined using the macro system.  Built-in shortcuts are:

- ``devs``: Alias for ``connect list``

- ``a0``, ``a1``, ``a2``, ``a3``: Aliases for ``connect /dev/ttyACMn``

- ``u0``, ``u1``, ``u2``, ``u3``: Aliases for ``connect /dev/ttyUSBn``

- ``c0``, ``c1``, ``c2``, ``c3``: Aliases for ``connect COMn``

- ``cat``, ``edit``, ``ls``, ``cp``, ``rm``, ``mkdir``, ``rmdir``, ``touch``: Aliases for ``fs <sub-command>``

Additional shortcuts can be defined by in user-configuration files, which is
located at ``.config/mpremote/config.py``. This file should define a
dictionary named ``commands``. The keys of this dictionary are the shortcuts
and the values are either a string or a list-of-strings:

.. code-block:: python3

  "c33": "connect id:334D335C3138",

The command ``c33`` is replaced by ``connect id:334D335C3138``.

.. code-block:: python3

  "test": ["mount", ".", "exec", "import test"],

The command ``test`` is replaced by ``mount . exec "import test"``.

Shortcuts can also accept arguments. For example:

.. code-block:: python3

  "multiply x=4 y=7": "eval x*y",

Running ``mpremote times 3 7`` will set ``x`` and ``y`` as variables on the device, then evaluate the expression ``x*y``.

An example ``config.py`` might look like:

.. code-block:: python3

    commands = {
        "c33": "connect id:334D335C3138", # Connect to a specific device by ID.
        "bl": "bootloader", # Shorter alias for bootloader.
        "double x=4": "eval x*2",  # x is an argument, with default 4
        "wl_scan": ["exec", """
    import network
    wl = network.WLAN()
    wl.active(1)
    for ap in wl.scan():
        print(ap)
    """,], # Print out nearby WiFi networks.
        "wl_ipconfig": [
    "exec",
    "import network; sta_if = network.WLAN(network.WLAN.IF_STA); print(sta_if.ipconfig('addr4'))",
    """,], # Print ip address of station interface.
        "test": ["mount", ".", "exec", "import test"], # Mount current directory and run test.py.
        "demo": ["run", "path/to/demo.py"], # Execute demo.py on the device.
    }

.. _mpremote_examples:

Examples
--------

.. code-block:: bash

  mpremote

Connect to the first available device and implicitly run the ``repl`` command.

.. code-block:: bash

  mpremote a1

Connect to the device at ``/dev/ttyACM1`` (Linux) and implicitly run the
``repl`` command. See :ref:`shortcuts <mpremote_shortcuts>` above.

.. code-block:: bash

  mpremote c1

Connect to the device at ``COM1`` (Windows) and implicitly run the ``repl``
command. See :ref:`shortcuts <mpremote_shortcuts>` above.

.. code-block:: bash

  mpremote connect /dev/ttyUSB0

Explicitly specify which device to connect to, and as above, implicitly run the
``repl`` command.

.. code-block:: bash

  mpremote a1 ls

Connect to the device at ``/dev/ttyACM0`` and then run the ``ls`` command.

It is equivalent to ``mpremote connect /dev/ttyACM1 fs ls``.

.. code-block:: bash

  mpremote exec "import micropython; micropython.mem_info()"

Run the specified Python command and display any output. This is equivalent to
typing the command at the REPL prompt.

.. code-block:: bash

  mpremote eval 1/2 eval 3/4

Evaluate each expression in turn and print the results.

.. code-block:: bash

  mpremote a0 eval 1/2 a1 eval 3/4

Evaluate ``1/2`` on the device at ``/dev/ttyACM0``, then ``3/4`` on the
device at ``/dev/ttyACM1``, printing each result.

.. code-block:: bash

  mpremote resume exec "print_state_info()" soft-reset

Connect to the device without triggering a :ref:`soft reset <soft_reset>` and
execute the ``print_state_info()`` function (e.g. to find out information about
the current program state), then trigger a soft reset.

.. code-block:: bash

  mpremote reset sleep 0.5 bootloader

Hard-reset the device, wait 500ms for it to become available, then enter the
bootloader.

.. code-block:: bash

  mpremote cp utils/driver.py :utils/driver.py + run test.py

Update the copy of utils/driver.py on the device, then execute the local
``test.py`` script on the device. ``test.py`` is never copied to the device
filesystem, rather it is run from RAM.

.. code-block:: bash

  mpremote cp utils/driver.py :utils/driver.py + exec "import app"

Update the copy of utils/driver.py on the device, then execute app.py on the
device.

This is a common development workflow to update a single file and then re-start
your program. In this scenario, your ``main.py`` on the device would also do
``import app``.

.. code-block:: bash

  mpremote cp utils/driver.py :utils/driver.py + soft-reset repl

Update the copy of utils/driver.py on the device, then trigger a soft-reset to
restart your program, and then monitor the output via the ``repl`` command.

.. code-block:: bash

  mpremote cp -r utils/ :utils/ + soft-reset repl

Same as above, but update the entire utils directory first.

.. code-block:: bash

  mpremote mount .

Mount the current local directory at ``/remote`` on the device and starts a
``repl`` session which will use ``/remote`` as the working directory.

.. code-block:: bash

  mpremote mount . exec "import demo"

After mounting the current local directory, executes ``demo.py`` from the
mounted directory.

.. code-block:: bash

  mpremote mount app run test.py

After mounting the local directory ``app`` as ``/remote`` on the device,
executes the local ``test.py`` from the host's current directory without
copying it to the filesystem.

.. code-block:: bash

  mpremote mount . repl --inject-code "import demo"

After mounting the current local directory, executes ``demo.py`` from the
mounted directory each time ``Ctrl-J`` is pressed.

You will first need to press ``Ctrl-D`` to reset the interpreter state
(which will preserve the mount) before pressing ``Ctrl-J`` to re-import
``demo.py``.

.. code-block:: bash

  mpremote mount app repl --inject-file demo.py

Same as above, but executes the contents of the local file demo.py at the REPL
every time ``Ctrl-K`` is pressed. As above, use Ctrl-D to reset the interpreter
state first.

.. code-block:: bash

  mpremote cat boot.py

Displays the contents of ``boot.py`` on the device.

.. code-block:: bash

  mpremote edit utils/driver.py

Edit ``utils/driver.py`` on the device using your local ``$EDITOR``.

.. code-block:: bash

  mpremote cp :main.py .

Copy ``main.py`` from the device to the local directory.

.. code-block:: bash

  mpremote cp main.py :

Copy ``main.py`` from the local directory to the device.

.. code-block:: bash

  mpremote cp :a.py :b.py

Copy ``a.py`` on the device to ``b.py`` on the device.

.. code-block:: bash

  mpremote cp -r dir/ :

Recursively copy the local directory ``dir`` to the remote device.

.. code-block:: bash

  mpremote cp a.py b.py : + repl

Copy ``a.py`` and ``b.py`` from the local directory to the device, then run the
``repl`` command.

.. code-block:: bash

  mpremote mip install aioble

Install the ``aioble`` package from :term:`micropython-lib` to the device.
See :ref:`packages`.

.. code-block:: bash

  mpremote mip install github:org/repo@branch

Install the package from the specified branch at org/repo on GitHub to the
device. See :ref:`packages`.

.. code-block:: bash

  mpremote mip install gitlab:org/repo@branch

Install the package from the specified branch at org/repo on GitLab to the
device. See :ref:`packages`.

.. code-block:: bash

  mpremote mip install --target /flash/third-party functools

Install the ``functools`` package from :term:`micropython-lib` to the
``/flash/third-party`` directory on the device. See :ref:`packages`.

MicroPython manifest files
==========================

When building firmware for a device the following components are included in
the compilation process:

- the core MicroPython virtual machine and runtime
- port-specific system code and drivers to interface with the
  microcontroller/device that the firmware is targeting
- standard built-in modules, like ``sys``
- extended built-in modules, like ``json`` and ``machine``
- extra modules written in C/C++
- extra modules written in Python

All the modules included in the firmware are available via ``import`` from
Python code.  The extra modules written in Python that are included in a build
(the last point above) are called *frozen modules*, and are specified by a
``manifest.py`` file.  Changing this manifest requires rebuilding the firmware.

It's also possible to add additional modules to the filesystem of the device
once it is up and running.  Adding and removing modules to/from the filesystem
does not require rebuilding the firmware so is a simpler process than rebuilding
firmware.  The benefit of using a manifest is that frozen modules are more
efficient: they are faster to import and take up less RAM once imported.

MicroPython manifest files are Python files and can contain arbitrary Python
code.  There are also a set of commands (predefined functions) which are used
to specify the Python source files to include.  These commands are described
below.

Freezing source code
--------------------

.. function:: freeze(path, script=None, opt=0)

    Freeze the input specified by *path*, automatically determining its type.  A
    ``.py`` script will be compiled to a ``.mpy`` first then frozen, and a
    ``.mpy`` file will be frozen directly.

    *path* must be a directory, which is the base directory to begin searching
    for files.  When importing the resulting frozen modules, the name of the
    module will start after *path*, i.e. *path* is excluded from the module
    name.

    If *path* is relative, it is resolved to the current ``manifest.py``.  Use
    ``$(MPY_DIR)``, ``$(MPY_LIB_DIR)``, ``$(PORT_DIR)``, ``$(BOARD_DIR)`` if you
    need to access specific paths.

    If *script* is None, all files in *path* will be frozen.

    If *script* is an iterable then ``freeze()`` is called on all items of the
    iterable (with the same *path* and *opt* passed through).

    If *script* is a string then it specifies the file or directory to freeze,
    and can include extra directories before the file or last directory.  The
    file or directory will be searched for in *path*.  If *script* is a
    directory then all files in that directory will be frozen.

    *opt* is the optimisation level to pass to mpy-cross when compiling ``.py``
    to ``.mpy``.

.. function:: freeze_as_str(path)

    Freeze the given *path* and all ``.py`` scripts within it as a string, which
    will be compiled upon import.

.. function:: freeze_as_mpy(path, script=None, opt=0)

    Freeze the input by first compiling the ``.py`` scripts to ``.mpy`` files,
    then freezing the resulting ``.mpy`` files.  See ``freeze()`` for further
    details on the arguments.

.. function::   freeze_mpy(path, script=None, opt=0)

    Freeze the input, which must be ``.mpy`` files that are frozen directly.
    See ``freeze()`` for further details on the arguments.


Including other manifest files
------------------------------

.. function:: include(manifest, **kwargs)

    Include another manifest.

    The *manifest* argument can be a string (filename) or an iterable of
    strings.

    Relative paths are resolved with respect to the current manifest file.

    Optional *kwargs* can be provided which will be available to the included
    script via the *options* variable.

    For example:

    .. code-block:: python3

        include("path.py", extra_features=True)

    then in path.py:

    .. code-block:: python3

            options.defaults(standard_features=True)
            # freeze minimal modules.
            if options.standard_features:
                # freeze standard modules.
            if options.extra_features:
                # freeze extra modules.


Examples
--------

To freeze a single file which is available as ``import mydriver``, use:

.. code-block:: python3

    freeze(".", "mydriver.py")

To freeze a set of files which are available as ``import test1`` and
``import test2``, and which are compiled with optimisation level 3, use:

.. code-block:: python3

    freeze("/path/to/tests", ("test1.py", "test2.py"), opt=3)

To freeze a module which can be imported as ``import mymodule``, use:

.. code-block:: python3

    freeze(
        "../relative/path",
        (
            "mymodule/__init__.py",
            "mymodule/core.py",
            "mymodule/extra.py",
        ),
    )

To include a manifest from the MicroPython repository, use:

.. code-block:: python3

    include("$(MPY_DIR)/extmod/uasyncio/manifest.py")

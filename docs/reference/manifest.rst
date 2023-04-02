.. _manifest:

MicroPython manifest files
==========================

Summary
-------

MicroPython has a feature that allows Python code to be "frozen" into the
firmware, as an alternative to loading code from the filesystem.

This has the following benefits:

- the code is pre-compiled to bytecode, avoiding the need for the Python
  source to be compiled at load-time.
- the bytecode can be executed directly from ROM (i.e. flash memory) rather than
  being copied into RAM. Similarly any constant objects (strings, tuples, etc)
  are loaded from ROM also. This can lead to significantly more memory being
  available for your application.
- on devices that do not have a filesystem, this is the only way to
  load Python code.

During development, freezing is generally not recommended as it will
significantly slow down your development cycle, as each update will require
re-flashing the entire firmware. However, it can still be useful to
selectively freeze some rarely-changing dependencies (such as third-party
libraries).

The way to list the Python files to be be frozen into the firmware is via
a "manifest", which is a Python file that will be interpreted by the build
process. Typically you would write a manifest file as part of a board
definition, but you can also write a stand-alone manifest file and use it with
an existing board definition.

Manifest files can define dependencies on libraries from :term:`micropython-lib`
as well as Python files on the filesystem, and also on other manifest files.

Writing manifest files
----------------------

A manifest file is a Python file containing a series of function calls. See the
available functions defined below.

Any paths used in manifest files can include the following variables. These all
resolve to absolute paths.

- ``$(MPY_DIR)`` -- path to the micropython repo.
- ``$(MPY_LIB_DIR)`` -- path to the micropython-lib submodule. Prefer to use
  ``require()``.
- ``$(PORT_DIR)`` -- path to the current port (e.g. ``ports/stm32``)
- ``$(BOARD_DIR)`` -- path to the current board
  (e.g. ``ports/stm32/boards/PYBV11``)

Custom manifest files should not live in the main MicroPython repository. You
should keep them in version control with the rest of your project.

Typically a manifest used for compiling firmware will need to include the port
manifest, which might include frozen modules that are required for the board to
function. If you just want to add additional modules to an existing board, then
include the board manifest (which will in turn include the port manifest).

Building with a custom manifest
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Your manifest can be specified on the ``make`` command line with:

.. code-block:: bash

    $ make BOARD=MYBOARD FROZEN_MANIFEST=/path/to/my/project/manifest.py

This applies to all ports, including CMake-based ones (e.g. esp32, rp2), as the
Makefile wrapper that will pass this into the CMake build.

Adding a manifest to a board definition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have a custom board definition, you can make it include your custom
manifest automatically. On make-based ports (most ports), in your
``mpconfigboard.mk`` set the ``FROZEN_MANIFEST`` variable.

.. code-block:: makefile

    FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py

On CMake-based ports (e.g. esp32, rp2), instead use ``mpconfigboard.cmake``

.. code-block:: cmake

    set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

High-level functions
~~~~~~~~~~~~~~~~~~~~

Note: The ``opt`` keyword argument can be set on the various functions, this controls
the optimisation level used by the cross-compiler.
See :func:`micropython.opt_level`.

.. function:: package(package_path, files=None, base_path=".", opt=None)

    This is equivalent to copying the "package_path" directory to the device
    (except as frozen code).

    In the simplest case, to freeze a package "foo" in the current directory:

    .. code-block:: python3

        package("foo")

    will recursively include all .py files in foo, and will be frozen as
    ``foo/**/*.py``.

    If the package isn't in the same directory as the manifest file, use ``base_path``:

    .. code-block:: python3

        package("foo", base_path="path/to/libraries")

    You can use the variables above, such as ``$(PORT_DIR)`` in ``base_path``.

    To restrict to certain files in the package use ``files`` (note: paths
    should be relative to the package): ``package("foo", files=["bar/baz.py"])``.

.. function:: module(module_path, base_path=".", opt=None)

    Include a single Python file as a module.

    If the file is in the current directory:

    .. code-block:: python3

        module("foo.py")

    Otherwise use base_path to locate the file:

    .. code-block:: python3

        module("foo.py", base_path="src/drivers")

    You can use the variables above, such as ``$(PORT_DIR)`` in ``base_path``.

.. function:: require(name, unix_ffi=False)

    Require a package by name (and its dependencies) from :term:`micropython-lib`.

    Optionally specify unix_ffi=True to use a module from the unix-ffi directory.

.. function:: include(manifest_path)

    Include another manifest.

    Typically a manifest used for compiling firmware will need to include the
    port manifest, which might include frozen modules that are required for
    the board to function.

    The *manifest* argument can be a string (filename) or an iterable of
    strings.

    Relative paths are resolved with respect to the current manifest file.

    If the path is to a directory, then it implicitly includes the
    manifest.py file inside that directory.

    You can use the variables above, such as ``$(PORT_DIR)`` in ``manifest_path``.

.. function:: metadata(description=None, version=None, license=None, author=None)

    Define metadata for this manifest file. This is useful for manifests for
    micropython-lib packages.

Low-level functions
~~~~~~~~~~~~~~~~~~~

These functions are documented for completeness, but with the exception of
``freeze_as_str`` all functionality can be accessed via the high-level functions.

.. function:: freeze(path, script=None, opt=0)

    Freeze the input specified by *path*, automatically determining its type.  A
    ``.py`` script will be compiled to a ``.mpy`` first then frozen, and a
    ``.mpy`` file will be frozen directly.

    *path* must be a directory, which is the base directory to begin searching
    for files.  When importing the resulting frozen modules, the name of the
    module will start after *path*, i.e. *path* is excluded from the module
    name.

    If *path* is relative, it is resolved to the current ``manifest.py``.

    If *script* is None, all files in *path* will be frozen.

    If *script* is an iterable then ``freeze()`` is called on all items of the
    iterable (with the same *path* and *opt* passed through).

    If *script* is a string then it specifies the file or directory to freeze,
    and can include extra directories before the file or last directory.  The
    file or directory will be searched for in *path*.  If *script* is a
    directory then all files in that directory will be frozen.

    *opt* is the optimisation level to pass to mpy-cross when compiling ``.py``
    to ``.mpy``.  These levels are described in :func:`micropython.opt_level`.

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

Examples
--------

To freeze a single file from the current directory which will be available as
``import mydriver``, use:

.. code-block:: python3

    module("mydriver.py")

To freeze a directory of files in a subdirectory "mydriver" of the current
directory which will be available as ``import mydriver``, use:

.. code-block:: python3

    package("mydriver")

To freeze the "hmac" library from :term:`micropython-lib`, use:

.. code-block:: python3

    require("hmac")

A more complete example of a custom ``manifest.py`` file for the ``PYBD_SF2``
board is:

.. code-block:: python3

    # Include the board's default manifest.
    include("$(BOARD_DIR)/manifest.py")
    # Add a custom driver
    module("mydriver.py")
    # Add aiorepl from micropython-lib
    require("aiorepl")

Then the board can be compiled with

.. code-block:: bash

    $ cd ports/stm32
    $ make BOARD=PYBD_SF2 FROZEN_MANIFEST=~/src/myproject/manifest.py

Note that most boards do not have their own ``manifest.py``, rather they use the
port one directly, in which case your manifest should just
``include("$(PORT_DIR)/boards/manifest.py")`` instead.

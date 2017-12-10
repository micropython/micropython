Distribution packages, package management, and deploying applications
=====================================================================

Just as the "big" Python, MicroPython supports creation of "third party"
packages, distributing them, and easily installing them in each user's
environment. This chapter discusses how these actions are achieved.
Some familiarity with Python packaging is recommended.

Overview
--------

Steps below represent a high-level workflow when creating and consuming
packages:

1. Python modules and packages are turned into distribution package
   archives, and published at the Python Package Index (PyPI).
2. `upip` package manager can be used to install a distribution package
   on a `MicroPython port` with networking capabilities (for example,
   on the Unix port).
3. For ports without networking capabilities, an "installation image"
   can be prepared on the Unix port, and transferred to a device by
   suitable means.
4. For low-memory ports, the installation image can be frozen as the
   bytecode into MicroPython executable, thus minimizing the memory
   storage overheads.

The sections below describe this process in details.

Distribution packages
---------------------

Python modules and packages can be packaged into archives suitable for
transfer between systems, storing at the well-known location (PyPI),
and downloading on demand for deployment. These archives are known as
*distribution packages* (to differentiate them from Python packages
(means to organize Python source code)).

The MicroPython distribution package format is a well-known tar.gz
format, with some adaptations however. The Gzip compressor, used as
an external wrapper for TAR archives, by default uses 32KB dictionary
size, which means that to uncompress a compressed stream, 32KB of
contguous memory needs to be allocated. This requirement may be not
satisfiable on low-memory devices, which may have total memory available
less than that amount, and even if not, a contiguous block like that
may be hard to allocate due to `memory fragmentation`. To accommodate
these constraints, MicroPython distribution packages use Gzip compression
with the dictionary size of 4K, which should be a suitable compromise
with still achieving some compression while being able to uncompressed
even by the smallest devices.

Besides the small compression dictionary size, MicroPython distribution
packages also have other optimizations, like removing any files from
the archive which aren't used by the installation process. In particular,
`upip` package manager doesn't execute ``setup.py`` during installation
(see below), and thus that file is not included in the archive.

At the same time, these optimizations make MicroPython distribution
packages not compatible with `CPython`'s package manager, ``pip``.
This isn't considered a big problem, because:

1. Packages can be installed with `upip`, and then can be used with
   CPython (if they are compatible with it).
2. In the other direction, majority of CPython packages would be
   incompatible with MicroPython by various reasons, first of all,
   the reliance on features not implemented by MicroPython.

Summing up, the MicroPython distribution package archives are highly
optimized for MicroPython's target environments, which are highly
resource constrained devices.


``upip`` package manager
------------------------

MicroPython distribution packages are intended to be installed using
the `upip` package manager. `upip` is a Python application which is
usually distributed (as frozen bytecode) with network-enabled
`MicroPython ports <MicroPython port>`. At the very least,
`upip` is available in the `MicroPython Unix port`.

On any `MicroPython port` providing `upip`, it can be accessed as
following::

    import upip
    upip.help()
    upip.install(package_or_package_list, [path])

Where *package_or_package_list* is the name of a distribution
package to install, or a list of such names to install multiple
packages. Optional *path* parameter specifies filesystem
location to install under and defaults to the standard library
location (see below).

An example of installing a specific package and then using it::

    >>> import upip
    >>> upip.install("micropython-pystone_lowmem")
    [...]
    >>> import pystone_lowmem
    >>> pystone_lowmem.main()

Note that the name of Python package and the name of distribution
package for it in general don't have to match, and oftentimes they
don't. This is because PyPI provides a central package repository
for all different Python implementations and versions, and thus
distribution package names may need to be namespaced for a particular
implementation. For example, all packages from `micropython-lib`
follow this naming convention: for a Python module or package named
``foo``, the distribution package name is ``micropython-foo``.

For the ports which run MicroPython executable from the OS command
prompts (like the Unix port), `upip` can be (and indeed, usually is)
run from the command line instead of MicroPython's own REPL. The
commands which corresponds to the example above are::

    micropython -m upip -h
    micropython -m upip install [-p <path>] <packages>...
    micropython -m upip install micropython-pystone_lowmem

[TODO: Describe installation path.]


Cross-installing packages
-------------------------

For `MicroPython ports <MicroPython port>` without native networking
capabilities, the recommend process is "cross-installing" them into a
"directory image" using the `MicroPython Unix port`, and then
transferring this image to a device by suitable means.

Installing to a directory image involves using ``-p`` switch to `upip`::

    micropython -m upip install -p install_image micropython-pystone_lowmem

After this command, the package content (and contents of every depenency
packages) will be available in the ``install_image/`` subdirectory. You
would need to transfer contents of this directory (without the
``install_image/`` prefix) to the device, at the suitable location, where
it can be found by the Python ``import`` statement (see discussion of
the `upip` installation path above).


Cross-installing packages with freezing
---------------------------------------

For the low-memory `MicroPython ports <MicroPython port>`, the process
described in the previous section does not provide the most efficient
resource usage,because the packages are installed in the source form,
so need to be compiled to the bytecome on each import. This compilation
requires RAM, and the resulting bytecode is also stored in RAM, reducing
its amount available for storing application data. Moreover, the process
above requires presence of the filesystem on a device, and the most
resource-constrained devices may not even have it.

The bytecode freezing is a process which resolves all the issues
mentioned above:

* The source code is pre-compiled into bytecode and store as such.
* The bytecode is stored in ROM, not RAM.
* Filesystem is not required for frozen packages.

Using frozen bytecode requires building the executable (firmware)
for a given `MicroPython port` from the C source code. Consequently,
the process is:

1. Follow the instructions for a particular port on setting up a
   toolchain and building the port. For example, for ESP8266 port,
   study instructions in ``ports/esp8266/README.md`` and follow them.
   Make sure you can build the port and deploy the resulting
   executable/firmware successfully before proceeding to the next steps.
2. Build `MicroPython Unix port` and make sure it is in your PATH and
   you can execute ``micropython``.
3. Change to port's directory (e.g. ``ports/esp8266/`` for ESP8266).
4. Run ``make clean-frozen``. This step cleans up any previous
   modules which were installed for freezing (consequently, you need
   to skip this step to add additional modules, instead of starting
   from scratch).
5. Run ``micropython -m upip install -p modules <packages>...`` to
   install packages you want to freeze.
6. Run ``make clean``.
7. Run ``make``.

After this, you should have the executable/firmware with modules as
the bytecode inside, which you can deploy the usual way.

Few notes:

1. Step 5 in the sequence above assumes that the distribution package
   is available from PyPI. If that is not the case, you would need
   to copy Python source files manually to ``modules/`` subdirectory
   of the port port directory. (Note that upip does not support
   installing from e.g. version control repositories).
2. The firmware for baremetal devices usually has size restrictions,
   so adding too many frozen modules may overflow it. Usually, you
   would get a linking error if this happens. However, in some cases,
   an image may be produced, which is not runnable on a device. Such
   cases are in general bugs, and should be reported and further
   investigated. If you face such a situation, as an initial step,
   you may want to decrease the amount of frozen modules included.


Application resources
---------------------

A complete application, besides the source code, oftentimes also consists
of data files, e.g. web page templates, game images, etc. It's clear how
to deal with those when application is installed manually - you just put
those data files in the filesystem at some location and use the normal
file access functions.

The situation is different when deploying applications from packages - this
is more advanced, streamlined and flexible way, but also requires more
advanced approach to accessing data files. This approach is treating
the data files as "resources", and abstracting away access to them.

Python supports resource access using its "setuptools" library, using
``pkg_resources`` module. MicroPython, following its usual approach,
implements subset of the functionality of that module, specifically
`pkg_resources.resource_stream(package, resource)` function.
The idea is that an application calls this function, passing a
resource identifier, which is a relative path to data file within
the specified package (usually top-level application package). It
returns a stream object which can be used to access resource contents.
Thus, the ``resource_stream()`` emulates interface of the standard
`open()` function.

Implementation-wise, ``resource_stream()`` uses file operations
underlyingly, if distribution package is install in the filesystem.
However, it also supports functioning without the underlying filesystem,
e.g. if the package is frozen as the bytecode. This however requires
an extra intermediate step when packaging application - creation of
"Python resource module".

The idea of this module is to convert binary data to a Python bytes
object, and put it into the dictionary, indexed by the resource name.
This conversion is done using ``tools/mpy_bin2res.py`` script from
the MicroPython distribution.

Let's trace the complete process using the following example. Suppose
your application has the following structure::

    my_app/
        __main__.py
        utils.py
        data/
            page.html
            image.png

``__main__.py`` and ``utils.py`` should access resources using the
following calls::

    import pkg_resources

    pkg_resources.resource_stream(__name__, "data/page.html")
    pkg_resources.resource_stream(__name__, "data/image.png")

You can develop and debug using the `MicroPython Unix port` as usual.
When times come to make a distribution package out of it, you would
need to run following command, with ``my_app/`` being the current
directory (and assuming ``mpy_bin2res.py`` is in your path)::

    mpy_bin2res.py data/page.html data/image.png

This will produce a Python resource module named ``R.py``. Afterwards,
you package the project for distribution as usual (using ``setup.py sdist``).
Prepared like this, your application will work both when deployed to
filesystem and as frozen bytecode.

References
----------

* Python Packaging User Guide: https://packaging.python.org/
* Setuptools documentation: https://setuptools.readthedocs.io/
* Distutils documentation: https://docs.python.org/3/library/distutils.html

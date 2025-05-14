.. _packages:

Package management
==================

Installing packages with ``mip``
--------------------------------

Network-capable boards include the ``mip`` module, which can install packages
from :term:`micropython-lib` and from third-party sites (including GitHub, GitLab).

``mip`` ("mip installs packages") is similar in concept to Python's ``pip`` tool,
however it does not use the PyPI index, rather it uses :term:`micropython-lib`
as its index by default. ``mip`` will automatically fetch compiled
:term:`.mpy file` when downloading from micropython-lib.

The most common way to use ``mip`` is from the REPL::

    >>> import mip
    >>> mip.install("pkgname")  # Installs the latest version of "pkgname" (and dependencies)
    >>> mip.install("pkgname", version="x.y")  # Installs version x.y of "pkgname"
    >>> mip.install("pkgname", mpy=False)  # Installs the source version (i.e. .py rather than .mpy files)

``mip`` will detect an appropriate location on the filesystem by searching
``sys.path`` for the first entry ending in ``/lib``. You can override the
destination using ``target``, but note that this path must be in ``sys.path`` to be
able to subsequently import it.::

    >>> mip.install("pkgname", target="third-party")
    >>> sys.path.append("third-party")

As well as downloading packages from the micropython-lib index, ``mip`` can also
install third-party libraries. The simplest way is to download a file directly::

    >>> mip.install("http://example.com/x/y/foo.py")
    >>> mip.install("http://example.com/x/y/foo.mpy")

When installing a file directly, the ``target`` argument is still supported to set
the destination path, but ``mpy`` and ``version`` are ignored.

The URL can also start with ``github:`` or ``gitlab:`` as a simple way of pointing to content
hosted on GitHub or GitLab::

    >>> mip.install("github:org/repo/path/foo.py")  # Uses default branch
    >>> mip.install("github:org/repo/path/foo.py", version="branch-or-tag")  # Optionally specify the branch or tag
    >>> mip.install("gitlab:org/repo/path/foo.py")  # Uses default branch
    >>> mip.install("gitlab:org/repo/path/foo.py", version="branch-or-tag")  # Optionally specify the branch or tag

More sophisticated packages (i.e. with more than one file, or with dependencies)
can be downloaded by specifying the path to their ``package.json``.

    >>> mip.install("http://example.com/x/package.json")
    >>> mip.install("github:org/user/path/package.json")
    >>> mip.install("gitlab:org/user/path/package.json")

If no json file is specified, then "package.json" is implicitly added::

    >>> mip.install("http://example.com/x/")
    >>> mip.install("github:org/repo")  # Uses default branch of that repo
    >>> mip.install("github:org/repo", version="branch-or-tag")
    >>> mip.install("gitlab:org/repo")  # Uses default branch of that repo
    >>> mip.install("gitlab:org/repo", version="branch-or-tag")

Using ``mip`` on the Unix port
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

On the Unix port, ``mip`` can be used at the REPL as above, and also by using ``-m``::

    $ ./micropython -m mip install pkgname-or-url
    $ ./micropython -m mip install pkgname-or-url@version

The ``--target path``, ``--no-mpy``, and ``--index`` arguments can be set::

    $ ./micropython -m mip install --target third-party pkgname
    $ ./micropython -m mip install --no-mpy pkgname
    $ ./micropython -m mip install --index https://host/pi pkgname

Installing packages with ``mpremote``
-------------------------------------

The :term:`mpremote` tool also includes the same functionality as ``mip`` and
can be used from a host PC to install packages to a locally connected device
(e.g. via USB or UART)::

    $ mpremote mip install pkgname
    $ mpremote mip install pkgname@x.y
    $ mpremote mip install http://example.com/x/y/foo.py
    $ mpremote mip install github:org/repo
    $ mpremote mip install github:org/repo@branch-or-tag
    $ mpremote mip install gitlab:org/repo
    $ mpremote mip install gitlab:org/repo@branch-or-tag

The ``--target=path``, ``--no-mpy``, and ``--index`` arguments can be set::

    $ mpremote mip install --target=/flash/third-party pkgname
    $ mpremote mip install --no-mpy pkgname
    $ mpremote mip install --index https://host/pi pkgname

:term:`mpremote` can also install packages from files stored on the host's local
filesystem::

    $ mpremote mip install path/to/pkg.py
    $ mpremote mip install path/to/app/package.json
    $ mpremote mip install \\path\\to\\pkg.py

This is especially useful for testing packages during development and for
installing packages from local clones of GitHub repositories. Note that URLs in
``package.json`` files must use forward slashes ("/") as directory separators,
even on Windows, so that they are compatible with installing from the web.

Installing packages manually
----------------------------

Packages can also be installed (in either .py or .mpy form) by manually copying
the files to the device. Depending on the board this might be via USB Mass Storage,
the :term:`mpremote` tool (e.g. ``mpremote fs cp path/to/package.py :package.py``),
:term:`webrepl`, etc.

Writing & publishing packages
-----------------------------

Publishing to :term:`micropython-lib` is the easiest way to make your package
broadly accessible to MicroPython users, and automatically available via
``mip`` and ``mpremote`` and compiled to bytecode. See
https://github.com/micropython/micropython-lib for more information.

To write a "self-hosted" package that can be downloaded by ``mip`` or
``mpremote``, you need a static webserver (or GitHub) to host either a
single .py file, or a ``package.json`` file alongside your .py files.

An example ``mlx90640`` library hosted on GitHub could be installed with::

    $ mpremote mip install github:org/micropython-mlx90640

The layout for the package on GitHub might look like::

    https://github.com/org/micropython-mlx90640/
        package.json
        mlx90640/
            __init__.py
            utils.py

The ``package.json`` specifies the location of files to be installed and other
dependencies::

    {
      "urls": [
        ["mlx90640/__init__.py", "mlx90640/__init__.py"],
        ["mlx90640/utils.py", "mlx90640/utils.py"]
      ],
      "deps": [
        ["collections-defaultdict", "latest"],
        ["os-path", "latest"],
        ["github:org/micropython-additions", "main"],
        ["gitlab:org/micropython-otheradditions", "main"]
      ],
      "version": "0.2"
    }

The ``urls`` list specifies the files to be installed according to::

    "urls": [
        [destination_path, source_url]
        ...

where ``destination_path`` is the location and name of the file to be installed
on the device and ``source_url`` is the URL of the file to be installed. The
source URL would usually be specified relative to the directory containing the
``package.json`` file, but can also be an absolute URL, eg::

    ["mlx90640/utils.py", "github:org/micropython-mlx90640/mlx90640/utils.py"]

The package depends on ``collections-defaultdict`` and ``os-path`` which will
be installed automatically from the :term:`micropython-lib`. The third
dependency installs the content as defined by the ``package.json`` file of the
``main`` branch of the GitHub repo ``org/micropython-additions``.

Freezing packages
-----------------

When a Python module or package is imported from the device filesystem, it is
compiled into :term:`bytecode` in RAM, ready to be executed by the VM. For
a :term:`.mpy file`, this conversion has been done already, but the bytecode
still ends up in RAM.

For low-memory devices, or for large applications, it can be advantageous to
instead run the bytecode from ROM (i.e. flash memory). This can be done
by "freezing" the bytecode into the MicroPython firmware, which is then flashed
to the device. The runtime performance is the same (although importing is
faster), but it can free up significant amounts of RAM for your program to
use.

The downside of this approach is that it's much slower to develop, because you
have to flash the firmware each time, but it can be still useful to freeze
dependencies that don't change often.

Freezing is done by writing a manifest file and using it in the build, often as
part of a custom board definition. See the :ref:`manifest` guide for more
information.

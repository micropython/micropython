.. _gettingstarted:

Getting Started
===============

This guide covers a step-by-step process on setting up version control, obtaining and building
a copy of the source code for a port, building the documentation, running tests, and a description of the
directory structure of the MicroPython code base.

Source control with git
-----------------------

MicroPython is hosted on `GitHub <https://github.com/micropython/micropython>`_ and uses
`Git <https://git-scm.com>`_ for source control. The workflow is such that
code is pulled and pushed to and from the main repository. Install the respective version
of Git for your operating system to follow through the rest of the steps.

.. note::
   For a reference on the installation instructions, please refer to
   the `Git installation instructions <https://git-scm.com/book/en/v2/Getting-Started-Installing-Git>`_.
   Learn about the basic git commands in this `Git Handbook <https://guides.github.com/introduction/git-handbook/>`_
   or any other sources on the internet.

.. note::
   A .git-blame-ignore-revs file is included which avoids the output of git blame getting cluttered
   by commits which are only for formatting code but have no functional changes. See `git blame documentation
   <https://git-scm.com/docs/git-blame#Documentation/git-blame.txt---ignore-revltrevgt>`_ on how to use this.

Get the code
------------

It is recommended that you maintain a fork of the MicroPython repository for your development purposes.
The process of obtaining the source code includes the following:

#. Fork the repository https://github.com/micropython/micropython
#. You will now have a fork at <https://github.com/<your-user-name>/micropython>.
#. Clone the forked repository using the following command:

.. code-block:: bash

   $ git clone https://github.com/<your-user-name>/micropython

Then, `configure the remote repositories <https://git-scm.com/book/en/v2/Git-Basics-Working-with-Remotes>`_ to be able to
collaborate on the MicroPython project.

Configure remote upstream:

.. code-block:: bash

   $ cd micropython
   $ git remote add upstream https://github.com/micropython/micropython

It is common to configure ``upstream`` and ``origin`` on a forked repository
to assist with sharing code changes. You can maintain your own mapping but
it is recommended that ``origin`` maps to your fork and ``upstream`` to the main
MicroPython repository.

After the above configuration, your setup should be similar to this:

.. code-block:: bash

   $ git remote -v
   origin       https://github.com/<your-user-name>/micropython (fetch)
   origin       https://github.com/<your-user-name>/micropython (push)
   upstream     https://github.com/micropython/micropython (fetch)
   upstream     https://github.com/micropython/micropython (push)

You should now have a copy of the source code. By default, you are pointing
to the master branch. To prepare for further development, it is recommended
to work on a development branch.

.. code-block:: bash

    $ git checkout -b dev-branch

You can give it any name. You will have to compile MicroPython whenever you change
to a different branch.

Compile and build the code
--------------------------

When compiling MicroPython, you compile a specific :term:`port`, usually
targeting a specific :ref:`board <glossary>`. Start by installing the required dependencies.
Then build the MicroPython cross-compiler before you can successfully compile and build.
This applies specifically when using Linux to compile.
The Windows instructions are provided in a later section.

.. _required_dependencies:

Required dependencies
~~~~~~~~~~~~~~~~~~~~~

Install the required dependencies for Linux:

.. code-block:: bash

   $ sudo apt-get install build-essential libffi-dev git pkg-config

For the stm32 port, the ARM cross-compiler is required:

.. code-block:: bash

   $ sudo apt-get install arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-newlib

See the `ARM GCC
toolchain <https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm>`_
for the latest details.

Python is also required. Python 2 is supported for now, but we recommend using Python 3.
Check that you have Python available on your system:

.. code-block:: bash

   $ python3
   Python 3.5.0 (default, Jul 17 2020, 14:04:10) 
   [GCC 5.4.0 20160609] on linux
   Type "help", "copyright", "credits" or "license" for more information.
   >>> 

All supported ports have different dependency requirements, see their respective
`readme files <https://github.com/micropython/micropython/tree/master/ports>`_.

Building the MicroPython cross-compiler
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Almost all ports require building ``mpy-cross`` first to perform pre-compilation
of Python code that will be included in the port firmware:

.. code-block:: bash

   $ cd mpy-cross
   $ make

.. note::
   Note that, ``mpy-cross`` must be built for the host architecture
   and not the target architecture.

If it built successfully, you should see a message similar to this:

.. code-block:: bash

   LINK mpy-cross
      text          data    bss     dec     hex filename
    279328          776     880  280984   44998 mpy-cross

.. note::

   Use ``make -C mpy-cross`` to build the cross-compiler in one statement
   without moving to the ``mpy-cross`` directory otherwise, you will need
   to do ``cd ..`` for the next steps.

Building the Unix port of MicroPython
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Unix port is a version of MicroPython that runs on Linux, macOS, and other Unix-like operating systems.
It's extremely useful for developing MicroPython as it avoids having to deploy your code to a device to test it.
In many ways, it works a lot like CPython's python binary.

To build for the Unix port, make sure all Linux related dependencies are installed as detailed in the
required dependencies section. See the :ref:`required_dependencies`
to make sure that all dependencies are installed for this port. Also, make sure you have a working
environment for ``gcc`` and ``GNU make``. Ubuntu 20.04 has been used for the example
below but other unixes ought to work with little modification:

.. code-block:: bash

   $ gcc --version
   gcc (Ubuntu 9.3.0-10ubuntu2) 9.3.0
   Copyright (C) 2019 Free Software Foundation, Inc.
   This is free software; see the source for copying conditions.  There is NO
   warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.then build:

.. code-block:: bash

   $ cd ports/unix
   $ make submodules
   $ make

If MicroPython built correctly, you should see the following:

.. code-block:: bash

   LINK micropython
      text         data     bss     dec     hex filename
    412033         5680    2496  420209   66971 micropython

Now run it:

.. code-block:: bash

   $ ./micropython
   MicroPython v1.13-38-gc67012d-dirty on 2020-09-13; linux version
   Use Ctrl-D to exit, Ctrl-E for paste mode
   >>> print("hello world")
   hello world
   >>>

Building the Windows port
~~~~~~~~~~~~~~~~~~~~~~~~~

The Windows port includes a Visual Studio project file micropython.vcxproj that you can use to build micropython.exe.
It can be opened in Visual Studio or built from the command line using msbuild. Alternatively, it can be built using mingw,
either in Windows with Cygwin, or on Linux.
See `windows port documentation <https://github.com/micropython/micropython/tree/master/ports/windows>`_ for more information.

Building the STM32 port
~~~~~~~~~~~~~~~~~~~~~~~

Like the Unix port, you need to install some required dependencies
as detailed in the :ref:`required_dependencies` section, then build:

.. code-block:: bash

   $ cd ports/stm32
   $ make submodules
   $ make

Please refer to the `stm32 documentation <https://github.com/micropython/micropython/tree/master/ports/stm32>`_
for more details on flashing the firmware.

.. note::
   See the :ref:`required_dependencies` to make sure that all dependencies are installed for this port.
   The cross-compiler is needed. ``arm-none-eabi-gcc`` should also be in the $PATH or specified manually
   via CROSS_COMPILE, either by setting the environment variable or in the ``make`` command line arguments.

You can also specify which board to use:

.. code-block:: bash

   $ cd ports/stm32
   $ make submodules
   $ make BOARD=<board>

See `ports/stm32/boards <https://github.com/micropython/micropython/tree/master/ports/stm32/boards>`_
for the available boards. e.g. "PYBV11" or "NUCLEO_WB55".

Building the documentation
--------------------------

MicroPython documentation is created using ``Sphinx``. If you have already
installed Python, then install ``Sphinx`` using ``pip``. It is recommended
that you use a virtual environment:

.. code-block:: bash

   $ python3 -m venv env
   $ source env/bin/activate
   $ pip install sphinx

Navigate to the ``docs`` directory:

.. code-block:: bash

   $ cd docs

Build the docs:

.. code-block:: bash

   $ make html

Open ``docs/build/html/index.html`` in your browser to view the docs locally. Refer to the
documentation on `importing your documentation
<https://docs.readthedocs.io/en/stable/intro/import-guide.html>`_ to use Read the Docs.

Running the tests
-----------------

To run all tests in the test suite on the Unix port use:

.. code-block:: bash

   $ cd ports/unix
   $ make test

To run a selection of tests on a board/device connected over USB use:

.. code-block:: bash

   $ cd tests
   $ ./run-tests.py --target minimal --device /dev/ttyACM0

See also :ref:`writingtests`.

Folder structure
----------------

There are a couple of directories to take note of in terms of where certain implementation details
are. The following is a break down of the top-level folders in the source code.

py

  Contains the compiler, runtime, and core library implementation.

mpy-cross

  Has the MicroPython cross-compiler which pre-compiles the Python scripts to bytecode.

ports

  Code for all the versions of MicroPython for the supported ports.

lib

  Low-level C libraries used by any port which are mostly 3rd-party libraries.

drivers

  Has drivers for specific hardware and intended to work across multiple ports.

extmod

  Contains a C implementation of more non-core modules.

docs

  Has the standard documentation found at https://docs.micropython.org/.

tests

  An implementation of the test suite.

tools

  Contains helper tools including the ``upip`` and the ``pyboard.py`` module.

examples

  Example code for building MicroPython as a library as well as native modules.

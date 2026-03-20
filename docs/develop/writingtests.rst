.. _writingtests:

Writing tests
=============

Tests in MicroPython are located at the path ``tests/``. The following is a listing of
key directories and the run-tests.py runner script:

.. code-block:: bash

   .
    ├── basics
    ├── extmod
    ├── float
    ├── micropython
    ├── run-tests.py
    ...

There are subfolders maintained to categorize the tests. Add a test by creating a new file in one of the
existing folders or in a new folder. It's also possible to make custom tests outside this tests folder,
which would be recommended for a custom port.

For example, add the following code in a file ``print.py`` in the ``tests/unix/`` subdirectory:

.. code-block:: python

   def print_one():
       print(1)

   print_one()

If you run your tests, this test should appear in the test output:

.. code-block:: bash

   $ cd ports/unix
   $ make tests
   skip  unix/extra_coverage.py
   pass  unix/ffi_callback.py
   pass  unix/ffi_float.py
   pass  unix/ffi_float2.py
   pass  unix/print.py
   pass  unix/time.py
   pass  unix/time2.py

Tests are run by comparing the output from the test target against the output from CPython.
So any test should use print statements to indicate test results.

When writing tests for name or string-related functionality, please add both English/ASCII 
as well as non-English/non-ASCII text and include Unicode examples.
Please do add comments in English explaining the meaning and intent of the Unicode text. 
This help ensure Unicode support is tested and verified across different platforms.

For tests that can't be compared to CPython (i.e. micropython-specific functionality),
you can provide a ``.py.exp`` file which will be used as the truth for comparison.

The other way to run tests, which is useful when running on targets other than the Unix port, is:

.. code-block:: bash

   $ cd tests
   $ ./run-tests.py

Then to run on a board:

.. code-block:: bash

   $ ./run-tests.py -t /dev/ttyACM0

And to run only a certain set of tests (eg a directory):

.. code-block:: bash

   $ ./run-tests.py -d basics
   $ ./run-tests.py float/builtin*.py

Using run-tests.py
------------------

The ``run-tests.py`` script supports several parameters to customize test execution:

**Target and Device Selection:**

* ``-t, --test-instance``

The -t option accepts the following for the test instance:

- **unix** - use the unix port of MicroPython, specified by the MICROPY_MICROPYTHON
  environment variable (which defaults to the standard variant of either the unix
  or windows ports, depending on the host platform)
- **webassembly** - use the webassembly port of MicroPython, specified by the
  MICROPY_MICROPYTHON_MJS environment variable (which defaults to the standard
  variant of the webassembly port)
- **port:<device>** - connect to and use the given serial port device
- **a<n>** - connect to and use /dev/ttyACM<n>
- **u<n>** - connect to and use /dev/ttyUSB<n>
- **c<n>** - connect to and use COM<n>
- **exec:<command>** - execute a command and attach to it's stdin/stdout
- **execpty:<command>** - execute a command and attach to the printed /dev/pts/<n> device
- **<a>.<b>.<c>.<d>** - connect to the given IPv4 address
- anything else specifies a serial port

**Test Selection:**

* ``-d, --test-dirs`` - Specify one or more test directories to run
* ``-i, --include REGEX`` - Include tests matching regex pattern
* ``-e, --exclude REGEX`` - Exclude tests matching regex pattern
* ``files`` - Specific test files to run

**Execution Options:**

* ``--emit <EMITTER>`` - MicroPython emitter, EMITTER can be bytecode or native. Default: bytecode
* ``--via-mpy`` - Compile .py files to .mpy first
* ``--heapsize`` - Set heap size for tests
* ``-j, --jobs N`` - Number of tests to run simultaneously

Set the MICROPY_MPYCROSS environment variable to use a specific version of ``mpy-cross`` when using ``--via-mpy``.

**Result Management:**

* ``-r, --result-dir`` - Directory for test results. Default: results/
* ``--print-failures`` - Show diff of failed tests and exit
* ``--clean-failures`` - Delete .exp and .out files from prior failed tests
* ``--run-failures`` - Re-run only previously failed tests

**Examples:**

.. code-block:: bash

   # Run only basic tests with native emitter
   $ ./run-tests.py --emit native -d basics extmod

   # Run tests excluding async functionality
   $ ./run-tests.py -e async

   # Run tests matching *_pep_*
   $ ./run-tests.py -i *_pep_*

   # Run specific test files in parallel
   $ ./run-tests.py -j 4 basics/list*.py

   # Test on connected ESP32 board
   $ ./run-tests.py -t /dev/ttyUSB0
   # or
   $ ./run-tests.py -t u0

   # Re-run only failed tests from previous run
   $ ./run-tests.py --run-failures

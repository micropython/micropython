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

* ``--target`` - Target port (unix, pyboard, esp32, etc.). Default: unix
* ``--device`` - Serial device or IP address for remote targets. Default: /dev/ttyACM0
* ``-b, --baudrate`` - Baud rate for serial communication. Default: 115200

**Test Selection:**

* ``-d, --test-dirs`` - Specify test directories to run
* ``-i, --include REGEX`` - Include tests matching regex pattern
* ``-e, --exclude REGEX`` - Exclude tests matching regex pattern
* ``files`` - Specific test files to run

**Execution Options:**

* ``--emit`` - MicroPython emitter (bytecode or native). Default: bytecode
* ``--via-mpy`` - Compile .py files to .mpy first
* ``--heapsize`` - Set heap size for tests
* ``-j, --jobs N`` - Number of tests to run simultaneously

**Result Management:**

* ``-r, --result-dir`` - Directory for test results. Default: results/
* ``--print-failures`` - Show diff of failed tests and exit
* ``--clean-failures`` - Delete .exp and .out files from failed tests
* ``--run-failures`` - Re-run only previously failed tests

**Examples:**

.. code-block:: bash

   # Run only basic tests with native emitter
   $ ./run-tests.py --emit native -d basics

   # Run tests excluding async functionality
   $ ./run-tests.py -e async

   # Run specific test files in parallel
   $ ./run-tests.py -j 4 basics/list*.py

   # Test on ESP32 board
   $ ./run-tests.py --target esp32 --device /dev/ttyUSB0

   # Re-run only failed tests from previous run
   $ ./run-tests.py --run-failures

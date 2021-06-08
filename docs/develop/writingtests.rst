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

   $ ./run-tests.py --target minimal --device /dev/ttyACM0

And to run only a certain set of tests (eg a directory):

.. code-block:: bash

   $ ./run-tests.py -d basics
   $ ./run-tests.py float/builtin*.py

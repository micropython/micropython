.. _writingtests:

Writing tests
=============

Tests in MicroPython are written in the path ``py/tests``. The following is a listing of
key directories and the run-tests runner script:

.. code-block:: bash
   
   .
    ├── basics
    ├── extmod
    ├── float
    ├── micropython
    ├── run-tests

There are subfolders maintained to categorize most tests. Add a test by creating a new file in one of the
existing folders or in a new folder. It also works to make custom tests outside this tests folder,
which would be recommended for a custom port.

For example, add the following code in a file ``print.py`` in the unix subdirectory:

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

If you create a test under a new subfolder, be sure to update the test script ``run-tests``.

Tests are run by comparing the output of a program by running it against the unix port and CPython.
So your "test" should use print statements to indicate test results.

For tests that can't be compared to CPython (i.e. micropython-specific functionality),
you can provide a ``.exp`` file.

The other way to run tests is:

.. code-block:: bash

   $ cd tests
   $ ./run-tests

Then to run on a board:

.. code-block:: bash

   $ ./run-tests --target minimal --device /dev/ttyACM0

And to run only a certain set of tests (eg a directory):

.. code-block:: bash

   $ ./run-tests -d basics

This directory contains the framework and test files for testing the byte code
output of the Micro Python compiler.

You need to first build the 'cpy' executable in the directory micropython/unix-cpy/.
This executable is a minimal version of Micro Python which compiles a single source
file and outputs the corresponding byte code.

The output of Micro Python is checked against CPython 3.4.

To run the tests use:

    ./run-tests

Note that the tests in pylib-test/ are from the Python 3.3 library, and are licensed
under the relevant license, as per pylib-test/LICENSE.txt.

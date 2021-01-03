Adafruit's CircuitPython Documentation
=========================

The latest documentation can be found at:
http://circuitpython.readthedocs.io/en/latest/

The documentation you see there is generated from the files in the whole tree:
https://github.com/adafruit/circuitpython/tree/main

Building the documentation locally
----------------------------------

If you're making changes to the documentation, you should build the
documentation locally so that you can preview your changes.

Install Sphinx, recommonmark, and optionally (for the RTD-styling), sphinx_rtd_theme,
preferably in a virtualenv:

     pip install sphinx
     pip install recommonmark
     pip install sphinx_rtd_theme

In `circuitpython/`, build the docs:

    make html

You'll find the index page at `_build/html/index.html`.

### More flexibility

Running `make` by itself will list out the multiple doc generating commands available.

All commands will, by default, run with `-E` (forces a rebuild from scratch of docs) and `-v` (verbosity level 1).  This can be customized as desired:

    # will turn OFF the force rebuild
    make html FORCE=

    # will turn OFF the verbosity
    make html VERBOSE=

    # will turn OFF the force rebuild and make it doubly verbose when running
    make html FORCE= VERBOSE="-v -v"

You can also pass other options to sphinx by using `SPHINXOPTS`.

    make html SPHINXOPTS="-T"

For more flexibility and customization, take a look at the Makefile for all variables you can pass in and overwrite.

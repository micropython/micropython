Adafruit's MicroPython Documentation
=========================

The latest documentation can be found at:
http://adafruit-micropython.readthedocs.io/en/latest/

The documentation you see there is generated from the files in the whole tree:
https://github.com/adafruit/micropython/tree/master

Building the documentation locally
----------------------------------

If you're making changes to the documentation, you should build the
documentation locally so that you can preview your changes.

Install Sphinx, and optionally (for the RTD-styling), sphinx_rtd_theme,
preferably in a virtualenv:

     pip install sphinx
     pip install sphinx_rtd_theme

In `micropython/`, build the docs:

    sphinx-build -v -b html . _build/html

You'll find the index page at `micropython/docs/_build/html/index.html`.

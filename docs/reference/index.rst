MicroPython language and implementation
=======================================

MicroPython aims to implement the Python 3.4 standard (with selected
features from later versions) with respect to language syntax, and most
of the features of MicroPython are identical to those described by the
"Language Reference" documentation at
`docs.python.org <https://docs.python.org/3/reference/index.html>`_.

The MicroPython standard library is described in the
:ref:`corresponding chapter <micropython_lib>`. The :ref:`cpython_diffs`
chapter describes differences between MicroPython and CPython (which
mostly concern standard library and types, but also some language-level
features).

This chapter describes features and peculiarities of MicroPython
implementation and the best practices to use them.

.. toctree::
   :maxdepth: 1

   glossary.rst
   repl.rst
   reset_boot.rst
   mpremote.rst
   mpyfiles.rst
   isr_rules.rst
   speed_python.rst
   constrained.rst
   manifest.rst
   packages.rst
   asm_thumb2_index.rst
   filesystem.rst
   pyboard.py.rst
   micropython2_migration.rst

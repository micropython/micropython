|buildstatus|_
|codecov|_

Welcome to Pumbaa's documentation!
==================================

`Pumbaa` is `Python`_ on top of `Simba`_.

The implementation is a port of `MicroPython`_, designed for embedded
devices with limited amount of RAM and code memory.

Project homepage: https://github.com/eerimoq/pumbaa

.. toctree::
   :maxdepth: 1
   :titlesonly:
   :hidden:

   installation
   user-guide
   developer-guide
   boards
   examples
   library-reference

Features
--------

* `MicroPython 3`_ language.

* :doc:`Python Standard Library<library-reference/standard>` modules.

* :doc:`library-reference/micropython` modules.

* A thin :doc:`Python wrapper<library-reference/pumbaa>` for `Simba
  modules`_.

See the :doc:`library-reference` for a full list of features.

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

.. |buildstatus| image:: https://travis-ci.org/eerimoq/pumbaa.svg
.. _buildstatus: https://travis-ci.org/eerimoq/pumbaa

.. |codecov| image:: https://codecov.io/gh/eerimoq/pumbaa/branch/master/graph/badge.svg
.. _codecov: https://codecov.io/gh/eerimoq/pumbaa

.. _Python: https://www.python.org/
.. _Simba: http://simba-os.readthedocs.io/en/latest/
.. _MicroPython: http://www.micropython.org/
.. _MicroPython 3: http://docs.micropython.org/en/latest/pyboard/reference/index.html

.. _Simba modules: http://simba-os.readthedocs.io/en/latest/library-reference.html

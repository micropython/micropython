.. _micropython_internals:

MicroPython Internals
=====================

This chapter covers a tour of MicroPython from the perspective of a developer.
It describes the implementation details to aid developers have a gentle 
contribution curve.

Development around MicroPython usually involves touching the core runtime, porting or 
maintaining a new library. The chapter therefore describes at great depth, the implementation
details of MicroPython from its compiler internals, porting micropython to a new
platform and implemenating a new MicroPython library.

.. toctree::
   :maxdepth: 3

   introduction.rst
   core.rst
   library.rst
   porting.rst

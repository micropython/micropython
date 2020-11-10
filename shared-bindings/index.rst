Core Modules
========================================

These core modules are intended on being consistent across ports and boards.
A module may not exist on a port/board if no underlying hardware support is
present or if flash space is limited. For example, a microcontroller without
analog features will not have `analogio`. See the `support_matrix` page for
a list of modules supported on each board.

.. toctree::
    :hidden:

    support_matrix

Modules
---------

.. toctree::
    :glob:
    :maxdepth: 2

    ../shared-bindings/*/index
    help

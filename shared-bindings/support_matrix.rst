.. _module-support-matrix:

Module Support Matrix - Which Modules Are Available on Which Boards
===================================================================

The following table lists the available built-in modules for each CircuitPython
capable board.

.. list-table::
   :header-rows: 1
   :widths: 7, 50

   * - Board
     - Modules Available

   {% for key, value in support_matrix|dictsort %}
       {{ '.. _' ~ key|replace(" ", "-") ~ ':' }}
   * - {{ key }}
     - {{ '`' ~ value|join("`, `") ~ '`' }}

   {% endfor %}

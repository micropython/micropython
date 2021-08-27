.. _module-support-matrix:

Module Support Matrix - Which Modules Are Available on Which Boards
===================================================================

The following table lists the available built-in modules for each CircuitPython
capable board.

.. raw:: html

    <p id="support-matrix-filter-block"><input placeholder="Filter the boards by available modules" id="support-matrix-filter" type="text"/><span id="support-matrix-filter-num">(all)</span></p>

.. rst-class:: support-matrix-table
.. list-table::
   :header-rows: 1
   :widths: 7, 50

   * - Board
     - Modules Available

   {% for key, value in support_matrix|dictsort %}
       {{ '.. _' ~ key|replace(" ", "-") ~ ':' }}
   * - {{ key }}
     - {{ ':py:mod:`' ~ value|join("`, :py:mod:`") ~ '`' }}

   {% endfor %}

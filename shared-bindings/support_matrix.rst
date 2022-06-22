.. _module-support-matrix:

Module Support Matrix - Which Modules Are Available on Which Boards
===================================================================

The following table lists the available built-in modules for each CircuitPython
capable board, as well as each :term:`frozen module` included on it.

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
     - {{ ':py:mod:`' ~ value[0]|join("`, :py:mod:`") ~ '`' }}

       {% for module in value[1] %}\
       {% if loop.index == 1 %}**Frozen Modules:** {% endif %}\
       {% if loop.index > 1 %}, {% endif %}\
       {% if module[1] %}{{ '`' ~ module[0] ~ ' <' ~ module[1] ~ '>`__' }}\
       {% else %}{{ '`' ~ module[0] ~ ' <#>`__' }}\
       {% endif %}\
       {% endfor %}

   {% endfor %}

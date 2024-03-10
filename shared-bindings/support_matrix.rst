.. _module-support-matrix:

Module Support Matrix - Which Modules Are Available on Which Boards
===================================================================

The following table lists the available built-in modules for each CircuitPython
capable board, as well as each :term:`frozen module` included on it.

You can filter this list by typing one or more module names or partial names into the search box.
Only those boards that provide those modules will be listed.
To exclude boards that provide a module, type a "-" in front of the module name.
You can also type a regular expression as a filter.

.. jinja

.. raw:: html

    <p id="support-matrix-filter-block"><input placeholder="Filter the boards by available modules" id="support-matrix-filter" type="text"/><span id="support-matrix-filter-num">(all)</span></p>

.. raw:: latex

    \begin{landscape}

.. rst-class:: support-matrix-table
.. list-table::
   :header-rows: 1
   :widths: 7, 50

   * - Board
     - Modules Available

   {% for key, value in support_matrix|dictsort %}
   * - {{ key }}

       {{ '.. _' ~ key|replace(" ", "-") ~ ':' }}

     - {{ ':py:mod:`' ~ value.modules|join("`, :py:mod:`") ~ '`' }}

       {% for module in value.frozen_libraries %}\
       {% if loop.index == 1 %}**Frozen Modules:** {% endif %}\
       {% if loop.index > 1 %}, {% endif %}\
       {% if module[1] %}{{ '`' ~ module[0] ~ ' <' ~ module[1] ~ '>`__' }}\
       {% else %}{{ '`' ~ module[0] ~ ' <#>`__' }}\
       {% endif %}\
       {% endfor %}

   {% endfor %}

.. raw:: latex

    \end{landscape}

.. currentmodule:: zephyr

:mod:`zephyr` --- functionality specific to the Zephyr port
===========================================================

.. module:: zephyr
    :synopsis: functionality specific to Zephyr

The ``zephyr`` module contains functions and classes specific to the Zephyr port.

Functions
---------

.. function:: is_preempt_thread()

   Returns true if the current thread is a preemptible thread.

   Zephyr preemptible threads are those with non-negative priority values (low priority levels), which therefore,
   can be supplanted as soon as a higher or equal priority thread becomes ready.

.. function:: current_tid()

   Returns the thread id of the current thread, which is used to reference the thread.

.. function:: thread_analyze(cpu)

   Runs the Zephyr debug thread analyzer on the current thread on the given cpu
   and prints stack size statistics in the format:

    "``thread_name``-20s: STACK: unused ``available_stack_space`` usage ``stack_space_used``
    / ``stack_size`` (``percent_stack_space_used`` %); CPU: ``cpu_utilization`` %"

    * *CPU utilization is only printed if runtime statistics are configured via the ``CONFIG_THREAD_RUNTIME_STATS`` kconfig*

   This function can only be accessed if ``CONFIG_THREAD_ANALYZER`` is configured for the port in ``zephyr/prj.conf``.
   For more information, see documentation for Zephyr `thread analyzer
   <https://docs.zephyrproject.org/latest/guides/debug_tools/thread-analyzer.html#thread-analyzer>`_.

   Note that the ``cpu`` argument is only used in Zephyr v4.0.0 and
   newer and ignored otherwise.

.. function:: shell_exec(cmd_in)

   Executes the given command on an UART backend. This function can only be accessed if ``CONFIG_SHELL_BACKEND_SERIAL``
   is configured for the port in ``zephyr/prj.conf``.

   A list of possible commands can be found in the documentation for Zephyr `shell commands <https://docs.zephyrproject.org/latest/reference/shell/index.html?highlight=shell_execute_cmd#commands>`_.

Classes
-------

.. toctree::
    :maxdepth: 1

    zephyr.DiskAccess.rst
    zephyr.FlashArea.rst

Additional Modules
------------------

.. toctree::
    :maxdepth: 1

    zephyr.zsensor.rst

:mod:`kernel` --- Kernel
========================

.. module:: kernel
   :synopsis: Kernel.

The kernel package is the heart in `Simba`. It implements the thread
scheduler.

Simba documentation: `kernel`_

----------------------------------------------


.. function:: kernel.sys_lock()

   Take the system lock. Turns off interrupts.


.. function:: kernel.sys_unlock()

   Release the system lock. Turns on interrupts.


.. function:: kernel.thrd_yield()

  Put the currently executing thread on the ready list and reschedule.

  This function is often called periodically from low priority work
  heavy threads to give higher priority threads the chance to execute.


.. function:: kernel.thrd_join(thrd)

   Wait for given thread to terminate.


.. function:: kernel.thrd_self()

   Get current thread's id.


.. function:: kernel.thrd_set_name(name)

   Set the name of the current thread to `name`.


.. function:: kernel.thrd_get_name()

   Returns the name of the current thread.


.. function:: kernel.thrd_get_by_name(name)

   Returns the identifier of given thread.


.. function:: kernel.thrd_set_log_mask(thrd, mask)

   Set the log mask of given thread.


.. function:: kernel.thrd_get_log_mask()

   Get the log mask of the current thread.


.. function:: kernel.thrd_set_prio(thrd, prio)

   Set the priority of given thread.


.. function:: kernel.thrd_get_prio()

   Get the priority of the current thread.


.. function:: kernel.thrd_set_global_env(name, value)

   Set the value of given environment variable. The pointers to given
   name and value are stored in the current global environment array.


.. function:: kernel.thrd_get_global_env(name)

   Get the value of given environment variable in the global
   environment array.


.. function:: kernel.thrd_set_env(name, value)

   Set the value of given environment variable. The pointers to given
   name and value are stored in the current threads' environment
   array.


.. function:: kernel.thrd_get_env(name)

   Returns the value of given environment variable. If given variable
   is not found in the current threads' environment array, the global
   environment array is searched. Returns ``None`` if the variable is
   missing.


.. class:: kernel.Timer(timeout, event=None, mask=0x1, callback=None, flags=0)

   Instantiate a timer object from given arguemts. The timer expires
   `timeout` seconds after the timer has been started. When the timer
   expires given `callback` is called from interrupt context and
   `mask` is written to given :class:`event
   channel<sync.sync.Event>`. Set `flags` to :data:`.PERIODIC` to
   create a periodic timer.

   Simba documentation: `kernel/timer`_


   .. method:: start()

      Start the timer.


   .. method:: stop()

      Stop the timer. If the timer is stopped before it has expired it
      will never exipre. This function has no effect if the timer has
      already expired.


   .. data:: PERIODIC

      Pass this flag to :class:`Timer<kernel.kernel.Timer>` for
      periodic timers.


.. _kernel: http://simba-os.readthedocs.io/en/latest/library-reference/kernel.html
.. _kernel/timer: http://simba-os.readthedocs.io/en/latest/library-reference/kernel/timer.html

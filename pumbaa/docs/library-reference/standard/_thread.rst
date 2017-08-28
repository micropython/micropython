:mod:`_thread` --- Low-level threading API
==========================================

.. module:: _thread
   :synopsis: Low-level threading API.

This module provides low-level primitives for working with multiple
threads - multiple threads of control sharing their global data
space. For synchronization, simple locks (also called mutexes or
binary semaphores) are provided.

A thread created by this module is a Simba thread. The classes
``sync.Event`` and ``sync.Queue`` can be used for synchronization in
addition to the simple locks.

----------------------------------------------


.. function:: _thread.start_new_thread(function, args[, kwargs])

   Start a new thread and return its identifier. The thread executes
   the function `function` with the argument list `args` (which must
   be a tuple). The optional `kwargs` argument specifies a dictionary
   of keyword arguments. When the function returns, the thread
   silently exits.


.. function:: _thread.exit()

   Raise the ``SystemExit`` exception. When not caught, this will
   cause the thread to exit silently.


.. function:: _thread.allocate_lock()

   Return a new lock object. Methods of locks are described below. The
   lock is initially unlocked.


.. function:: _thread.get_ident()

   Return the thread identifier of the current thread. This is a
   nonzero integer. Its value has no direct meaning; it is intended as
   a magic cookie to be used e.g. to index a dictionary of
   thread-specific data. Thread identifiers may be recycled when a
   thread exits and another thread is created.


.. function:: _thread.stack_size([size])

   Return the thread stack size used when creating new threads.


.. class:: LockType()


   .. method:: acquire([waitflag])

      Without the optional argument, this method acquires the lock
      unconditionally, if necessary waiting until it is released by
      another thread (only one thread at a time can acquire a lock —
      that’s their reason for existence). If the integer `waitflag`
      argument is present, the action depends on its value: if it is
      zero, the lock is only acquired if it can be acquired
      immediately without waiting, while if it is nonzero, the lock is
      acquired unconditionally as before. The return value is ``True`` if
      the lock is acquired successfully, ``False`` if not.


   .. method:: release()

      Releases the lock. The lock must have been acquired earlier, but
      not necessarily by the same thread.


   .. method:: locked()

      Return the status of the lock: ``True`` if it has been acquired
      by some thread, ``False`` if not.

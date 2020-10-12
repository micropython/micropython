.. _publiccapi:

The public C API
=================

The public C-API comprises functions defined in all ``C`` header files in the ``py``
directory. Most of the important core runtime C APIS are exposed in ``runtime.h`` and
``obj.h``.

The following is an example of public API functions from ``obj.h``:

.. code-block:: c

   mp_obj_t mp_obj_new_tuple(size_t n, const mp_obj_t *items);
   void mp_obj_tuple_get(mp_obj_t self_in, size_t *len, mp_obj_t **items);
   void mp_obj_tuple_del(mp_obj_t self_in);
   mp_int_t mp_obj_tuple_hash(mp_obj_t self_in);

At its core, any functions and macros in header files make up the public
API and can be used to access very low-level details of MicroPython. Static
functions in header files are fine too. Static inline means the function will be
inlined in the code when used. They are "static" so the linker doesn't
complain about duplicate definition in in-lining fails.

Header files in the ``ports`` directory are only exposed to the functionality in
specific to a given port.

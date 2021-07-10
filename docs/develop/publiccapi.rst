.. _publiccapi:

The public C API
================

The public C-API comprises functions defined in all C header files in the ``py/``
directory. Most of the important core runtime C APIs are exposed in ``runtime.h`` and
``obj.h``.

The following is an example of public API functions from ``obj.h``:

.. code-block:: c

   mp_obj_t mp_obj_new_list(size_t n, mp_obj_t *items);
   mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg);
   mp_obj_t mp_obj_list_remove(mp_obj_t self_in, mp_obj_t value);
   void mp_obj_list_get(mp_obj_t self_in, size_t *len, mp_obj_t **items);

At its core, any functions and macros in header files make up the public
API and can be used to access very low-level details of MicroPython. Static
inline functions in header files are fine too, such functions will be
inlined in the code when used.

Header files in the ``ports`` directory are only exposed to the functionality
specific to a given port.

:mod:`uefi.variable` --- NVRAM variables and Variable Policy
============================================================

.. module:: uefi.variable
    :synopsis: read/write/enumerate UEFI variables and register Variable Policy

The ``uefi.variable`` submodule reads, writes and enumerates UEFI firmware
(NVRAM) variables, and -- where the firmware provides it -- registers Variable
Policy constraints.  Variable *names* are Python ``str`` (marshalled to
``CHAR16`` internally), *namespaces* are :class:`~uefi.guid.GUID`, and *data* is
``bytes``.

.. warning::

    ``set`` on a non-volatile variable writes SPI flash.  Some firmware wears out
    or rate-limits on abuse -- fine for a few config keys, wrong as a general
    datastore.  **Never write variables in a loop.**

Read, write, enumerate
----------------------

.. function:: get(name, namespace)

    Return the variable's data as ``bytes``.  Raises :class:`uefi.Error` (with
    ``NOT_FOUND``) if absent.

.. function:: get_with_attributes(name, namespace)

    Return ``(attributes, data_bytes)``.

.. function:: set(name, namespace, data, attributes=NON_VOLATILE | BOOTSERVICE_ACCESS)

    Create or replace a variable.

.. function:: delete(name, namespace)

    Delete a variable (a no-op if it does not exist).

.. function:: exists(name, namespace)

    Return ``True`` if the variable is present.

.. function:: enumerate(namespace=None)

    Return a list of ``(name, GUID)`` for every variable, optionally filtered to
    *namespace*.

    .. note::

        ``GetNextVariableName`` ordering is not stable, and creating or deleting
        variables while enumerating is undefined -- collect the full list before
        mutating the store.

Attribute flags
---------------

.. data:: NON_VOLATILE
.. data:: BOOTSERVICE_ACCESS
.. data:: RUNTIME_ACCESS

    .. note::

        ``RUNTIME_ACCESS`` **without** ``NON_VOLATILE`` is a *volatile* runtime
        variable -- it disappears at reset.

.. data:: HARDWARE_ERROR_RECORD
.. data:: AUTHENTICATED_WRITE_ACCESS

    Deprecated.

.. data:: TIME_BASED_AUTHENTICATED_WRITE_ACCESS
.. data:: APPEND_WRITE

Variable Policy
---------------

:class:`VariablePolicy` wraps the EDK2 ``EDKII_VARIABLE_POLICY_PROTOCOL`` (GUID
``81d1675c-86f6-48df-bd95-9a6e4f0925c3``), which may be absent on some firmware.
It is re-exported at the top level as :class:`uefi.VariablePolicy`.

.. class:: VariablePolicy

    .. staticmethod:: available()

        Return ``True`` if the Variable Policy protocol is present in this
        firmware.

    .. staticmethod:: is_enabled()

        Return ``True`` if the policy engine is enabled.

    .. staticmethod:: register(namespace, name, *, min_size=0, max_size=0xFFFFFFFF, attributes_must=0, attributes_cant=0, lock=LOCK_NONE)

        Register a constraint on a variable (or, with *name* of ``None``, a whole
        *namespace*): minimum/maximum size, must-have / can't-have attribute
        masks, and a lock policy.  *lock* is one of :data:`LOCK_NONE`,
        :data:`LOCK_NOW`, :data:`LOCK_ON_CREATE` or :data:`LOCK_ON_VAR_STATE`
        (the last currently raises ``NotImplementedError``).

    .. staticmethod:: lock_interface()

        Lock the policy engine -- no further policy edits until reset.

Lock-policy constants
---------------------

.. data:: LOCK_NONE
.. data:: LOCK_NOW
.. data:: LOCK_ON_CREATE
.. data:: LOCK_ON_VAR_STATE

Example
-------

::

    from uefi import variable, guid

    # A private, volatile scratch variable (no flash wear)
    ns = uefi.GUID('11111111-2222-3333-4444-555555555555')
    variable.set('scratch', ns, b'hi', attributes=variable.BOOTSERVICE_ACCESS)
    print(variable.get('scratch', ns))       # b'hi'
    variable.delete('scratch', ns)

    if variable.VariablePolicy.available():
        variable.VariablePolicy.register(ns, 'locked', lock=variable.LOCK_NOW)

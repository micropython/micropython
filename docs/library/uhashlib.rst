:mod:`uhashlib` -- hashing algorithm
====================================

.. module:: uhashlib
   :synopsis: hashing algorithm

This module implements binary data hashing algorithms. Currently, it
implements SHA256 algorithm. Choosing SHA256 was a deliberate choice,
as a modern, cryptographically secure algorithm. This means that a
single algorithm can cover both usecases of "any hash algorithm" and
security-related usage, and thus save space omitting legacy algorithms
like MD5 or SHA1.

Constructors
------------

.. class:: uhashlib.sha256([data])

   Create a hasher object and optionally feed ``data`` into it.


Methods
-------

.. method:: sha256.update(data)

   Feed more binary data into hash.

.. method:: sha256.digest()

   Return hash for all data passed thru hash, as a bytes object. After this
   method is called, more data cannot be fed into hash any longer.

.. method:: sha256.hexdigest()

   This method is NOT implemented. Use ``ubinascii.hexlify(sha256.digest())``
   to achieve similar effect.

:mod:`uhashlib` -- hashing algorithms
=====================================

.. module:: uhashlib
   :synopsis: hashing algorithms

This module implements binary data hashing algorithms. The exact inventory
of available algorithms depends on a board. Among the algorithms which may
be implemented:

* SHA256 - The current generation, modern hashing algorithm (of SHA2 series).
  It is suitable for cryptographically-secure purposes. Included in the
  MicroPython core and any board is recommended to provide this, unless
  it has particular code size constraints.

* SHA1 - A previous generation algorithm. Not recommended for new usages,
  but SHA1 is a part of number of Internet standards and existing
  applications, so boards targetting network connectivity and
  interoperatiability will try to provide this.

* MD5 - A legacy algorithm, not considered cryptographically secure. Only
  selected boards, targetting interoperatibility with legacy applications,
  will offer this.

Constructors
------------

.. class:: uhashlib.sha256([data])

    Create an SHA256 hasher object and optionally feed ``data`` into it.

.. class:: uhashlib.sha1([data])

    Create an SHA1 hasher object and optionally feed ``data`` into it.

.. class:: uhashlib.md5([data])

    Create an MD5 hasher object and optionally feed ``data`` into it.

Methods
-------

.. method:: hash.update(data)

   Feed more binary data into hash.

.. method:: hash.digest()

   Return hash for all data passed through hash, as a bytes object. After this
   method is called, more data cannot be fed into the hash any longer.

.. method:: hash.hexdigest()

   This method is NOT implemented. Use ``ubinascii.hexlify(hash.digest())``
   to achieve a similar effect.

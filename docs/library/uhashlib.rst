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

.. only:: port_wipy

    .. class:: uhashlib.sha1([data[, block_size]])
    
       Create a sha1 hasher object and optionally feed ``data`` or ``data and block_size`` into it.

    .. class:: uhashlib.sha256([data[, block_size]])
    
       Create a sha256 hasher object and optionally feed ``data`` or ``data and block_size`` into it.

    .. admonition:: CPython extension
       :class: attention
   
       Due to hardware implementation details of the WiPy, data must be buffered before being
       digested, which would make it impossible to calculate the hash of big blocks of data that
       do not fit in RAM. In this case, since most likely the total size of the data is known
       in advance, the size can be passed to the constructor and hence the HASH hardware engine
       of the WiPy can be properly initialized without needing buffering. If ``block_size`` is
       to be given, an initial chunk of ``data`` must be passed as well. **When using this extension,
       care must be taken to make sure that the length of all intermediate chunks (including the
       initial one) is a multiple of 4 bytes.** The last chunk may be of any length.
      
       Example::
      
           hash = uhashlib.sha1('abcd1234', 1001)    # length of the initial piece is multiple of 4 bytes
           hash.update('1234')                       # also multiple of 4 bytes
           ...
           hash.update('12345')                      # last chunk may be of any length
           hash.digest()

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

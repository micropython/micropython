:mod:`uhashlib` -- hashing algorithm
====================================

.. module:: uhashlib
   :synopsis: hashing algorithm

.. only:: port_pyboard

    This module implements binary data hashing algorithms. Currently, it
    implements SHA256 algorithm. Choosing SHA256 was a deliberate choice,
    as a modern, cryptographically secure algorithm. This means that a
    single algorithm can cover both usecases of "any hash algorithm" and
    security-related usage, and thus save space omitting legacy algorithms
    like MD5 or SHA1.

.. only:: port_wipy

    This module implements binary data hashing algorithms. Currently, it
    implements SHA1 and SHA256 algorithms only. These two algorithms are
    more than enough for today's web applications.


Constructors
------------

.. only:: port_pyboard

    .. class:: uhashlib.sha256([data])
    
       Create a hasher object and optionally feed ``data`` into it.

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
      
           hash = uhashlib.sha1('abcd1234', 1001)    # lenght of the initial piece is multiple of 4 bytes
           hash.update('1234')                       # also multiple of 4 bytes
           ...
           hash.update('12345')                      # last chunk may be of any length
           hash.digest()

Methods
-------

.. method:: hash.update(data)

   Feed more binary data into hash.

.. method:: hash.digest()

   Return hash for all data passed thru hash, as a bytes object. After this
   method is called, more data cannot be fed into hash any longer.

   .. only:: port_wipy
   
        SHA1 hashes are 20-byte long. SHA256 hashes are 32-byte long.

.. method:: hash.hexdigest()

   This method is NOT implemented. Use ``ubinascii.hexlify(hash.digest())``
   to achieve a similar effect.

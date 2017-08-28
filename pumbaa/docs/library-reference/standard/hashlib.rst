:mod:`hashlib` --- Secure hashes and message digests
====================================================

.. module:: hashlib
   :synopsis: Secure hashes and message digests.

This module implements a common interface to many different secure
hash and message digest algorithms. Included are the FIPS secure hash
algorithms SHA256.

----------------------------------------------

There is one constructor method named for each type of hash. All
return a hash object with the same simple interface. For example: use
``sha256()`` to create a SHA-256 hash object. You can now feed this
object with bytes-like objects (normally bytes) using the ``update()``
method. At any point you can ask it for the digest of the
concatenation of the data fed to it so far using the ``digest()`` or
``hexdigest()`` methods.

   .. code-block:: python
                
      >>> import hashlib
      >>> m = hashlib.sha256()
      >>> m.update(b"Nobody inspects")
      >>> m.update(b" the spammish repetition")
      >>> m.digest()
      b'\x03\x1e\xdd}Ae\x15\x93\xc5\xfe\\\x00o\xa5u+7\xfd\xdf\xf7\xbcN\x84:\xa6\xaf\x0c\x95\x0fK\x94\x06'

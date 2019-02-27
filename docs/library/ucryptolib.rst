:mod:`ucryptolib` -- cryptographic ciphers
==========================================

.. module:: ucryptolib
   :synopsis: cryptographic ciphers

Classes
-------

.. class:: aes

    .. classmethod:: __init__(key, mode, [IV])

        Initialize cipher object, suitable for encryption/decryption. Note:
        after initialization, cipher object can be use only either for
        encryption or decryption. Running decrypt() operation after encrypt()
        or vice versa is not supported.

        Parameters are:

            * *key* is an encryption/decryption key (bytes-like).
            * *mode* is:

                * ``1`` (or ``ucryptolib.MODE_ECB`` if it exists) for Electronic Code Book (ECB).
                * ``2`` (or ``ucryptolib.MODE_CBC`` if it exists) for Cipher Block Chaining (CBC)

            * *IV* is an initialization vector for CBC mode.

    .. method:: encrypt(in_buf, [out_buf])

        Encrypt *in_buf*. If no *out_buf* is given result is returned as a
        newly allocated `bytes` object. Otherwise, result is written into
        mutable buffer *out_buf*. *in_buf* and *out_buf* can also refer
        to the same mutable buffer, in which case data is encrypted in-place.

    .. method:: decrypt(in_buf, [out_buf])

        Like `encrypt()`, but for decryption.

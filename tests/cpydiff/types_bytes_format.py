"""
categories: Types,bytes
description: bytes objects support .format() method
cause: MicroPython strives to be a more regular implementation, so if both `str` and `bytes` support ``__mod__()`` (the % operator), it makes sense to support ``format()`` for both too. Support for ``__mod__`` can also be compiled out, which leaves only ``format()`` for bytes formatting.
workaround: If you are interested in CPython compatibility, don't use ``.format()`` on bytes objects.
"""
print(b"{}".format(1))

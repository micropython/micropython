"""
categories: Modules,errno
description: Micropython does not include ``ENOTSUP`` as a name for errno 95.
cause: Micropython does not implement the ``ENOTSUP`` canonical alias for ``EOPNOTSUPP`` added in CPython 3.2.
workaround: Use ``errno.EOPNOTSUPP`` in place of ``errno.ENOTSUP``.
"""

import errno

print(f"{errno.errorcode[errno.EOPNOTSUPP]=!s}")

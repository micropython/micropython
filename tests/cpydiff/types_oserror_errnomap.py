"""
categories: Types,OSError
description: OSError constructor returns a plain OSError for all errno values, rather than a relevant subtype.
cause: MicroPython does not include the CPython-standard OSError subclasses.
workaround: Catch OSError and use its errno attribute to discriminate the cause.
"""

import errno

errno_list = [  # i.e. the set implemented by micropython
    errno.EPERM,
    errno.ENOENT,
    errno.EIO,
    errno.EBADF,
    errno.EAGAIN,
    errno.ENOMEM,
    errno.EACCES,
    errno.EEXIST,
    errno.ENODEV,
    errno.EISDIR,
    errno.EINVAL,
    errno.EOPNOTSUPP,
    errno.EADDRINUSE,
    errno.ECONNABORTED,
    errno.ECONNRESET,
    errno.ENOBUFS,
    errno.ENOTCONN,
    errno.ETIMEDOUT,
    errno.ECONNREFUSED,
    errno.EHOSTUNREACH,
    errno.EALREADY,
    errno.EINPROGRESS,
]


def errno_output_type(n):
    try:
        raise OSError(n, "")
    except OSError as e:
        return f"{type(e).__name__}"
    except Exception as e:
        return f"non-OSError {type(e).__name__}"
    else:
        return "no error"


for n in errno_list:
    print(errno.errorcode[n], "=", errno_output_type(n))

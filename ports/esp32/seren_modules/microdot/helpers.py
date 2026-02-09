try:
    from functools import wraps
except ImportError:  # pragma: no cover
    # MicroPython does not currently implement functools.wraps
    def wraps(wrapped):
        def _(wrapper):
            return wrapper
        return _

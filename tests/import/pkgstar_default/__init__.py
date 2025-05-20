# When __all__ is undefined, star import should only
# show objects that do not start with an underscore


def visibleFun():
    return 42


class VisibleClass:
    def __init__(self):
        self._val = 42


def _hiddenFun():
    return -1


class _HiddenClass:
    def __init__(self):
        self._val = -1

__all__ = ["publicFun", "PublicClass", "dynamicFun"]


# Definitions below should always be imported by a star import
def publicFun():
    return 1


class PublicClass:
    def __init__(self):
        self._val = 1


# If __all__ support is enabled, definitions below
# should not be imported by a star import
def unlistedFun():
    return 0


class UnlistedClass:
    def __init__(self):
        self._val = 0


# Definitions below should be not be imported by a star import
# (they start with an underscore, and are not listed in __all__)
def _privateFun():
    return -1


class _PrivateClass:
    def __init__(self):
        self._val = -1


# Test lazy loaded function, as used by extmod/asyncio:
# Works with a star import only if __all__ support is enabled
_attrs = {
    "dynamicFun": "funcs",
}


def __getattr__(attr):
    mod = _attrs.get(attr, None)
    if mod is None:
        raise AttributeError(attr)
    value = getattr(__import__(mod, globals(), locals(), True, 1), attr)
    globals()[attr] = value
    return value

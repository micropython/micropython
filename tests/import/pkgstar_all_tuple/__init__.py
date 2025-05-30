__all__ = ('publicFun2', 'PublicClass2')


# Definitions below should always be imported by a star import
def publicFun2():
    return 2


class PublicClass2:
    def __init__(self):
        self._val = 2


# If __all__ support is enabled, definitions below
# should not be imported by a star import
def unlistedFun2():
    return 0


class UnlistedClass2:
    def __init__(self):
        self._val = 0

class ABCMeta:
    pass


#class ABC(metaclass=ABCMeta):
class ABC:
    pass


def abstractmethod(f):
    return f

# enum.py
# Enum implementation without metaclasses
# version="1.5.1"

# ==============================================================================
# Variable & Abbreviation Definitions:
# ==============================================================================
# Functions & Helper Methods:
#   _c(cls, n, v, t) -> create_enum_item: Helper function to construct a typed enum instance.
#   _i()        -> items: Classmethod to initialize and return the __members__ dict.
#   a(s, k, v)  -> __setattr__: Inner setter guard raising AttributeError on enum items.
#
# Helper Arguments & Local Variables:
#   e       - enum_name / enum_item: Class name string or instance of an enum item.
#   n       - name: String representing the enum member key (e.g., "RED").
#   v       - value / member value: Value assigned to, searched within, or passed to lookup.
#   c       - class / new_class: Dynamically created Enum subclass via `type()`.
#   t       - value_type: Target constraint type (`int`, `str`, or `None` for generic Enum).
#   l       - names_list: List of parsed string keys from comma/space-separated inputs.
#   d       - dict / mapping: Temporary dictionary for parsed enum members.
#   k, v    - key, value: Key-value pair during iteration over attributes or dictionaries.
#   i       - item: Individual enum member instance during iteration.
#   __members__ - Dictionary mapping enum member instances to their raw values.
#   s, k, o - self, key/attribute_name, other_enum (standard short parameters).
#   f       - function: The body function of the class being defined.
#   b       - bases: Tuple of base classes passed during class creation.
#   kw      - kwargs: Additional keyword arguments passed during class creation.
#   cls     - class object: Target class being created or initialized.
# ==============================================================================


def _n(n):
    # str.capitalize()
    return n[0].upper() + n[1:]


def _c(cls, n, v, t=None):
    # prevent modification of enum item attributes
    def a(s, k, v):
        raise AttributeError("cannot set attribute")

    # IntEnum / StrEnum type checks
    if t and not isinstance(v, t):
        raise TypeError(f"{_n(t.__name__)}Enum {n!r} must be {t.__name__}")

    # extract a printable class name (works for both classes and objects)
    e = cls.__name__

    # create a subclass inheriting the value's type (int, str, float etc.)
    return type(
        f"{e}.{n}",
        (type(v), cls),
        {
            "name": n,
            "value": v,
            "__str__": lambda s: str(v) if t else f"{e}.{n}",
            "__repr__": lambda s: f"<{e}.{n}: {v!r}>",
            "__call__": lambda s: v,
            "__setattr__": a,
        },
    )(v)


class Enum:
    _v_t = None  # expected value type (int, str, or None)

    def __new__(cls, value=None, names=None, *, start=1):
        # functional API: dynamic creation of a new Enum class
        if value is not None and names is not None:
            is_str = cls._v_t is str

            # parse 'names' parameter into a key-value dictionary
            if isinstance(names, dict):
                d = names
            else:
                s = isinstance(names, str)
                # space or comma-separated string of names, or any other iterable
                names = names.replace(",", " ").split() if s else list(names)
                if names and isinstance(names[0], (list, tuple)):
                    # list/tuple of (name, value) pairs
                    d = dict(names)
                elif is_str:
                    d = {k: k.lower() if s else k for k in names}
                else:
                    d = {k: v for v, k in enumerate(names, start=start)}

            # build and return a new Enum subclass
            c = type(str(value), (cls,), d)
            c._i()
            return c

        # lookup by value or name: Color(1) / Color("RED")
        if value is not None and cls not in (Enum, IntEnum, StrEnum):
            return cls.__call__(value)

        return super().__new__(cls)

    @classmethod
    def __contains__(cls, v):
        # lookup by value or name
        return any(i.value == v or i.name == v for i in cls._i())

    @classmethod
    def __call__(cls, v=None):
        if v is None:
            return cls()

        # lookup by value or name
        for i in cls._i():
            if i.value == v or i.name == v:
                return i
        raise ValueError(f"{v!r} not in {cls.__name__}")

    @classmethod
    def __getitem__(cls, k):
        # instance-level name lookup: Color["RED"]
        for i in cls._i():
            if i.name == k:
                return i
        raise KeyError(k)

    # True if both classes are Enums and the member dicts match
    __eq__ = classmethod(lambda cls, o: getattr(o, "_i", False) and cls._i() == o._i())
    __iter__ = classmethod(lambda cls: iter(cls._i()))
    __len__ = classmethod(lambda cls: len(cls._i()))
    __str__ = __repr__ = classmethod(lambda cls: f"<enum '{type(cls).__name__}'>")

    @classmethod
    def __setattr__(cls, k, v):
        raise AttributeError("cannot set attribute")

    @classmethod
    def __delattr__(cls, k):
        raise AttributeError("cannot delete attribute")

    @classmethod
    def dump(cls):
        # serialize the class into a string that can be converted back into a class using eval()
        # cls == eval(cls.dump())()
        d = {i.name: i.value for i in cls._i()}
        return f"{cls._v_t and _n(cls._v_t.__name__) or ''}Enum('{cls.__name__}', {d})"

    @classmethod
    def _i(cls):
        # build and return the dict that maps member instances to their values
        if "__members__" not in cls.__dict__:
            # convert raw class attributes into typed enum instances
            cls.__members__ = {}
            for k, v in list(cls.__dict__.items()):
                if not k.startswith("_") and not callable(v):
                    if v in cls.__members__:
                        raise ValueError(f"duplicate value {v!r} in {cls.__name__}")
                    e = _c(cls, k, v, cls._v_t)
                    setattr(cls, k, e)
                    cls.__members__[e] = v
        return cls.__members__


class IntEnum(Enum):
    _v_t = int


class StrEnum(Enum):
    _v_t = str


# install class-builder hook
import builtins

_orig = builtins.__build_class__


# def _build_class(function, name, *bases, **kwargs):
def _bc(f, n, *b, **kw):
    # when a base is an Enum instance (returned by cls()), extract its class
    b = tuple(type(e) if isinstance(e, Enum) else e for e in b)
    cls = _orig(f, n, *b, **kw)
    if issubclass(cls, Enum):
        cls._i()
        return cls()
    return cls


builtins.__build_class__ = _bc

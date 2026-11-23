# enum.py
# Enum implementation without metaclasses
# version="1.5.2"

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
#   __members__ - Dictionary mapping enum member names to member instances.
#   s, k, o - self, key/attribute_name, other_enum (standard short parameters).
#   f       - function: The body function of the class being defined.
#   b       - bases: Tuple of base classes passed during class creation.
#   kw      - kwargs: Additional keyword arguments passed during class creation.
#   cls     - class object: Target class being created or initialized.
# ==============================================================================


def _n(n):
    # like str.capitalize()
    return n[0].upper() + n[1:]

def _c(cls, n, v, t=None):
    # prevent modification of enum item attributes
    def a(s, k, v=None):
        raise AttributeError("cannot set attribute")

    # IntEnum / StrEnum type checks
    if t and not isinstance(v, t):
        raise TypeError(f"{_n(t.__name__)}Enum {n!r} must be {t.__name__}")

    if t is not None:
        c = cls(v)
        c._i()
        assert not (type(c) is type)  # not (c is class)
        assert isinstance(c, Enum)
        assert isinstance(c, t)
        if t is str:
            assert isinstance(c, StrEnum)
        if t is int:
            assert isinstance(c, IntEnum)
    else:
        c = object.__new__(cls)
        assert isinstance(c, Enum)

    c.name = n
    c.value = v
    c.__setattr__ = a
    return c


class EnumMeta:
    __len__ = len = classmethod(lambda cls: len(cls._i()))
    __str__ = classmethod(lambda cls: f"<enum '{cls.__name__}'>")
    __repr__ = classmethod(lambda cls: f"<enum '{cls.__name__}'>")

#    __eq__ = classmethod(lambda cls, o: cls is o or (isinstance(o, type) and issubclass(o, Enum) and cls._i() == o._i()))
    __eq__ = classmethod(
        lambda cls, o: (
            cls is o
            or (isinstance(o, Enum) and type(o) is cls)
            or (isinstance(o, type) and issubclass(o, Enum) and cls._i() == o._i())
        )
    )

    @classmethod
    def __delattr__(cls, k):
        raise AttributeError("cannot delete attribute")

    @classmethod
    def __getitem__(cls, k):
        if isinstance(k, int) and not isinstance(k, bool):
            members = list(cls._i().values())
            return members[k]
        try:
            return cls._i()[k]
        except KeyError:
            raise KeyError(k)

    def __call__(self, value=None):
        if value is None:
            return getattr(self, "value", None)
        return type(self)._lookup_member(value)

class Enum(EnumMeta):
    _v_t = None  # expected value type (int, str, or None)

    def __new__(cls, value=None, names=None, *, start=1):
        # Functional API: dynamic creation of a new Enum class
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
            d["__name__"] = value
            c = type(str(value), (cls,), d)
            c._i()
            return c

        # lookup by value or name: Color(1) / Color("RED")
        if value is not None and cls not in (Enum, IntEnum, StrEnum):
            return cls._lookup_member(value)

        return super().__new__(cls)

    @classmethod
    def __contains__(cls, v):
        # lookup by value or name
        return any(e is v or e.value == v or e.name == v for e in cls._i().values())

    @classmethod
    def _lookup_member(cls, value):
        if isinstance(value, cls):
            return value

        members = cls._i()
        if isinstance(value, str) and value in members:
            return members[value]

        for member in members.values():
            if member.value == value:
                return member

        raise ValueError(f"{value!r} is not a valid {cls.__name__}")

    def __str__(self):
        if not hasattr(self, "name"):
            return f"<enum '{type(self).__name__}'>"
        return f"{type(self).__name__}.{self.name}"

    def __repr__(self):
        return f"<{type(self).__name__}.{getattr(self, 'name', None)}: {getattr(self, 'value', None)!r}>"

    def __eq__(self, o):
        if isinstance(o, Enum):
            return self is o or (type(self).__name__ == type(o).__name__ and self.name == o.name and self.value == o.value)
        return hasattr(self, "value") and self.value == o

    @classmethod
    def __iter__(cls):
        return iter(cls._i().values())

    @classmethod
    def dump(cls):
        d = {i.name: i.value for i in cls._i().values()}
        prefix = _n(cls._v_t.__name__) if cls._v_t else ""
        return f"{prefix}Enum({cls.__name__!r}, {d!r})"

    @classmethod
    def _i(cls):
        # build and return the dict that maps member instances to their values
        if "__members__" not in cls.__dict__:
            for b in cls.__bases__:
                if issubclass(b, Enum) and b is not Enum and b._i():
                    raise TypeError(f"can't extend {b.__name__}")

            # convert raw class attributes into typed enum instances
            cls.__members__ = {}
            for k, v in list(cls.__dict__.items()):
                if k.startswith("_") or callable(v) or isinstance(v, (staticmethod, classmethod, property)):
                    continue

                if any(m.value == v for m in cls.__members__.values()):
                    raise ValueError(f"duplicate value {v!r} in {cls.__name__}")

                e = _c(cls, k, v, cls._v_t)
                setattr(cls, k, e)
                cls.__members__[k] = e
        return cls.__members__


class IntEnum(int, Enum):
    _v_t = int

    def __eq__(self, o):
        if isinstance(o, Enum):
            return self is o or (type(self) is type(o) and self.name == o.name and self.value == o.value)
        if isinstance(o, int):
            return int(self) == o
        return False

    def __iter__(self):
        return iter(type(self)._i().values())


class StrEnum(str, Enum):
    _v_t = str

    def __eq__(self, o):
        if isinstance(o, Enum):
            return self is o or (type(self) is type(o) and self.name == o.name and self.value == o.value)
        if isinstance(o, str):
            return str(self) == o
        return False

    def __iter__(self):
        return iter(type(self)._i().values())

    def __repr__(self):
        return f"<{type(self).__name__}.{getattr(self, 'name', None)}: {getattr(self, 'value', None)!r}>"

# install class-builder hook
import builtins

_orig = builtins.__build_class__


# def _build_class(function, name, *bases, **kwargs):
def _bc(f, n, *b, **kw):
    # when a base is an Enum instance (returned by cls()), extract its class
    b = tuple(type(e) if isinstance(e, Enum) else e for e in b)
    cls = _orig(f, n, *b, **kw)
    if issubclass(cls, Enum) and cls is not Enum:
        cls._i()
    return cls


builtins.__build_class__ = _bc

# enum_mini.py
# Minimalist Enum implementation without metaclasses
# version="1.4.0"

# ==============================================================================
# Variable & Abbreviation Definitions:
# ==============================================================================
# Functions & Helper Methods:
#   _c(e, n, v) -> create_enum_item: Helper function to construct a typed enum instance.
#   _i()        -> items: Classmethod to lazily initialize and return the __members__ dict.
#   a(s, k, v)  -> __setattr__: Inner setter guard raising AttributeError on enum items.
#
# Helper Arguments & Local Variables:
#   e       - enum_name / enum_item: Class name string or instance of an enum item.
#   n       - name: String representing the enum member key (e.g., "RED").
#   v       - value / member value: Value assigned to or searched within the enum member.
#   c       - class / new_class: Dynamically created Enum subclass via `type()`.
#   k, v    - key, value: Key-value pair during iteration over attributes or dictionaries.
#   i       - item: Individual enum member instance during iteration.
#   __members__ - Dictionary mapping enum instances to values.
#   s, k, o - self, key/attribute_name, other_enum (standard short parameters).
# ==============================================================================


def _c(e, n, v):
    # Inner guard to prevent modification of enum item attributes
    def a(s, k, v):
        raise AttributeError("readonly")

    # Safely extract class name string if a class object or instance was passed
    e = getattr(e, "__name__", str(e))

    # Dynamically create a subclass inheriting from type(v) (e.g., int, str, float)
    return type(
        f"{e}.{n}",
        (type(v),),
        {
            "name": n,
            "value": v,
            "__str__": lambda s: f"{e}.{n}",
            "__repr__": lambda s: f"<{e}.{n}: {v!r}>",
            "__call__": lambda s: v,
            "__setattr__": a,
        },
    )(v)


class Enum:
    def __new__(cls, value=None, names=None):
        # Functional API: Enum('Name', {'KEY': 'val'})
        if value is not None and names is not None:
            if not isinstance(names, dict):
                raise TypeError("names must be dict")

            # Construct and return a new Enum subclass
            c = type(str(value), (cls,), {"__members__": {}})
            for k, v in names.items():
                e = _c(value, k, v)
                setattr(c, k, e)
                c.__members__[e] = v
            return c

        if cls is not Enum:
            # Lazy initialization of subclass members
            cls._i()

            # Lookup existing member by value or name using call: e.g., Color(1) or Color("RED")
            if value is not None:
                return cls()(value)

        return super().__new__(cls)

    def __contains__(cls, v):
        # Lookup member by value or name
        for i in cls._i():
            if i.value == v or i.name == v:
                return True
        return False

    @classmethod
    @classmethod
    def __call__(cls, v):
        # Lookup member by value or name
        for i in cls._i():
            if i.value == v or i.name == v:
                return i
        raise ValueError(f"invalid: {v!r}")

    # Equality checks if both classes are Enums and have identical __members__ dicts
    # __eq__ = classmethod(lambda cls, o: isinstance(o, type) and issubclass(o, Enum) and cls._i() == o._i())
    __eq__ = classmethod(lambda cls, o: getattr(o, "_i", None) and cls._i() == o._i())
    # Iteration yields enum member instances (keys of __members__)
    __iter__ = classmethod(lambda cls: iter(cls._i()))
    __len__ = classmethod(lambda cls: len(cls._i()))
    __str__ = __repr__ = classmethod(lambda cls: f"<enum '{type(cls).__name__}'>")

    @classmethod
    def __setattr__(cls, k, v):
        raise AttributeError("readonly")

    @classmethod
    def __delattr__(cls, k):
        raise AttributeError("readonly")

    @classmethod
    def _i(cls):
        # Initialize and return dictionary mapping enum instances to their values
        if "__members__" not in cls.__dict__:
            # Convert raw class attributes into typed enum instances
            cls.__members__ = {}
            for k, v in list(cls.__dict__.items()):
                if not k.startswith("_") and not callable(v):
                    e = _c(cls.__name__, k, v)
                    setattr(cls, k, e)
                    cls.__members__[e] = v
        return cls.__members__

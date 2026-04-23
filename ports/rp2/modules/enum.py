# enum.py
# Enum implementation without metaclasses
# version="1.5.0"

# ==============================================================================
# Variable & Abbreviation Definitions:
# ==============================================================================
# Functions & Helper Methods:
#   _c(e, n, v, t) -> create_enum_item: Helper function to construct a typed enum instance.
#   _i()        -> items: Classmethod to lazily initialize and return the __members__ dict.
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


def _c(e, n, v, t=None):
    # Inner guard to prevent modification of enum item attributes
    def a(s, k, v):
        raise AttributeError("cannot set attribute")

    # Type checks for IntEnum and StrEnum
    if t is int and not isinstance(v, int):
        raise TypeError(f"IntEnum member {n!r} value must be int, got {type(v).__name__}")
    elif t is str and not isinstance(v, str):
        raise TypeError(f"StrEnum member {n!r} value must be str, got {type(v).__name__}")

    # Safely extract class name string if a class or object was passed
    e = getattr(e, "__name__", str(e))

    # Create dynamic subclass inheriting the value's type (int, str, float etc.)
    return type(
        f"{e}.{n}",
        (type(v),),
        {
            "name": n,
            "value": v,
            "__str__": lambda s: str(v) if t in (int, str) else f"{e}.{n}",
            "__repr__": lambda s: f"<{e}.{n}: {v!r}>",
            "__call__": lambda s: v,
            "__setattr__": a,
        },
    )(v)


class Enum:
    _v_t = None  # Expected value type constraint (int, str, or None)

    def __new__(cls, value=None, names=None, *, start=1):
        # Functional API: dynamic creation of a new Enum class
        if value is not None and names is not None:
            is_str = cls._v_t is str

            # Parse 'names' parameter into a key-value dictionary
            if isinstance(names, dict):
                d = names
            elif isinstance(names, str):
                # Space or comma-separated string of member names
                l = names.replace(",", " ").split()
                d = (
                    {k: k.lower() for k in l}
                    if is_str
                    else {k: v for v, k in enumerate(l, start=start)}
                )
            elif isinstance(names, (list, tuple)):
                # List/tuple of strings or (name, value) pairs
                if names and isinstance(names[0], (list, tuple)):
                    d = dict(names)
                else:
                    d = (
                        {k: k for k in names}
                        if is_str
                        else {k: v for v, k in enumerate(names, start=start)}
                    )
            else:
                # Other iterables (e.g., sets)
                d = (
                    {k: k for k in names}
                    if is_str
                    else {k: v for v, k in enumerate(names, start=start)}
                )

            # Construct and return a new Enum subclass
            c = type(str(value), (cls,), {})
            for k, v in d.items():
                setattr(c, k, v)
            c._i()
            return c

        if cls not in (Enum, IntEnum, StrEnum):
            # Lazy initialization of subclass members
            cls._i()

            # Lookup existing member by value or name: e.g., Color(1) or Color("RED")
            if value is not None:
                return cls()(value)

        return super().__new__(cls)

    @classmethod
    def __contains__(cls, v):
        # Lookup member by value or name
        for i in cls._i():
            if i.value == v or i.name == v:
                return True
        return False

    @classmethod
    def __call__(cls, v=None):
        if v is None:
            return cls()

        # Lookup member by value or name
        for i in cls._i():
            if i.value == v or i.name == v:
                return i
        raise ValueError(f"{v!r} is not a valid {cls.__name__}")

    @classmethod
    def __getitem__(cls, k):
        # Instance-level container lookup: Color["RED"]
        for i in cls._i():
            if i.name == k:
                return i
        raise KeyError(k)

    # Equality checks if both classes are Enums and have identical __members__ dicts
    __eq__ = classmethod(lambda cls, o: getattr(o, "_i", False) and cls._i() == o._i())
    # Iteration yields enum member instances (keys of __members__)
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
        # Serialize enum members to string representation for eval compatibility
        # cls == eval(cls.dump())
        if cls._v_t is None:
            e = "Enum"
        else:
            e = cls._v_t.__name__[0].upper() + cls._v_t.__name__[1:] + "Enum"
        d = {i.name: i.value for i in cls._i()}
        return f"{e}('{cls.__name__}', {d})"

    @classmethod
    def _i(cls):
        # Initialize and return dictionary mapping enum member instances to their values
        if "__members__" not in cls.__dict__:
            # Convert raw class attributes into typed enum instances
            cls.__members__ = {}
            for k, v in list(cls.__dict__.items()):
                if not k.startswith("_") and not callable(v):
                    if isinstance(v, cls):
                        continue
                    if v in cls.__members__:
                        raise ValueError(f"duplicate value {v!r} in {cls.__name__}")
                    e = _c(cls.__name__, k, v, cls._v_t)
                    setattr(cls, k, e)
                    cls.__members__[e] = v
        return cls.__members__


class IntEnum(Enum):
    _v_t = int


class StrEnum(Enum):
    _v_t = str


# Install builder hook
import builtins

_SN = "__ENUM_STATE__"  # _STATE_NAME
if getattr(builtins, _SN, None):
    builtins.__ENUM_STATE__["base"] = Enum
else:
    _orig = builtins.__build_class__

    # def _build_class(f, n, *b, **kw):
    def _bc(f, n, *b, **kw):
        cls = _orig(f, n, *b, **kw)
        return (
            cls() if any(isinstance(base, type) and issubclass(base, Enum) for base in b) else cls
        )

    builtins.__build_class__ = _bc
    setattr(builtins, _SN, {"original": _orig, "base": Enum, "wrapper": _bc})

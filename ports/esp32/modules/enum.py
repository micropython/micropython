# enum.py
# version="1.3.0"


def _make_enum(v, n, e):
    T = type(v)

    def _setattr(self, k, v):
        raise AttributeError("EnumValue is immutable")

    # Create class: type(name, bases, dict), which inherits a base type (int, str, etc.)
    return type(
        "EnumValue",
        (T,),
        {
            "name": n,
            "value": property(lambda s: v),
            "__repr__": lambda s: f"{e}.{n}: {v}",
            "__str__": lambda s: f"{e}.{n}: {v}",
            "__call__": lambda s: v,
            "__setattr__": _setattr,
        },
    )(v)


class Enum:
    def __new__(cls, name=None, names=None):
        # If a name and names are provided, create a NEW subclass of Enum
        if name and names:
            # Support Functional API: Enum("Name", {"KEY1": VALUE1, "KEY2": VALUE2, ..})
            # Dynamically create: class <name>
            new_cls = type(name, (cls,), {"_inited": True})
            for k, v in names.items():
                setattr(new_cls, k, _make_enum(v, k, name))
            return super().__new__(new_cls)

        # Reverse lookup by value or name (e.g., Color(1) or Color("RED"))
        if name and cls is not Enum:
            return cls._lookup(name)

        return super().__new__(cls)

    def __init__(self, name=None, names=None):
        if "_inited" not in self.__class__.__dict__:
            self.list()

    @classmethod
    def _lookup(cls, v):
        for m in cls.list():
            if m.value == v or m.name == v:
                return m
        raise AttributeError(f"{v} is not in {cls.__name__}")

    @classmethod
    def __iter__(cls):
        return iter(cls.list())

    @classmethod
    def list(cls):
        if "_inited" not in cls.__dict__:
            # Copy dict.items() to avoid RuntimeError when changing the dictionary
            for k, v in list(cls.__dict__.items()):
                if not k.startswith("_") and not callable(v):
                    setattr(cls, k, _make_enum(v, k, cls.__name__))
            cls._inited = True
        return [
            m for k in dir(cls) if not k.startswith("_") and hasattr(m := getattr(cls, k), "name")
        ]

    @classmethod
    def is_value(cls, v):
        return any(m.value == v or m.name == v for m in cls.list())

    def __repr__(self):
        # Supports the condition: obj == eval(repr(obj))
        d = {m.name: m.value for m in self.__class__.list()}
        # Return a string like: Enum(name='Name', names={'KEY1': VALUE1, 'KEY2': VALUE2, ..})
        return f"Enum(name='{self.__class__.__name__}', names={d})"

    def __call__(self, v):
        return self._lookup(v)

    def __setattr__(self, k, v):
        if "_inited" in self.__class__.__dict__:
            raise AttributeError(f"Enum '{self.__class__.__name__}' is immutable")
        super().__setattr__(k, v)

    def __delattr__(self, k):
        raise AttributeError("Enum is immutable")

    @classmethod
    def __len__(cls):
        return len(cls.list())

    def __eq__(self, o):
        if not isinstance(o, Enum):
            return False
        return self.list() == o.list()

# enum.py
# version="1.2.6"


class EnumValue:
    # An immutable object representing a specific enum member
    def __init__(self, v, n):
        object.__setattr__(self, "value", v)
        object.__setattr__(self, "name", n)

    def __repr__(self):
        return f"{self.name}: {self.value}"

    def __call__(self):
        return self.value

    def __setattr__(self, k, v):
        raise AttributeError("EnumValue is immutable")

    # Helper function to extract the raw value
    def _get_value(self, o):
        return o.value if isinstance(o, EnumValue) else o

    # Arithmetic and Bitwise operations (Forward)
    def __add__(self, o):
        return self.value + self._get_value(o)

    def __sub__(self, o):
        return self.value - self._get_value(o)

    def __mul__(self, o):
        return self.value * self._get_value(o)

    def __truediv__(self, o):
        return self.value / self._get_value(o)

    def __floordiv__(self, o):
        return self.value // self._get_value(o)

    def __mod__(self, o):
        return self.value % self._get_value(o)

    def __pow__(self, o):
        return self.value ** self._get_value(o)

    def __and__(self, o):
        return self.value & self._get_value(o)

    def __or__(self, o):
        return self.value | self._get_value(o)

    def __xor__(self, o):
        return self.value ^ self._get_value(o)

    def __lshift__(self, o):
        return self.value << self._get_value(o)

    def __rshift__(self, o):
        return self.value >> self._get_value(o)

    # Arithmetic and Bitwise operations (Reflected)
    def __radd__(self, o):
        return self._get_value(o) + self.value

    def __rsub__(self, o):
        return self._get_value(o) - self.value

    def __rmul__(self, o):
        return self._get_value(o) * self.value

    def __rtruediv__(self, o):
        return self._get_value(o) / self.value

    def __rfloordiv__(self, o):
        return self._get_value(o) // self.value

    def __rand__(self, o):
        return self._get_value(o) & self.value

    def __ror__(self, o):
        return self._get_value(o) | self.value

    def __rxor__(self, o):
        return self._get_value(o) ^ self.value

    def __rlshift__(self, o):
        return self._get_value(o) << self.value

    def __rrshift__(self, o):
        return self._get_value(o) >> self.value

    # Unary operators
    def __neg__(self):
        return -self.value

    def __pos__(self):
        return +self.value

    def __abs__(self):
        return abs(self.value)

    def __invert__(self):
        return ~self.value

    # Comparison
    def __eq__(self, o):
        return self.value == self._get_value(o)

    def __lt__(self, o):
        return self.value < self._get_value(o)

    def __le__(self, o):
        return self.value <= self._get_value(o)

    def __gt__(self, o):
        return self.value > self._get_value(o)

    def __ge__(self, o):
        return self.value >= self._get_value(o)

    def __ne__(self, o):
        return self.value != self._get_value(o)


class Enum:
    def __new__(cls, name=None, names=None):
        # If a name and names are provided, create a NEW subclass of Enum
        if name and names:
            # Support Functional API: Enum("Name", {"KEY": VALUE})
            # Dynamically create: class <name>
            new_cls = type(name, (cls,), {})
            for k, v in names.items():
                new_cls._up(k, v)
            new_cls._inited = True
            return super().__new__(new_cls)

        # Reverse lookup by value or name (e.g., Color(1) or Color("RED"))
        if name and not names and cls is not Enum:
            return cls._lookup(name)

        return super().__new__(cls)

    def __init__(self, name=None, names=None):
        if "_inited" not in self.__class__.__dict__:
            self._scan()

    @classmethod
    def _lookup(cls, v):
        if "_inited" not in cls.__dict__:
            cls._scan()

        # Finds an EnumValue by its raw value or name
        for k in dir(cls):
            a = getattr(cls, k)
            if isinstance(a, EnumValue) and (a.value == v or a.name == v):
                return a
        raise AttributeError(f"{v} is not in {cls.__name__}")

    @classmethod
    def __iter__(cls):
        if "_inited" not in cls.__dict__:
            cls._scan()

        for k in dir(cls):
            attr = getattr(cls, k)
            if isinstance(attr, EnumValue):
                yield attr

    @classmethod
    def list(cls):
        if "_inited" not in cls.__dict__:
            cls._scan()

        # Returns a list of all members
        return [getattr(cls, k) for k in dir(cls) if isinstance(getattr(cls, k), EnumValue)]

    @classmethod
    def _up(cls, k, v):
        setattr(cls, k, EnumValue(v, k))

    @classmethod
    def _scan(cls):
        # Convert class-level attributes (constants) to EnumValue objects
        for k, v in list(cls.__dict__.items()):
            if not k.startswith("_") and not callable(v) and not isinstance(v, EnumValue):
                cls._up(k, v)
        cls._inited = True

    def is_value(self, v):
        return any(m.value == v for m in self)

    def __repr__(self):
        # Supports the condition: obj == eval(repr(obj))
        d = {m.name: m.value for m in self}
        # Return a string like: Enum(name='Name', names={'KEY1': VALUE1, 'KEY2': VALUE2, ..})
        return f"Enum(name='{self.__class__.__name__}', names={d})"

    def __call__(self, v):
        if "_inited" in self.__class__.__dict__:
            self._scan()

        return self._lookup(v)

    def __setattr__(self, k, v):
        if "_inited" in self.__class__.__dict__:
            raise AttributeError(f"Enum '{self.__class__.__name__}' is immutable")
        super().__setattr__(k, v)

    def __delattr__(self, k):
        raise AttributeError("Enum is immutable")

    def __len__(self):
        return sum(1 for _ in self)

    def __eq__(self, o):
        if not isinstance(o, Enum):
            return False
        return self.list() == o.list()

# enum.py
# version="1.2.4"


class EnumValue:
    # An immutable object representing a specific enum member
    def __init__(self, value, name):
        object.__setattr__(self, "value", value)
        object.__setattr__(self, "name", name)

    def __repr__(self):
        return f"{self.name}: {self.value}"

    def __call__(self):
        return self.value

    def __eq__(self, other):
        if isinstance(other, EnumValue):
            return self.value == other.value
        return self.value == other

    def __setattr__(self, key, value):
        raise AttributeError("EnumValue is immutable")


class Enum:
    def __new__(cls, name=None, names=None):
        # Scenario 1: Reverse lookup by value (e.g., Status(1))
        if name is not None and names is None:
            if cls is not Enum:
                return cls._lookup(name)

        # Scenario 2: Functional API (e.g., Enum("Color", {"RED": 1}))
        return super(Enum, cls).__new__(cls)

    def __init__(self, name=None, names=None):
        if hasattr(self, "_initialized"):
            return

        # 1. Convert class-level attributes (constants) to EnumValue objects
        self._scan_class_attrs()

        # Support Functional API: Enum("Name", {"KEY": VALUE})
        if name is not None and isinstance(names, dict):
            for key, value in names.items():
                # Prevent addition if the key already exists
                if not hasattr(self, key):
                    self._update(key, value)

        object.__setattr__(self, "_initialized", True)

    @classmethod
    def _lookup(cls, value):
        # Finds an EnumValue by its raw value
        for key in dir(cls):
            if key.startswith("_"):
                continue
            attr = getattr(cls, key)
            if isinstance(attr, EnumValue) and (attr.value == value or attr.name == value):
                return attr
            if not callable(attr) and attr == value:
                # Wrap static numbers found in class definition
                return EnumValue(attr, key)
        raise AttributeError(f"{value} is not in {cls.__name__}")

    @classmethod
    def __iter__(cls):
        if "_initialized" not in cls.__dict__:
            cls._scan_class_attrs()
            setattr(cls, "_initialized", True)

        for key in dir(cls):
            if key.startswith("_"):
                continue
            attr = getattr(cls, key)
            if isinstance(attr, EnumValue):
                yield attr

    @classmethod
    def list(cls):
        if "_initialized" not in cls.__dict__:
            cls._scan_class_attrs()
            setattr(cls, "_initialized", True)

        # Returns a list of all members
        return [getattr(cls, key) for key in dir(cls) if isinstance(getattr(cls, key), EnumValue)]

    @classmethod
    def _update(cls, key, value):
        setattr(cls, key, EnumValue(value, key))

    @classmethod
    def _scan_class_attrs(cls):
        # Converts static class attributes into EnumValue objects
        # List of methods and internal names that should not be converted
        ignored = ("is_value", "list")
        for key in dir(cls):
            # Skip internal names and methods
            if key.startswith("_") or key in ignored:
                continue

            value = getattr(cls, key)
            # Convert only constants, not methods
            if not callable(value) and not isinstance(value, EnumValue):
                cls._update(key, value)

    def is_value(self, value):
        return any(member.value == value for member in self)

    def __repr__(self):
        # Supports the condition: obj == eval(repr(obj))
        members = {member.name: member.value for member in self}
        if self.__class__.__name__ == "Enum":
            return f"Enum(name='Enum', names={members})"
        # Return a string like: Name(names={"KEY1": VALUE1, "KEY2": VALUE2, ..})
        return f"{self.__class__.__name__}(names={members})"

    def __call__(self, value):
        if not hasattr(self, "_initialized"):
            self._scan_class_attrs()
            object.__setattr__(self, "_initialized", True)

        for member in self:
            if member.value == value or member.name == value:
                return member
        raise AttributeError(f"{value} is not in {self.__class__.__name__}")

    def __setattr__(self, key, value):
        if hasattr(self, "_initialized"):
            raise AttributeError(f"Enum '{self.__class__.__name__}' is static")
        super().__setattr__(key, value)

    def __delattr__(self, key):
        if hasattr(self, key) and isinstance(getattr(self, key), EnumValue):
            raise AttributeError("Enum members cannot be deleted")
        super().__delattr__(key)

    def __len__(self):
        return sum(1 for _ in self)

    def __eq__(self, other):
        if not isinstance(other, Enum):
            return False
        return self.list() == other.list()

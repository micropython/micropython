try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

print("# Python 3.6")
print("### PEP 526 - Syntax for variable annotations")

# https://peps.python.org/pep-0526/
# Currently excludes tests using `Generic[T]` due to MicroPython runtime limitations


print("Specification")

my_var: int
my_var = 5  # Passes type check.
other_var: int = "a"  # Flagged as error by type checker, # type: ignore
# but OK at runtime.


print("Global and local variable annotations")
from typing import List, Tuple, Optional

some_number: int  # variable without initial value
some_list: List[int] = []  # variable with initial value


sane_world: bool
if 2 + 2 == 4:
    sane_world = True
else:
    sane_world = False

# Tuple packing with variable annotation syntax
t: Tuple[int, ...] = (1, 2, 3)
# or
t: Tuple[int, ...] = 1, 2, 3  # This only works in Python 3.8+

# Tuple unpacking with variable annotation syntax
header: str
kind: int
body: Optional[List[str]]

a: int  # type: ignore
try:
    print(a)  # raises NameError # type: ignore

except NameError:
    print("Expected NameError")


def f_1():
    a: int
    try:
        print(a)  # raises UnboundLocalError # type: ignore
    except UnboundLocalError:
        print("Expected UnboundLocalError")


a: int  # type: ignore
a: str  # Static type checker may or may not warn about this.


print("Class and instance variable annotations")
from typing import ClassVar, Dict


class BasicStarship:
    captain: str = "Picard"  # instance variable with default
    damage: int  # instance variable without default
    stats: ClassVar[Dict[str, int]] = {}  # class variable


class Starship_1:
    captain = "Picard"
    stats = {}

    def __init__(self, damage, captain=None):
        self.damage = damage
        if captain:
            self.captain = captain  # Else keep the default

    def hit(self):
        Starship.stats["hits"] = Starship.stats.get("hits", 0) + 1


class Starship:
    captain: str = "Picard"
    damage: int
    stats: ClassVar[Dict[str, int]] = {}

    def __init__(self, damage: int, captain: str = None):  # type: ignore
        self.damage = damage
        if captain:
            self.captain = captain  # Else keep the default

    def hit(self):
        Starship.stats["hits"] = Starship.stats.get("hits", 0) + 1


enterprise_d = Starship(3000)
enterprise_d.stats = {}  # Flagged as error by a type checker # type: ignore
Starship.stats = {}  # This is OK


# FIXME: - cpy_diff - User Defined Generic Classes unsupported
# from typing import Generic, TypeVar
# T = TypeVar("T")
# class Box(Generic[T]):
#     def __init__(self, content):
#         self.content: T = content


print("Annotating expressions")


class Cls:
    pass


c = Cls()
c.x: int = 0  # Annotates c.x with int. # type: ignore
c.y: int  # Annotates c.y with int.# type: ignore

d = {}
d["a"]: int = 0  # Annotates d['a'] with int.# type: ignore
d["b"]: int  # Annotates d['b'] with int.# type: ignore

(x): int  # Annotates x with int, (x) treated as expression by compiler.# type: ignore
(y): int = 0  # Same situation here.


# print("Where annotations aren’t allowed")
# The Examples crash both CPython and MicroPython at runtime.

print("Runtime Effects of Type Annotations")


def f():
    x: NonexistentName  # No RUNTIME error. # type: ignore


# FIXME: cpy_diff - MicroPython does not raise NameError at runtime
# try:
#     x: NonexistentName  # Error!
#     print("-[ ] FIXME: Expected NameError")
# except NameError:
#     print("Expected NameError:")

# try:

#     class X:
#         var: NonexistentName  # Error!
# except NameError:
#     print("Expected NameError:")


# FIXME: cpy_diff - MicroPython does not provide the ``__annotations__`` dict at runtime
# print(__annotations__)
# __annotations__["s"] = str


alice: "well done" = "A+"  # type: ignore
bob: "what a shame" = "F-"  # type: ignore

print("-----")

# Test that "TYPE_CHECKING = False" is treated as a compile-time constant,
# so that code guarded by "if TYPE_CHECKING:" is eliminated.

TYPE_CHECKING = False

if "TYPE_CHECKING" in globals():
    # MICROPY_COMP_TYPE_CHECKING is disabled.
    print("SKIP")
    raise SystemExit

print(TYPE_CHECKING)

if TYPE_CHECKING:
    from typing import List, Optional

    print("not executed")
else:
    print("else branch")

if not TYPE_CHECKING:
    print("not TYPE_CHECKING")

print(TYPE_CHECKING or "or")
print(TYPE_CHECKING and "and")

while TYPE_CHECKING:
    print("not executed")

try:
    List
except NameError:
    print("List not defined")


class Foo:
    if TYPE_CHECKING:

        def write_cmd(self, cmd: int) -> None: ...

    def bar(self):
        if TYPE_CHECKING:
            print("not executed")
        return "bar"


print(hasattr(Foo, "write_cmd"))
print(Foo().bar())


def func(x):
    if TYPE_CHECKING:
        from typing import Any
    return x + 1


print(func(1))

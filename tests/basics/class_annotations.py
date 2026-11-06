# test PEP 526 class-body annotations recorded in __annotations__
#
# Gated by MICROPY_PY_BUILTINS_CLASS_ANNOTATIONS=1.

try:
    class _Probe:
        x: int
    _Probe.__annotations__
except AttributeError:
    print("SKIP")
    raise SystemExit

# Annotated members are recorded; values bind normally.
class Point:
    x: int
    y: int = 0
    z = 99  # unannotated; not in __annotations__

print(sorted(Point.__annotations__))
print(Point.__annotations__["x"] is int)
print(Point.__annotations__["y"] is int)
print(Point.y, Point.z)

# Default value reachable normally even when annotated.
class WithDefault:
    name: str = "hi"

print(WithDefault.name)
print("name" in WithDefault.__annotations__)

# Subclass does NOT auto-inherit __annotations__ from a base — match
# CPython's behaviour (typing.get_type_hints handles that walk, not the
# attribute itself).
class Sub(Point):
    extra: str = "child"

print(sorted(Sub.__annotations__))
# Defaults DO inherit (normal class attribute lookup), but only for
# fields that had a value — Point.x had only an annotation, no value.
print(Sub.y, Sub.z)

# Annotations evaluate exactly once per class definition. Side-effecting
# evaluator should produce a single trace, not per-access.
_side_effects = []

class _Trace:
    pass

class Marked:
    a: _Trace if not _side_effects.append("eval") else None = 1

print(_side_effects)

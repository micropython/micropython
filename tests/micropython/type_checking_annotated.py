# Test that the annotated form "TYPE_CHECKING: bool = False" is treated as a
# compile-time constant, so that code guarded by "if TYPE_CHECKING:" is eliminated.

TYPE_CHECKING: bool = False

if "TYPE_CHECKING" in globals():
    # MICROPY_COMP_TYPE_CHECKING is disabled.
    print("SKIP")
    raise SystemExit

print(TYPE_CHECKING)

if TYPE_CHECKING:
    print("not executed")
else:
    print("else branch")

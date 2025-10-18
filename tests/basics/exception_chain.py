# Exception chaining is not supported, but check that basic
# exception works as expected.

import sys

# The unix minimal build doesn't enable MICROPY_WARNINGS (required for this test).
if getattr(sys.implementation, "_build", None) == "minimal":
    print("SKIP")
    raise SystemExit

try:
    raise Exception from None
except Exception:
    print("Caught Exception")

try:
    try:
        raise ValueError("Value")
    except Exception as exc:
        raise RuntimeError("Runtime") from exc
except Exception as ex2:
    print("Caught Exception:", ex2)

# This list of package files doesn't include task.py because that's provided
# by the C module.
package(
    "asyncio",
    (
        "__init__.py",
        "core.py",
        "event.py",
        "funcs.py",
        "lock.py",
        "stream.py",
    ),
    base_path="..",
    opt=3,
)

# Backwards-compatible uasyncio module.
module("uasyncio.py", opt=3)

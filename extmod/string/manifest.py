package(
    "string",
    (
        "__init__.py",
        "templatelib.py",
    ),
    base_path="..",
    opt=3,
)

package(
    "string",
    ("string.py",),
    base_path="$(MPY_LIB_DIR)/python-stdlib",
    opt=3,
)

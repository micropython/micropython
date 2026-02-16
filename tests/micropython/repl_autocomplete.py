# Check that micropython.repl_autocomplete works as expected.
try:
    import micropython

    micropython.repl_autocomplete
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# Single completion: keyword "import" (includes trailing space).
# The "import" keyword is special-cased in mp_repl_autocomplete and works
# regardless of globals dict context (direct, -c, --via-mpy, etc).
print(repr(micropython.repl_autocomplete("impo")))

# No match: returns None.
print(repr(micropython.repl_autocomplete("xyz_no_match")))

# Multiple completions with no further common prefix: prints candidates to
# stdout and returns empty string.
_tca = 1
_tcb = 2
print(repr(micropython.repl_autocomplete("_tc")))

# Attribute completion: "sys.ver" matches sys.version and sys.version_info.
# Common prefix is "version" so the completion suffix is "sion".
import sys

print(repr(micropython.repl_autocomplete("sys.ver")))

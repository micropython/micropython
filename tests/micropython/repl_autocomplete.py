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

# No match: returns empty string
print(repr(micropython.repl_autocomplete("xyz_no_match")))

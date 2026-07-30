# Strict SyntaxError checks for experimental match/case

def must_syntax_error(src):
    try:
        exec(src)
    except SyntaxError:
        return
    raise AssertionError("expected SyntaxError")


# Irrefutable pattern must be last
must_syntax_error(
    """
match 1:
    case x:
        pass
    case 2:
        pass
"""
)

must_syntax_error(
    """
match 1:
    case _:
        pass
    case 2:
        pass
"""
)

# Or-patterns must bind the same names
must_syntax_error(
    """
match 1:
    case x | y:
        pass
"""
)

print("PASS")

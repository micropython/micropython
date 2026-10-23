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

must_syntax_error(
    """
match 1:
    case x | x:
        pass
    case 2:
        pass
"""
)

# Or-patterns must bind the same capture names
must_syntax_error(
    """
match 1:
    case x | y:
        pass
"""
)

must_syntax_error(
    """
match 1:
    case (a, 1) | (b, 1):
        pass
"""
)

# Repeated capture name in one pattern
must_syntax_error(
    """
match 1:
    case (c, c):
        pass
"""
)

# Capture limit for or-pattern validation
must_syntax_error(
    """
match 1:
    case (a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) | (a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q):
        pass
"""
)

print("PASS")

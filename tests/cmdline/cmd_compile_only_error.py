# cmdline: -X compile-only
# test compile-only with syntax error
print("This should not be printed")
def broken_syntax(
    # Missing closing parenthesis
    return "error"

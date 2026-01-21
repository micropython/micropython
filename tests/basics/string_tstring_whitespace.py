print("# Empty expression (whitespace only)")
try:
    exec('t"{ }"')
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print("Empty expr (space): SyntaxError (correct)")

try:
    exec('t"{\t}"')
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print("Empty expr (tab): SyntaxError (correct)")

try:
    exec('t"{\n}"')
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print("Empty expr (newline): SyntaxError (correct)")

print("\n# Lexer escape sequence: invalid then valid")
try:
    exec("t'\\U00200000\\x41'")
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print("Invalid escape sequence: SyntaxError (correct)")

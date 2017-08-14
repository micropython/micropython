# parser tests

try:
    compile
except NameError:
    print("SKIP")
    raise SystemExit

# completely empty string
# uPy and CPy differ for this case
#try:
#    compile("", "stdin", "single")
#except SyntaxError:
#    print("SyntaxError")
try:
    compile("", "stdin", "eval")
except SyntaxError:
    print("SyntaxError")
compile("", "stdin", "exec")

# empty continued line
try:
    compile("\\\n", "stdin", "single")
except SyntaxError:
    print("SyntaxError")
try:
    compile("\\\n", "stdin", "eval")
except SyntaxError:
    print("SyntaxError")
compile("\\\n", "stdin", "exec")

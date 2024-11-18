# test the lexer

try:
    eval
    exec
except NameError:
    print("SKIP")
    raise SystemExit

# __debug__ is a special symbol
print(type(__debug__))

# short input
exec("")
exec("\n")
exec("\n\n")
exec("\r")
exec("\r\r")
exec("\t")
exec("\r\n")
exec("\nprint(1)")
exec("\rprint(2)")
exec("\r\nprint(3)")
exec("\n5")
exec("\r6")
exec("\r\n7")
print(eval("1"))
print(eval("12"))
print(eval("123"))
print(eval("1\n"))
print(eval("12\n"))
print(eval("123\n"))
print(eval("1\r"))
print(eval("12\r"))
print(eval("123\r"))

# line continuation
print(eval("'123' \\\r '456'"))
print(eval("'123' \\\n '456'"))
print(eval("'123' \\\r\n '456'"))
print(eval("'123'\\\r'456'"))
print(eval("'123'\\\n'456'"))
print(eval("'123'\\\r\n'456'"))

# backslash used to escape a line-break in a string
print('a\
b')

# lots of indentation
def a(x):
 if x:
  if x:
   if x:
    if x:
     if x:
      if x:
       if x:
        if x:
         if x:
          if x:
           if x:
            if x:
             if x:
              if x:
               if x:
                print(x)
a(1)

# badly formed hex escape sequences
try:
    exec(r"'\x0'")
except SyntaxError:
    print("SyntaxError")
try:
    exec(r"b'\x0'")
except SyntaxError:
    print("SyntaxError")
try:
    exec(r"'\u000'")
except SyntaxError:
    print("SyntaxError")
try:
    exec(r"'\U0000000'")
except SyntaxError:
    print("SyntaxError")

# Properly formed integer literals
print(eval("00"))
# badly formed integer literals
try:
    eval("01")
except SyntaxError:
    print("SyntaxError")

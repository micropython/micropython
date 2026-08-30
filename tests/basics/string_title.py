# Test str.title(), str.istitle(), and str.casefold()
try:
    str.title
except:
    print("SKIP")
    raise SystemExit

# title
print("".title())
print("hello world".title())
print("HELLO WORLD".title())
print("hello-world".title())
print("hello123world".title())
print("  hello  ".title())
print("a".title())

# istitle
print("".istitle())
print("Hello World".istitle())
print("Hello world".istitle())
print("HELLO".istitle())
print("Hello".istitle())
print("123".istitle())
print("Hello-World".istitle())

# casefold
print("".casefold())
print("HELLO".casefold())
print("Hello World".casefold())
print("abc123".casefold())

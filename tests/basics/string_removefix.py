# Test str.removeprefix() and str.removesuffix()
try:
    str.removeprefix
except:
    print("SKIP")
    raise SystemExit

print("=== removeprefix ===")
print("hello world".removeprefix("hello "))
print("hello world".removeprefix("world"))
print("hello world".removeprefix(""))
print("hello".removeprefix("hello"))
print("hello".removeprefix("hello world"))
print("".removeprefix("test"))

print("\n=== removesuffix ===")
print("hello world".removesuffix(" world"))
print("hello world".removesuffix("hello"))
print("hello world".removesuffix(""))
print("hello".removesuffix("hello"))
print("hello".removesuffix("hello world"))
print("".removesuffix("test"))

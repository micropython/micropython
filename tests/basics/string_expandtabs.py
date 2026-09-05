# Test str.expandtabs()
try:
    str.expandtabs
except:
    print("SKIP")
    raise SystemExit

print("".expandtabs())
print("\t".expandtabs())
print("hello\tworld".expandtabs())
print("hello\tworld".expandtabs(4))
print("\t\t".expandtabs())
print("01\t012\t0123\t01234".expandtabs())
print("01\t012\t0123\t01234".expandtabs(4))
print("hello\tworld\n\tfoo".expandtabs())
print("hello\tworld\r\tfoo".expandtabs())
print("no tabs here".expandtabs())
print("\t".expandtabs(0))
print("\t".expandtabs(1))

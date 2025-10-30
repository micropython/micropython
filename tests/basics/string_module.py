try:
    import string
except ImportError:
    print("SKIP")
    raise SystemExit

print("ascii_letters", string.ascii_letters[:8])
print("digits", string.digits)
print("punct", string.punctuation[:5])

table = {ord("a"): "z", ord("b"): None, ord("c"): 120}
print("translate", string.translate("abc", table))

from string.templatelib import Template
print("templatelib", isinstance(t"hi", Template))

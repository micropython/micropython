try:
    try:
        import ubase64 as base64
    except ImportError:
        import base64
except ImportError:
    print("SKIP")
    raise SystemExit


print(base64.b64encode(b"foo"))
print(base64.b64encode(b"\xfb\xff", altchars=b"[]"))

print(base64.b64decode(b"Zm9v"))
print(base64.b64decode(b"[]8=", altchars=b"[]"))

print(base64.b64decode(b"Zm9v", validate=True))
print(base64.b64decode("+/8=", validate=True))

print(base64.b64decode(b"[]8=", altchars=b"[]", validate=True))
print(base64.b64decode("[]8=", altchars=b"[]", validate=True))

print(base64.standard_b64encode(b"foo"))
print(base64.standard_b64encode(b"\xfb\xff"))

print(base64.standard_b64decode("Zm9v"))
print(base64.standard_b64decode(b"Zm9v"))
print(base64.standard_b64decode(b"/+8="))

print(base64.urlsafe_b64encode(b"foo"))
print(base64.urlsafe_b64encode(b"\xfb\xff"))

print(base64.urlsafe_b64decode("Zm9v"))
print(base64.urlsafe_b64decode(b"Zm9v"))
print(base64.urlsafe_b64decode(b"-_8="))

try:
    print(base64.b64decode(b"Zm9-", validate=True))
except ValueError:
    print("ValueError")
try:
    print(base64.b64decode(b"Zm9", validate=True))
except ValueError:
    print("ValueError")

try:
    print(base64.b64encode(b"foo", altchars=b"["))
except AssertionError:
    print("AssertionError")

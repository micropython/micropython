# test os.uname() attrtuple, if available
try:
    import os
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    u = os.uname()
except AttributeError:
    print("SKIP")
    raise SystemExit

# test printing of attrtuple
print(str(u).find("machine=") > 0)

# test read attr
print(isinstance(u.machine, str))

# test str modulo operator for attrtuple
impl_str = ("%s " * len(u)) % u
test_str = ""
for val in u:
    test_str += val + " "
print(impl_str == test_str)

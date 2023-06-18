# Checks that an instance type inheriting from a native base that uses
# MP_TYPE_FLAG_ITER_IS_STREAM will still have a getiter.

try:
    import uio as io
except ImportError:
    import io

a = io.StringIO()
a.write("hello\nworld\nmicro\npython\n")
a.seek(0)

for line in a:
    print(line)

class X(io.StringIO):
    pass

b = X()
b.write("hello\nworld\nmicro\npython\n")
b.seek(0)

for line in b:
    print(line)

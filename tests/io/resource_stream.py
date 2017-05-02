import uio
import sys

try:
    uio.resource_stream
except AttributeError:
    print('SKIP')
    sys.exit()

try:
    buf = uio.resource_stream("data", "file2")
except NotImplementedError:
    pass

# resource_stream(None, ...) look ups from current dir, hence sys.path[0] hack
buf = uio.resource_stream(None, sys.path[0] + "/data/file2")
print(buf.read())

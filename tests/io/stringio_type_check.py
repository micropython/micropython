import sys

sys.path[0] = ".frozen"  # avoid local dir io import

import io


class TestStream(io.StringIO):
    def __init_(self, alloc_size):
        super().__init__(alloc_size)


class ByteStream(io.BytesIO):
    def __init_(self, alloc_size):
        super().__init__(alloc_size)


test_stringio = TestStream(100)

test_bytesio = ByteStream(100)

try:
    print("hello", file=test_stringio)
except Exception as e:
    print(e)


try:
    print("hello", file=test_bytesio)
except Exception as e:
    print(e)

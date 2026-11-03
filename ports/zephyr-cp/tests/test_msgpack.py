# SPDX-FileCopyrightText: 2026 Tim Cocks for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test the msgpack module."""

import pytest


ROUNDTRIP_CODE = """\
import msgpack
from io import BytesIO

obj = {"list": [True, False, None, 1, 3.125], "str": "blah"}
b = BytesIO()
msgpack.pack(obj, b)
encoded = b.getvalue()
print(f"encoded_len: {len(encoded)}")
print(f"encoded_hex: {encoded.hex()}")

b.seek(0)
decoded = msgpack.unpack(b)
print(f"decoded: {decoded}")
print(f"match: {decoded == obj}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": ROUNDTRIP_CODE})
def test_msgpack_roundtrip(circuitpython):
    """Pack and unpack a dict containing the basic msgpack types."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "match: True" in output
    assert "done" in output


USE_LIST_CODE = """\
import msgpack
from io import BytesIO

b = BytesIO()
msgpack.pack([1, 2, 3], b)

b.seek(0)
as_list = msgpack.unpack(b)
print(f"as_list: {as_list} type={type(as_list).__name__}")

b.seek(0)
as_tuple = msgpack.unpack(b, use_list=False)
print(f"as_tuple: {as_tuple} type={type(as_tuple).__name__}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": USE_LIST_CODE})
def test_msgpack_use_list(circuitpython):
    """use_list=False should return a tuple instead of a list."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "as_list: [1, 2, 3] type=list" in output
    assert "as_tuple: (1, 2, 3) type=tuple" in output
    assert "done" in output


EXTTYPE_CODE = """\
from msgpack import pack, unpack, ExtType
from io import BytesIO

class MyClass:
    def __init__(self, val):
        self.value = val

data = MyClass(b"my_value")

def encoder(obj):
    if isinstance(obj, MyClass):
        return ExtType(1, obj.value)
    return f"no encoder for {obj}"

def decoder(code, data):
    if code == 1:
        return MyClass(data)
    return f"no decoder for type {code}"

buf = BytesIO()
pack(data, buf, default=encoder)
buf.seek(0)
decoded = unpack(buf, ext_hook=decoder)
print(f"decoded_type: {type(decoded).__name__}")
print(f"decoded_value: {decoded.value}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": EXTTYPE_CODE})
def test_msgpack_exttype(circuitpython):
    """ExtType with a custom encoder/decoder should round-trip."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "decoded_type: MyClass" in output
    assert "decoded_value: b'my_value'" in output
    assert "done" in output


EXTTYPE_PROPS_CODE = """\
from msgpack import ExtType

e = ExtType(5, b"hello")
print(f"code: {e.code}")
print(f"data: {e.data}")

e.code = 10
print(f"new_code: {e.code}")

try:
    ExtType(128, b"x")
except (ValueError, OverflowError) as ex:
    print(f"range_error: {type(ex).__name__}")

print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": EXTTYPE_PROPS_CODE})
def test_msgpack_exttype_properties(circuitpython):
    """ExtType exposes code/data as read/write properties and rejects out-of-range codes."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "code: 5" in output
    assert "data: b'hello'" in output
    assert "new_code: 10" in output
    assert "range_error:" in output
    assert "done" in output


INT_BOUNDARY_CODE = """\
import msgpack
from io import BytesIO

CASES = (
    (127, "7f"),
    (128, "cc80"),
    (255, "ccff"),
    (256, "cd0100"),
    (65535, "cdffff"),
    (65536, "ce00010000"),
    (2**32 - 1, "ceffffffff"),
    (2**32, "cf0000000100000000"),
    (2**64 - 1, "cfffffffffffffffff"),
    (-32, "e0"),
    (-33, "d0df"),
    (-129, "d1ff7f"),
    (-32769, "d2ffff7fff"),
    (-(2**31) - 1, "d3ffffffff7fffffff"),
    (-(2**63), "d38000000000000000"),
)
for value, expected in CASES:
    b = BytesIO()
    msgpack.pack(value, b)
    b.seek(0)
    if b.getvalue().hex() != expected or msgpack.unpack(b) != value:
        print(f"bad: {value} {b.getvalue().hex()}")

for value in (2**64, -(2**63) - 1, -(2**64)):
    b = BytesIO()
    msgpack.pack(value, b, default=lambda obj: "big")
    print(f"default: {b.getvalue().hex()}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": INT_BOUNDARY_CODE})
def test_msgpack_int_boundaries(circuitpython):
    """Ints use the shortest format, and those beyond 64 bits go to the default handler."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "bad:" not in output
    assert output.count("default: a3626967") == 3
    assert "done" in output

import io

from displayio import Bitmap
import binascii
import jpegio
import bitmaptools

content = binascii.a2b_base64(
    b"""
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
bGuXlnADbup+WcAAAAAAAAAAAAD/2Q=="""
)

decoder = jpegio.JpegDecoder()


def dump_bitmap(b):
    w = b.width
    h = b.height

    for y in range(min(30, h)):
        for x in range(min(72, w)):
            print(end="#" if (b[x, y] & 0x7E0) < 0x400 else " ")
        print("..." if w > 72 else "")
    print("...\n" if h > 24 else "")


def test(jpeg_input, scale, fill=0xFFFF, **position_and_crop):
    w, h = decoder.open(jpeg_input)
    w >>= scale
    h >>= scale
    print(f"{w}x{h}")

    b = Bitmap(w, h, 65535)
    b.fill(fill)

    decoder.decode(b, scale=scale, **position_and_crop)

    if position_and_crop:
        position_and_crop.setdefault("x", 0)
        position_and_crop.setdefault("y", 0)
        full = Bitmap(w, h, 65535)
        decoder.open(jpeg_input)
        decoder.decode(full, scale=scale)
        refb = Bitmap(w, h, 65535)
        refb.fill(fill)
        bitmaptools.blit(refb, full, **position_and_crop)
        print(f"{memoryview(refb) == memoryview(b)=}")
        if not memoryview(refb) == memoryview(b):
            dump_bitmap(b)
    else:
        dump_bitmap(b)


class IOAdapter(io.IOBase):
    def __init__(self, content):
        self._content = memoryview(content).cast("b")
        self._pos = 0

    def readinto(self, buf):
        pos = self._pos
        data = self._content[pos : pos + len(buf)]
        len_data = len(data)
        buf[:len_data] = data
        self._pos = pos + len_data
        return len_data


print("bytes")
test(content, scale=0)
test(content, scale=1)
test(content, scale=2)
test(content, scale=3)

bytesio = io.BytesIO(content)

print("BytesIO")
test(io.BytesIO(content), scale=3)

ioadapter = IOAdapter(content)
print("IOAdapter")
test(io.BytesIO(content), scale=3)

print("crop & move")
test(content, scale=3, x=8, y=12)
test(content, scale=3, x1=8, y1=12)
test(content, scale=3, x2=16, y2=16)
test(content, scale=3, x=12, y=16, x1=8, y1=12, x2=16, y2=16)

print("color key")
test(content, scale=0, skip_source_index=0x4529, fill=0)

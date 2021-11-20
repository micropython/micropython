import displayio
import bitmaptools


def loadbmp16(filename, width=320, height=240):
    """This specialized routine loads 16bpp uncompressed bmp files with a
    70-byte header. It is not appropriate for generic bmp files."""

    bitmap = displayio.Bitmap(width, height, 65536)
    with open(filename, "rb") as f:
        f.seek(70)
        bitmaptools.readinto(
            bitmap,
            f,
            bits_per_pixel=16,
            element_size=2,
            swap_bytes_in_element=True,
            reverse_rows=True,
        )

    return bitmap


if __name__ == "__main__":
    if "/" in __file__:
        here = __file__.rsplit("/", 1)[0]
    else:
        here = "."
    b = loadbmp16(here + "/minerva16.bmp")
    print(b[0, 0])
    print(b[160, 160])

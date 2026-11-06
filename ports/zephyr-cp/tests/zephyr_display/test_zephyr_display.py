# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

import colorsys
import shutil
import struct
from pathlib import Path

import pytest
from PIL import Image


def _read_image(path: Path) -> tuple[int, int, bytes]:
    """Read an image file and return (width, height, RGB bytes)."""
    with Image.open(path) as img:
        rgb = img.convert("RGB")
        return rgb.width, rgb.height, rgb.tobytes()


def _read_mask(golden_path: Path) -> bytes | None:
    """Load the companion mask PNG for a golden image, if it exists.

    Non-zero pixels are masked (skipped during comparison).
    """
    mask_path = golden_path.with_suffix(".mask.png")
    if not mask_path.exists():
        return None
    with Image.open(mask_path) as img:
        return img.convert("L").tobytes()


def _assert_pixels_equal_masked(
    golden_pixels: bytes, actual_pixels: bytes, mask: bytes | None = None
):
    """Assert pixels match, skipping positions where the mask is non-zero."""
    assert len(golden_pixels) == len(actual_pixels)
    for i in range(0, len(golden_pixels), 3):
        if mask is not None and mask[i // 3] != 0:
            continue
        if golden_pixels[i : i + 3] != actual_pixels[i : i + 3]:
            pixel = i // 3
            assert False, (
                f"Pixel {pixel} mismatch: "
                f"golden={tuple(golden_pixels[i : i + 3])} "
                f"actual={tuple(actual_pixels[i : i + 3])}"
            )


BOARD_DISPLAY_AVAILABLE_CODE = """\
import board
print(hasattr(board, 'DISPLAY'))
print(type(board.DISPLAY).__name__)
print(board.DISPLAY.width, board.DISPLAY.height)
print('done')
"""


@pytest.mark.circuitpy_drive({"code.py": BOARD_DISPLAY_AVAILABLE_CODE})
@pytest.mark.display
@pytest.mark.duration(8)
def test_board_display_available(circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "True" in output
    assert "Display" in output
    assert "320 240" in output
    assert "done" in output


CONSOLE_TERMINAL_PRESENT_CODE = """\
import board

root = board.DISPLAY.root_group
has_terminal_tilegrids = (
    type(root).__name__ == 'Group' and
    len(root) >= 2 and
    type(root[0]).__name__ == 'TileGrid' and
    type(root[-1]).__name__ == 'TileGrid'
)
print('has_terminal_tilegrids:', has_terminal_tilegrids)
print('done')
"""


@pytest.mark.circuitpy_drive({"code.py": CONSOLE_TERMINAL_PRESENT_CODE})
@pytest.mark.display
@pytest.mark.duration(8)
def test_console_terminal_present_by_default(circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "has_terminal_tilegrids: True" in output
    assert "done" in output


CONSOLE_OUTPUT_GOLDEN_CODE = """\
import time
time.sleep(0.25)
print('done')
while True:
    time.sleep(1)
"""


def _golden_compare_or_update(request, captures, golden_path, mask_path=None):
    """Compare captured PNG against golden, or update golden if --update-goldens.

    mask_path overrides the default companion mask lookup (golden.mask.png).
    """
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    if request.config.getoption("--update-goldens"):
        golden_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(captures[0], golden_path)
        return

    gw, gh, gpx = _read_image(golden_path)
    dw, dh, dpx = _read_image(captures[0])
    if mask_path is not None:
        mask = _read_mask(mask_path)
    else:
        mask = _read_mask(golden_path)

    assert (dw, dh) == (gw, gh)
    _assert_pixels_equal_masked(gpx, dpx, mask)


@pytest.mark.circuitpy_drive({"code.py": CONSOLE_OUTPUT_GOLDEN_CODE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(8)
def test_console_output_golden(request, circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "done" in output

    golden = Path(__file__).parent / "golden" / "terminal_console_output_320x240.png"
    _golden_compare_or_update(request, circuitpython.display_capture_paths(), golden)


PIXEL_FORMATS = ["ARGB_8888", "RGB_888", "RGB_565", "BGR_565", "L_8", "AL_88", "MONO01", "MONO10"]

# Shared mask: the same screen regions vary regardless of pixel format.
_CONSOLE_GOLDEN_MASK = Path(__file__).parent / "golden" / "terminal_console_output_320x240.png"


@pytest.mark.circuitpy_drive({"code.py": CONSOLE_OUTPUT_GOLDEN_CODE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(8)
@pytest.mark.parametrize(
    "pixel_format",
    PIXEL_FORMATS,
    indirect=True,
)
def test_console_output_golden_pixel_format(pixel_format, request, circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "done" in output

    golden_name = f"terminal_console_output_320x240_{pixel_format}.png"
    golden = Path(__file__).parent / "golden" / golden_name
    _golden_compare_or_update(
        request, circuitpython.display_capture_paths(), golden, _CONSOLE_GOLDEN_MASK
    )


MONO_NO_VTILED_FORMATS = ["MONO01", "MONO10"]


@pytest.mark.circuitpy_drive({"code.py": CONSOLE_OUTPUT_GOLDEN_CODE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.display_mono_vtiled(False)
@pytest.mark.duration(8)
@pytest.mark.parametrize(
    "pixel_format",
    MONO_NO_VTILED_FORMATS,
    indirect=True,
)
def test_console_output_golden_mono_no_vtiled(pixel_format, request, circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "done" in output

    golden_name = f"terminal_console_output_320x240_{pixel_format}_no_vtiled.png"
    golden = Path(__file__).parent / "golden" / golden_name
    _golden_compare_or_update(
        request, circuitpython.display_capture_paths(), golden, _CONSOLE_GOLDEN_MASK
    )


def _generate_gradient_bmp(width, height):
    """Generate a 24-bit BMP with HSL color gradient.

    Hue sweeps left to right, lightness goes from black (bottom) to white (top),
    saturation is 1.0.
    """
    row_size = width * 3
    row_padding = (4 - (row_size % 4)) % 4
    padded_row_size = row_size + row_padding
    pixel_data_size = padded_row_size * height
    file_size = 14 + 40 + pixel_data_size

    header = struct.pack(
        "<2sIHHI",
        b"BM",
        file_size,
        0,
        0,
        14 + 40,
    )
    info = struct.pack(
        "<IiiHHIIiiII",
        40,
        width,
        height,
        1,
        24,
        0,
        pixel_data_size,
        0,
        0,
        0,
        0,
    )

    # BMP is bottom-up: row 0 = bottom (black), row height-1 = top (white).
    rows = []
    for y in range(height):
        l = y / (height - 1)
        row = bytearray()
        for x in range(width):
            h = x / width
            r, g, b = colorsys.hls_to_rgb(h, l, 1.0)
            # BMP pixel order is BGR.
            row.extend([int(b * 255 + 0.5), int(g * 255 + 0.5), int(r * 255 + 0.5)])
        row.extend(b"\x00" * row_padding)
        rows.append(bytes(row))

    return header + info + b"".join(rows)


DISPLAYIO_COLOR_GRADIENT_CODE = """\
import board
import displayio
import time

odb = displayio.OnDiskBitmap("/gradient.bmp")
tg = displayio.TileGrid(odb, pixel_shader=odb.pixel_shader)
g = displayio.Group()
g.append(tg)

board.DISPLAY.auto_refresh = False
board.DISPLAY.root_group = g
board.DISPLAY.refresh()
print('rendered')
while True:
    time.sleep(1)
"""

_GRADIENT_BMP = _generate_gradient_bmp(320, 240)
_GRADIENT_DRIVE = {"code.py": DISPLAYIO_COLOR_GRADIENT_CODE, "gradient.bmp": _GRADIENT_BMP}


@pytest.mark.circuitpy_drive(_GRADIENT_DRIVE)
@pytest.mark.display(capture_times_ns=[14_000_000_000])
@pytest.mark.duration(18)
def test_displayio_color_gradient(request, circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "rendered" in output

    golden = Path(__file__).parent / "golden" / "color_gradient_320x240.png"
    _golden_compare_or_update(request, circuitpython.display_capture_paths(), golden)


@pytest.mark.circuitpy_drive(_GRADIENT_DRIVE)
@pytest.mark.display(capture_times_ns=[14_000_000_000])
@pytest.mark.duration(18)
@pytest.mark.parametrize(
    "pixel_format",
    PIXEL_FORMATS,
    indirect=True,
)
def test_displayio_color_gradient_pixel_format(pixel_format, request, circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "rendered" in output

    golden_name = f"color_gradient_320x240_{pixel_format}.png"
    golden = Path(__file__).parent / "golden" / golden_name
    _golden_compare_or_update(request, circuitpython.display_capture_paths(), golden)


DISPLAYIO_ONDISKBITMAP_LIFETIME_CODE = """\
import board
import displayio

with open("/gradient.bmp", "rb") as f:
    try:
        displayio.OnDiskBitmap(f)
    except TypeError:
        print("open file rejected")

with displayio.OnDiskBitmap("/gradient.bmp") as odb:
    print("width", odb.width)
try:
    odb.width
except ValueError:
    print("with block deinited")

odb = displayio.OnDiskBitmap("/gradient.bmp")
g = displayio.Group()
g.append(displayio.TileGrid(odb, pixel_shader=odb.pixel_shader))
board.DISPLAY.root_group = g
odb.deinit()
odb.deinit()
board.DISPLAY.refresh()
print("drawn after deinit")
"""


@pytest.mark.circuitpy_drive(
    {"code.py": DISPLAYIO_ONDISKBITMAP_LIFETIME_CODE, "gradient.bmp": _GRADIENT_BMP}
)
@pytest.mark.display
@pytest.mark.duration(8)
def test_ondiskbitmap_lifetime(circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "open file rejected" in output
    assert "width 320" in output
    assert "with block deinited" in output
    assert "drawn after deinit" in output


DISPLAYIO_GOLDEN_STRIPES_CODE = """\
import board
import displayio
import time

w = board.DISPLAY.width
h = board.DISPLAY.height

bitmap = displayio.Bitmap(w, h, 4)
palette = displayio.Palette(4)
palette[0] = 0xFF0000
palette[1] = 0x00FF00
palette[2] = 0x0000FF
palette[3] = 0xFFFFFF

for y in range(h):
    for x in range(w):
        bitmap[x, y] = (x * 4) // w

tg = displayio.TileGrid(bitmap, pixel_shader=palette)
g = displayio.Group()
g.append(tg)

board.DISPLAY.auto_refresh = False
board.DISPLAY.root_group = g
board.DISPLAY.refresh()
print('rendered')
while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": DISPLAYIO_GOLDEN_STRIPES_CODE})
@pytest.mark.display(capture_times_ns=[14_000_000_000])
@pytest.mark.duration(15)
def test_displayio_golden_stripes(circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "rendered" in output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    dw, dh, dpx = _read_image(captures[0])
    assert (dw, dh) == (320, 240)

    def pixel_at(x, y):
        i = (y * dw + x) * 3
        return tuple(dpx[i : i + 3])

    # Check stripe colors at four sample points across the middle row.
    # Depending on the selected Zephyr pixel format, red and blue channels may
    # be swapped, so accept either ordering for those two bands.
    y = dh // 2
    left = pixel_at(dw // 8, y)
    mid_left = pixel_at((3 * dw) // 8, y)
    mid_right = pixel_at((5 * dw) // 8, y)
    right = pixel_at((7 * dw) // 8, y)

    assert left in ((255, 0, 0), (0, 0, 255))
    assert mid_left == (0, 255, 0)
    assert mid_right in ((255, 0, 0), (0, 0, 255))
    assert mid_right != left
    assert right == (255, 255, 255)

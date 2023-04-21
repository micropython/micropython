"""ILI9341 LCD/Touch module."""
from time import sleep
from math import cos, sin, pi, radians
from sys import implementation
from framebuf import FrameBuffer, RGB565  # type: ignore
import ustruct  # type: ignore


def color565(r, g, b):
    """Return RGB565 color value.

    Args:
        r (int): Red value.
        g (int): Green value.
        b (int): Blue value.
    """
    return (r & 0xf8) << 8 | (g & 0xfc) << 3 | b >> 3


class TFT(object):
    """Serial interface for 16-bit color (5-6-5 RGB) IL9341 display.

    Note:  All coordinates are zero based.
    """

    # Command constants from ILI9341 datasheet
    NOP = const(0x00)  # No-op
    SWRESET = const(0x01)  # Software reset
    RDDID = const(0x04)  # Read display ID info
    RDDST = const(0x09)  # Read display status
    SLPIN = const(0x10)  # Enter sleep mode
    SLPOUT = const(0x11)  # Exit sleep mode
    PTLON = const(0x12)  # Partial mode on
    NORON = const(0x13)  # Normal display mode on
    RDMODE = const(0x0A)  # Read display power mode
    RDMADCTL = const(0x0B)  # Read display MADCTL
    RDPIXFMT = const(0x0C)  # Read display pixel format
    RDIMGFMT = const(0x0D)  # Read display image format
    RDSELFDIAG = const(0x0F)  # Read display self-diagnostic
    INVOFF = const(0x20)  # Display inversion off
    INVON = const(0x21)  # Display inversion on
    GAMMASET = const(0x26)  # Gamma set
    DISPLAY_OFF = const(0x28)  # Display off
    DISPLAY_ON = const(0x29)  # Display on
    SET_COLUMN = const(0x2A)  # Column address set
    SET_PAGE = const(0x2B)  # Page address set
    WRITE_RAM = const(0x2C)  # Memory write
    READ_RAM = const(0x2E)  # Memory read
    PTLAR = const(0x30)  # Partial area
    VSCRDEF = const(0x33)  # Vertical scrolling definition
    MADCTL = const(0x36)  # Memory access control
    VSCRSADD = const(0x37)  # Vertical scrolling start address
    PIXFMT = const(0x3A)  # COLMOD: Pixel format set
    WRITE_DISPLAY_BRIGHTNESS = const(0x51)  # Brightness hardware dependent!
    READ_DISPLAY_BRIGHTNESS = const(0x52)
    WRITE_CTRL_DISPLAY = const(0x53)
    READ_CTRL_DISPLAY = const(0x54)
    WRITE_CABC = const(0x55)  # Write Content Adaptive Brightness Control
    READ_CABC = const(0x56)  # Read Content Adaptive Brightness Control
    WRITE_CABC_MINIMUM = const(0x5E)  # Write CABC Minimum Brightness
    READ_CABC_MINIMUM = const(0x5F)  # Read CABC Minimum Brightness
    FRMCTR1 = const(0xB1)  # Frame rate control (In normal mode/full colors)
    FRMCTR2 = const(0xB2)  # Frame rate control (In idle mode/8 colors)
    FRMCTR3 = const(0xB3)  # Frame rate control (In partial mode/full colors)
    INVCTR = const(0xB4)  # Display inversion control
    DFUNCTR = const(0xB6)  # Display function control
    PWCTR1 = const(0xC0)  # Power control 1
    PWCTR2 = const(0xC1)  # Power control 2
    PWCTRA = const(0xCB)  # Power control A
    PWCTRB = const(0xCF)  # Power control B
    VMCTR1 = const(0xC5)  # VCOM control 1
    VMCTR2 = const(0xC7)  # VCOM control 2
    RDID1 = const(0xDA)  # Read ID 1
    RDID2 = const(0xDB)  # Read ID 2
    RDID3 = const(0xDC)  # Read ID 3
    RDID4 = const(0xDD)  # Read ID 4
    GMCTRP1 = const(0xE0)  # Positive gamma correction
    GMCTRN1 = const(0xE1)  # Negative gamma correction
    DTCA = const(0xE8)  # Driver timing control A
    DTCB = const(0xEA)  # Driver timing control B
    POSC = const(0xED)  # Power on sequence control
    ENABLE3G = const(0xF2)  # Enable 3 gamma control
    PUMPRC = const(0xF7)  # Pump ratio control

    ROTATE = {
        0: 0x88,
        90: 0xE8,
        180: 0x48,
        270: 0x28
    }

    def __init__(self, spi, cs, dc, rst,
                 width=240, height=320, rotation=0):
        """Initialize OLED.

        Args:
            spi (Class Spi):  SPI interface for OLED
            cs (Class Pin):  Chip select pin
            dc (Class Pin):  Data/Command pin
            rst (Class Pin):  Reset pin
            width (Optional int): Screen width (default 240)
            height (Optional int): Screen height (default 320)
            rotation (Optional int): Rotation must be 0 default, 90. 180 or 270
        """
        self.spi = spi
        self.cs = cs
        self.dc = dc
        self.rst = rst
        self.width = width
        self.height = height
        if rotation not in self.ROTATE.keys():
            raise RuntimeError('Rotation must be 0, 90, 180 or 270.')
        else:
            self.rotation = self.ROTATE[rotation]

        # Initialize GPIO pins and set implementation specific methods
        if implementation.name == 'circuitpython':
            self.cs.switch_to_output(value=True)
            self.dc.switch_to_output(value=False)
            self.rst.switch_to_output(value=True)
            self.reset = self.reset_cpy
            self.write_cmd = self.write_cmd_cpy
            self.write_data = self.write_data_cpy
        else:
            self.cs.init(self.cs.OUT, value=1)
            self.dc.init(self.dc.OUT, value=0)
            self.rst.init(self.rst.OUT, value=1)
            self.reset = self.reset_mpy
            self.write_cmd = self.write_cmd_mpy
            self.write_data = self.write_data_mpy
        self.reset()
        # Send initialization commands
        self.write_cmd(self.SWRESET)  # Software reset
        sleep(.1)
        self.write_cmd(self.PWCTRB, 0x00, 0xC1, 0x30)  # Pwr ctrl B
        self.write_cmd(self.POSC, 0x64, 0x03, 0x12, 0x81)  # Pwr on seq. ctrl
        self.write_cmd(self.DTCA, 0x85, 0x00, 0x78)  # Driver timing ctrl A
        self.write_cmd(self.PWCTRA, 0x39, 0x2C, 0x00, 0x34, 0x02)  # Pwr ctrl A
        self.write_cmd(self.PUMPRC, 0x20)  # Pump ratio control
        self.write_cmd(self.DTCB, 0x00, 0x00)  # Driver timing ctrl B
        self.write_cmd(self.PWCTR1, 0x23)  # Pwr ctrl 1
        self.write_cmd(self.PWCTR2, 0x10)  # Pwr ctrl 2
        self.write_cmd(self.VMCTR1, 0x3E, 0x28)  # VCOM ctrl 1
        self.write_cmd(self.VMCTR2, 0x86)  # VCOM ctrl 2
        self.write_cmd(self.MADCTL, self.rotation)  # Memory access ctrl
        self.write_cmd(self.VSCRSADD, 0x00)  # Vertical scrolling start address
        self.write_cmd(self.PIXFMT, 0x55)  # COLMOD: Pixel format
        self.write_cmd(self.FRMCTR1, 0x00, 0x18)  # Frame rate ctrl
        self.write_cmd(self.DFUNCTR, 0x08, 0x82, 0x27)
        self.write_cmd(self.ENABLE3G, 0x00)  # Enable 3 gamma ctrl
        self.write_cmd(self.GAMMASET, 0x01)  # Gamma curve selected
        self.write_cmd(self.GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E,
                       0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00)
        self.write_cmd(self.GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31,
                       0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F)
        self.write_cmd(self.SLPOUT)  # Exit sleep
        sleep(.1)
        self.write_cmd(self.DISPLAY_ON)  # Display on
        sleep(.1)
        self.clear()

    def block(self, x0, y0, x1, y1, data):
        """Write a block of data to display.

        Args:
            x0 (int):  Starting X position.
            y0 (int):  Starting Y position.
            x1 (int):  Ending X position.
            y1 (int):  Ending Y position.
            data (bytes): Data buffer to write.
        """
        self.write_cmd(self.SET_COLUMN, *ustruct.pack(">HH", x0, x1))
        self.write_cmd(self.SET_PAGE, *ustruct.pack(">HH", y0, y1))

        self.write_cmd(self.WRITE_RAM)
        self.write_data(data)

    def cleanup(self):
        """Clean up resources."""
        self.clear()
        self.display_off()
        self.spi.deinit()
        print('display off')

    def clear(self, color=0):
        """Clear display.

        Args:
            color (Optional int): RGB565 color value (Default: 0 = Black).
        """
        w = self.width
        h = self.height
        # Clear display in 1024 byte blocks
        if color:
            line = color.to_bytes(2, 'big') * (w * 8)
        else:
            line = bytearray(w * 16)
        for y in range(0, h, 8):
            self.block(0, y, w - 1, y + 7, line)

    def display_off(self):
        """Turn display off."""
        self.write_cmd(self.DISPLAY_OFF)

    def display_on(self):
        """Turn display on."""
        self.write_cmd(self.DISPLAY_ON)

    def draw_circle(self, x0, y0, r, color):
        """Draw a circle.

        Args:
            x0 (int): X coordinate of center point.
            y0 (int): Y coordinate of center point.
            r (int): Radius.
            color (int): RGB565 color value.
        """
        f = 1 - r
        dx = 1
        dy = -r - r
        x = 0
        y = r
        self.draw_pixel(x0, y0 + r, color)
        self.draw_pixel(x0, y0 - r, color)
        self.draw_pixel(x0 + r, y0, color)
        self.draw_pixel(x0 - r, y0, color)
        while x < y:
            if f >= 0:
                y -= 1
                dy += 2
                f += dy
            x += 1
            dx += 2
            f += dx
            self.draw_pixel(x0 + x, y0 + y, color)
            self.draw_pixel(x0 - x, y0 + y, color)
            self.draw_pixel(x0 + x, y0 - y, color)
            self.draw_pixel(x0 - x, y0 - y, color)
            self.draw_pixel(x0 + y, y0 + x, color)
            self.draw_pixel(x0 - y, y0 + x, color)
            self.draw_pixel(x0 + y, y0 - x, color)
            self.draw_pixel(x0 - y, y0 - x, color)

    def draw_ellipse(self, x0, y0, a, b, color):
        """Draw an ellipse.

        Args:
            x0, y0 (int): Coordinates of center point.
            a (int): Semi axis horizontal.
            b (int): Semi axis vertical.
            color (int): RGB565 color value.
        Note:
            The center point is the center of the x0,y0 pixel.
            Since pixels are not divisible, the axes are integer rounded
            up to complete on a full pixel.  Therefore the major and
            minor axes are increased by 1.
        """
        a2 = a * a
        b2 = b * b
        twoa2 = a2 + a2
        twob2 = b2 + b2
        x = 0
        y = b
        px = 0
        py = twoa2 * y
        # Plot initial points
        self.draw_pixel(x0 + x, y0 + y, color)
        self.draw_pixel(x0 - x, y0 + y, color)
        self.draw_pixel(x0 + x, y0 - y, color)
        self.draw_pixel(x0 - x, y0 - y, color)
        # Region 1
        p = round(b2 - (a2 * b) + (0.25 * a2))
        while px < py:
            x += 1
            px += twob2
            if p < 0:
                p += b2 + px
            else:
                y -= 1
                py -= twoa2
                p += b2 + px - py
            self.draw_pixel(x0 + x, y0 + y, color)
            self.draw_pixel(x0 - x, y0 + y, color)
            self.draw_pixel(x0 + x, y0 - y, color)
            self.draw_pixel(x0 - x, y0 - y, color)
        # Region 2
        p = round(b2 * (x + 0.5) * (x + 0.5) +
                  a2 * (y - 1) * (y - 1) - a2 * b2)
        while y > 0:
            y -= 1
            py -= twoa2
            if p > 0:
                p += a2 - py
            else:
                x += 1
                px += twob2
                p += a2 - py + px
            self.draw_pixel(x0 + x, y0 + y, color)
            self.draw_pixel(x0 - x, y0 + y, color)
            self.draw_pixel(x0 + x, y0 - y, color)
            self.draw_pixel(x0 - x, y0 - y, color)

    def draw_hline(self, x, y, w, color):
        """Draw a horizontal line.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of line.
            color (int): RGB565 color value.
        """
        if self.is_off_grid(x, y, x + w - 1, y):
            return
        line = color.to_bytes(2, 'big') * w
        self.block(x, y, x + w - 1, y, line)

    def draw_image(self, path, x=0, y=0, w=320, h=240):
        """Draw image from flash.

        Args:
            path (string): Image file path.
            x (int): X coordinate of image left.  Default is 0.
            y (int): Y coordinate of image top.  Default is 0.
            w (int): Width of image.  Default is 320.
            h (int): Height of image.  Default is 240.
        """
        x2 = x + w - 1
        y2 = y + h - 1
        if self.is_off_grid(x, y, x2, y2):
            return
        with open(path, "rb") as f:
            chunk_height = 1024 // w
            chunk_count, remainder = divmod(h, chunk_height)
            chunk_size = chunk_height * w * 2
            chunk_y = y
            if chunk_count:
                for c in range(0, chunk_count):
                    buf = f.read(chunk_size)
                    self.block(x, chunk_y,
                               x2, chunk_y + chunk_height - 1,
                               buf)
                    chunk_y += chunk_height
            if remainder:
                buf = f.read(remainder * w * 2)
                self.block(x, chunk_y,
                           x2, chunk_y + remainder - 1,
                           buf)

    def draw_letter(self, x, y, letter, font, color, background=0,
                    landscape=False):
        """Draw a letter.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            letter (string): Letter to draw.
            font (XglcdFont object): Font.
            color (int): RGB565 color value.
            background (int): RGB565 background color (default: black).
            landscape (bool): Orientation (default: False = portrait)
        """
        buf, w, h = font.get_letter(letter, color, background, landscape)
        # Check for errors (Font could be missing specified letter)
        if w == 0:
            return w, h

        if landscape:
            y -= w
            if self.is_off_grid(x, y, x + h - 1, y + w - 1):
                return 0, 0
            self.block(x, y,
                       x + h - 1, y + w - 1,
                       buf)
        else:
            if self.is_off_grid(x, y, x + w - 1, y + h - 1):
                return 0, 0
            self.block(x, y,
                       x + w - 1, y + h - 1,
                       buf)
        return w, h

    def draw_line(self, x1, y1, x2, y2, color):
        """Draw a line using Bresenham's algorithm.

        Args:
            x1, y1 (int): Starting coordinates of the line
            x2, y2 (int): Ending coordinates of the line
            color (int): RGB565 color value.
        """
        # Check for horizontal line
        if y1 == y2:
            if x1 > x2:
                x1, x2 = x2, x1
            self.draw_hline(x1, y1, x2 - x1 + 1, color)
            return
        # Check for vertical line
        if x1 == x2:
            if y1 > y2:
                y1, y2 = y2, y1
            self.draw_vline(x1, y1, y2 - y1 + 1, color)
            return
        # Confirm coordinates in boundary
        if self.is_off_grid(min(x1, x2), min(y1, y2),
                            max(x1, x2), max(y1, y2)):
            return
        # Changes in x, y
        dx = x2 - x1
        dy = y2 - y1
        # Determine how steep the line is
        is_steep = abs(dy) > abs(dx)
        # Rotate line
        if is_steep:
            x1, y1 = y1, x1
            x2, y2 = y2, x2
        # Swap start and end points if necessary
        if x1 > x2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1
        # Recalculate differentials
        dx = x2 - x1
        dy = y2 - y1
        # Calculate error
        error = dx >> 1
        ystep = 1 if y1 < y2 else -1
        y = y1
        for x in range(x1, x2 + 1):
            # Had to reverse HW ????
            if not is_steep:
                self.draw_pixel(x, y, color)
            else:
                self.draw_pixel(y, x, color)
            error -= abs(dy)
            if error < 0:
                y += ystep
                error += dx

    def draw_lines(self, coords, color):
        """Draw multiple lines.

        Args:
            coords ([[int, int],...]): Line coordinate X, Y pairs
            color (int): RGB565 color value.
        """
        # Starting point
        x1, y1 = coords[0]
        # Iterate through coordinates
        for i in range(1, len(coords)):
            x2, y2 = coords[i]
            self.draw_line(x1, y1, x2, y2, color)
            x1, y1 = x2, y2

    def draw_pixel(self, x, y, color):
        """Draw a single pixel.

        Args:
            x (int): X position.
            y (int): Y position.
            color (int): RGB565 color value.
        """
        if self.is_off_grid(x, y, x, y):
            return
        self.block(x, y, x, y, color.to_bytes(2, 'big'))

    def draw_polygon(self, sides, x0, y0, r, color, rotate=0):
        """Draw an n-sided regular polygon.

        Args:
            sides (int): Number of polygon sides.
            x0, y0 (int): Coordinates of center point.
            r (int): Radius.
            color (int): RGB565 color value.
            rotate (Optional float): Rotation in degrees relative to origin.
        Note:
            The center point is the center of the x0,y0 pixel.
            Since pixels are not divisible, the radius is integer rounded
            up to complete on a full pixel.  Therefore diameter = 2 x r + 1.
        """
        coords = []
        theta = radians(rotate)
        n = sides + 1
        for s in range(n):
            t = 2.0 * pi * s / sides + theta
            coords.append([int(r * cos(t) + x0), int(r * sin(t) + y0)])

        # Cast to python float first to fix rounding errors
        self.draw_lines(coords, color=color)

    def draw_rectangle(self, x, y, w, h, color):
        """Draw a rectangle.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of rectangle.
            h (int): Height of rectangle.
            color (int): RGB565 color value.
        """
        x2 = x + w - 1
        y2 = y + h - 1
        self.draw_hline(x, y, w, color)
        self.draw_hline(x, y2, w, color)
        self.draw_vline(x, y, h, color)
        self.draw_vline(x2, y, h, color)

    def draw_sprite(self, buf, x, y, w, h):
        """Draw a sprite (optimized for horizontal drawing).

        Args:
            buf (bytearray): Buffer to draw.
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of drawing.
            h (int): Height of drawing.
        """
        x2 = x + w - 1
        y2 = y + h - 1
        if self.is_off_grid(x, y, x2, y2):
            return
        self.block(x, y, x2, y2, buf)

    def draw_text(self, x, y, text, font, color,  background=0,
                  landscape=False, spacing=1):
        """Draw text.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            text (string): Text to draw.
            font (XglcdFont object): Font.
            color (int): RGB565 color value.
            background (int): RGB565 background color (default: black).
            landscape (bool): Orientation (default: False = portrait)
            spacing (int): Pixels between letters (default: 1)
        """
        for letter in text:
            # Get letter array and letter dimensions
            w, h = self.draw_letter(x, y, letter, font, color, background,
                                    landscape)
            # Stop on error
            if w == 0 or h == 0:
                print('Invalid width {0} or height {1}'.format(w, h))
                return

            if landscape:
                # Fill in spacing
                if spacing:
                    self.fill_hrect(x, y - w - spacing, h, spacing, background)
                # Position y for next letter
                y -= (w + spacing)
            else:
                # Fill in spacing
                if spacing:
                    self.fill_hrect(x + w, y, spacing, h, background)
                # Position x for next letter
                x += (w + spacing)

                # # Fill in spacing
                # if spacing:
                #     self.fill_vrect(x + w, y, spacing, h, background)
                # # Position x for next letter
                # x += w + spacing

    def draw_text8x8(self, x, y, text, color,  background=0,
                     rotate=0):
        """Draw text using built-in MicroPython 8x8 bit font.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            text (string): Text to draw.
            color (int): RGB565 color value.
            background (int): RGB565 background color (default: black).
            rotate(int): 0, 90, 180, 270
        """
        w = len(text) * 8
        h = 8
        # Confirm coordinates in boundary
        if self.is_off_grid(x, y, x + 7, y + 7):
            return
        # Rearrange color
        r = (color & 0xF800) >> 8
        g = (color & 0x07E0) >> 3
        b = (color & 0x1F) << 3
        buf = bytearray(w * 16)
        fbuf = FrameBuffer(buf, w, h, RGB565)
        if background != 0:
            bg_r = (background & 0xF800) >> 8
            bg_g = (background & 0x07E0) >> 3
            bg_b = (background & 0x1F) << 3
            fbuf.fill(color565(bg_b, bg_r, bg_g))
        fbuf.text(text, 0, 0, color565(b, r, g))
        if rotate == 0:
            self.block(x, y, x + w - 1, y + (h - 1), buf)
        elif rotate == 90:
            buf2 = bytearray(w * 16)
            fbuf2 = FrameBuffer(buf2, h, w, RGB565)
            for y1 in range(h):
                for x1 in range(w):
                    fbuf2.pixel(y1, x1,
                                fbuf.pixel(x1, (h - 1) - y1))
            self.block(x, y, x + (h - 1), y + w - 1, buf2)
        elif rotate == 180:
            buf2 = bytearray(w * 16)
            fbuf2 = FrameBuffer(buf2, w, h, RGB565)
            for y1 in range(h):
                for x1 in range(w):
                    fbuf2.pixel(x1, y1,
                                fbuf.pixel((w - 1) - x1, (h - 1) - y1))
            self.block(x, y, x + w - 1, y + (h - 1), buf2)
        elif rotate == 270:
            buf2 = bytearray(w * 16)
            fbuf2 = FrameBuffer(buf2, h, w, RGB565)
            for y1 in range(h):
                for x1 in range(w):
                    fbuf2.pixel(y1, x1,
                                fbuf.pixel((w - 1) - x1, y1))
            self.block(x, y, x + (h - 1), y + w - 1, buf2)

    def draw_vline(self, x, y, h, color):
        """Draw a vertical line.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            h (int): Height of line.
            color (int): RGB565 color value.
        """
        # Confirm coordinates in boundary
        if self.is_off_grid(x, y, x, y + h - 1):
            return
        line = color.to_bytes(2, 'big') * h
        self.block(x, y, x, y + h - 1, line)

    def fill_circle(self, x0, y0, r, color):
        """Draw a filled circle.

        Args:
            x0 (int): X coordinate of center point.
            y0 (int): Y coordinate of center point.
            r (int): Radius.
            color (int): RGB565 color value.
        """
        f = 1 - r
        dx = 1
        dy = -r - r
        x = 0
        y = r
        self.draw_vline(x0, y0 - r, 2 * r + 1, color)
        while x < y:
            if f >= 0:
                y -= 1
                dy += 2
                f += dy
            x += 1
            dx += 2
            f += dx
            self.draw_vline(x0 + x, y0 - y, 2 * y + 1, color)
            self.draw_vline(x0 - x, y0 - y, 2 * y + 1, color)
            self.draw_vline(x0 - y, y0 - x, 2 * x + 1, color)
            self.draw_vline(x0 + y, y0 - x, 2 * x + 1, color)

    def fill_ellipse(self, x0, y0, a, b, color):
        """Draw a filled ellipse.

        Args:
            x0, y0 (int): Coordinates of center point.
            a (int): Semi axis horizontal.
            b (int): Semi axis vertical.
            color (int): RGB565 color value.
        Note:
            The center point is the center of the x0,y0 pixel.
            Since pixels are not divisible, the axes are integer rounded
            up to complete on a full pixel.  Therefore the major and
            minor axes are increased by 1.
        """
        a2 = a * a
        b2 = b * b
        twoa2 = a2 + a2
        twob2 = b2 + b2
        x = 0
        y = b
        px = 0
        py = twoa2 * y
        # Plot initial points
        self.draw_line(x0, y0 - y, x0, y0 + y, color)
        # Region 1
        p = round(b2 - (a2 * b) + (0.25 * a2))
        while px < py:
            x += 1
            px += twob2
            if p < 0:
                p += b2 + px
            else:
                y -= 1
                py -= twoa2
                p += b2 + px - py
            self.draw_line(x0 + x, y0 - y, x0 + x, y0 + y, color)
            self.draw_line(x0 - x, y0 - y, x0 - x, y0 + y, color)
        # Region 2
        p = round(b2 * (x + 0.5) * (x + 0.5) +
                  a2 * (y - 1) * (y - 1) - a2 * b2)
        while y > 0:
            y -= 1
            py -= twoa2
            if p > 0:
                p += a2 - py
            else:
                x += 1
                px += twob2
                p += a2 - py + px
            self.draw_line(x0 + x, y0 - y, x0 + x, y0 + y, color)
            self.draw_line(x0 - x, y0 - y, x0 - x, y0 + y, color)

    def fill_hrect(self, x, y, w, h, color):
        """Draw a filled rectangle (optimized for horizontal drawing).

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of rectangle.
            h (int): Height of rectangle.
            color (int): RGB565 color value.
        """
        if self.is_off_grid(x, y, x + w - 1, y + h - 1):
            return
        chunk_height = 1024 // w
        chunk_count, remainder = divmod(h, chunk_height)
        chunk_size = chunk_height * w
        chunk_y = y
        if chunk_count:
            buf = color.to_bytes(2, 'big') * chunk_size
            for c in range(0, chunk_count):
                self.block(x, chunk_y,
                           x + w - 1, chunk_y + chunk_height - 1,
                           buf)
                chunk_y += chunk_height

        if remainder:
            buf = color.to_bytes(2, 'big') * remainder * w
            self.block(x, chunk_y,
                       x + w - 1, chunk_y + remainder - 1,
                       buf)

    def fill_rectangle(self, x, y, w, h, color):
        """Draw a filled rectangle.

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of rectangle.
            h (int): Height of rectangle.
            color (int): RGB565 color value.
        """
        if self.is_off_grid(x, y, x + w - 1, y + h - 1):
            return
        if w > h:
            self.fill_hrect(x, y, w, h, color)
        else:
            self.fill_vrect(x, y, w, h, color)

    def fill_polygon(self, sides, x0, y0, r, color, rotate=0):
        """Draw a filled n-sided regular polygon.

        Args:
            sides (int): Number of polygon sides.
            x0, y0 (int): Coordinates of center point.
            r (int): Radius.
            color (int): RGB565 color value.
            rotate (Optional float): Rotation in degrees relative to origin.
        Note:
            The center point is the center of the x0,y0 pixel.
            Since pixels are not divisible, the radius is integer rounded
            up to complete on a full pixel.  Therefore diameter = 2 x r + 1.
        """
        # Determine side coordinates
        coords = []
        theta = radians(rotate)
        n = sides + 1
        for s in range(n):
            t = 2.0 * pi * s / sides + theta
            coords.append([int(r * cos(t) + x0), int(r * sin(t) + y0)])
        # Starting point
        x1, y1 = coords[0]
        # Minimum Maximum X dict
        xdict = {y1: [x1, x1]}
        # Iterate through coordinates
        for row in coords[1:]:
            x2, y2 = row
            xprev, yprev = x2, y2
            # Calculate perimeter
            # Check for horizontal side
            if y1 == y2:
                if x1 > x2:
                    x1, x2 = x2, x1
                if y1 in xdict:
                    xdict[y1] = [min(x1, xdict[y1][0]), max(x2, xdict[y1][1])]
                else:
                    xdict[y1] = [x1, x2]
                x1, y1 = xprev, yprev
                continue
            # Non horizontal side
            # Changes in x, y
            dx = x2 - x1
            dy = y2 - y1
            # Determine how steep the line is
            is_steep = abs(dy) > abs(dx)
            # Rotate line
            if is_steep:
                x1, y1 = y1, x1
                x2, y2 = y2, x2
            # Swap start and end points if necessary
            if x1 > x2:
                x1, x2 = x2, x1
                y1, y2 = y2, y1
            # Recalculate differentials
            dx = x2 - x1
            dy = y2 - y1
            # Calculate error
            error = dx >> 1
            ystep = 1 if y1 < y2 else -1
            y = y1
            # Calcualte minimum and maximum x values
            for x in range(x1, x2 + 1):
                if is_steep:
                    if x in xdict:
                        xdict[x] = [min(y, xdict[x][0]), max(y, xdict[x][1])]
                    else:
                        xdict[x] = [y, y]
                else:
                    if y in xdict:
                        xdict[y] = [min(x, xdict[y][0]), max(x, xdict[y][1])]
                    else:
                        xdict[y] = [x, x]
                error -= abs(dy)
                if error < 0:
                    y += ystep
                    error += dx
            x1, y1 = xprev, yprev
        # Fill polygon
        for y, x in xdict.items():
            self.draw_hline(x[0], y, x[1] - x[0] + 2, color)

    def fill_vrect(self, x, y, w, h, color):
        """Draw a filled rectangle (optimized for vertical drawing).

        Args:
            x (int): Starting X position.
            y (int): Starting Y position.
            w (int): Width of rectangle.
            h (int): Height of rectangle.
            color (int): RGB565 color value.
        """
        if self.is_off_grid(x, y, x + w - 1, y + h - 1):
            return
        chunk_width = 1024 // h
        chunk_count, remainder = divmod(w, chunk_width)
        chunk_size = chunk_width * h
        chunk_x = x
        if chunk_count:
            buf = color.to_bytes(2, 'big') * chunk_size
            for c in range(0, chunk_count):
                self.block(chunk_x, y,
                           chunk_x + chunk_width - 1, y + h - 1,
                           buf)
                chunk_x += chunk_width

        if remainder:
            buf = color.to_bytes(2, 'big') * remainder * h
            self.block(chunk_x, y,
                       chunk_x + remainder - 1, y + h - 1,
                       buf)

    def is_off_grid(self, xmin, ymin, xmax, ymax):
        """Check if coordinates extend past display boundaries.

        Args:
            xmin (int): Minimum horizontal pixel.
            ymin (int): Minimum vertical pixel.
            xmax (int): Maximum horizontal pixel.
            ymax (int): Maximum vertical pixel.
        Returns:
            boolean: False = Coordinates OK, True = Error.
        """
        if xmin < 0:
            print('x-coordinate: {0} below minimum of 0.'.format(xmin))
            return True
        if ymin < 0:
            print('y-coordinate: {0} below minimum of 0.'.format(ymin))
            return True
        if xmax >= self.width:
            print('x-coordinate: {0} above maximum of {1}.'.format(
                xmax, self.width - 1))
            return True
        if ymax >= self.height:
            print('y-coordinate: {0} above maximum of {1}.'.format(
                ymax, self.height - 1))
            return True
        return False

    def load_sprite(self, path, w, h):
        """Load sprite image.

        Args:
            path (string): Image file path.
            w (int): Width of image.
            h (int): Height of image.
        Notes:
            w x h cannot exceed 2048
        """
        buf_size = w * h * 2
        with open(path, "rb") as f:
            return f.read(buf_size)

    def reset_cpy(self):
        """Perform reset: Low=initialization, High=normal operation.

        Notes: CircuitPython implemntation
        """
        self.rst.value = False
        sleep(.05)
        self.rst.value = True
        sleep(.05)

    def reset_mpy(self):
        """Perform reset: Low=initialization, High=normal operation.

        Notes: MicroPython implemntation
        """
        self.rst(0)
        sleep(.05)
        self.rst(1)
        sleep(.05)

    def scroll(self, y):
        """Scroll display vertically.

        Args:
            y (int): Number of pixels to scroll display.
        """
        self.write_cmd(self.VSCRSADD, y >> 8, y & 0xFF)

    def set_scroll(self, top, bottom):
        """Set the height of the top and bottom scroll margins.

        Args:
            top (int): Height of top scroll margin
            bottom (int): Height of bottom scroll margin
        """
        if top + bottom <= self.height:
            middle = self.height - (top + bottom)
            print(top, middle, bottom)
            self.write_cmd(self.VSCRDEF,
                           top >> 8,
                           top & 0xFF,
                           middle >> 8,
                           middle & 0xFF,
                           bottom >> 8,
                           bottom & 0xFF)

    def sleep(self, enable=True):
        """Enters or exits sleep mode.

        Args:
            enable (bool): True (default)=Enter sleep mode, False=Exit sleep
        """
        if enable:
            self.write_cmd(self.SLPIN)
        else:
            self.write_cmd(self.SLPOUT)


    def write_cmd_mpy(self, command, *args):
        """Write command to OLED (MicroPython).

        Args:
            command (byte): ILI9341 command code.
            *args (optional bytes): Data to transmit.
        """
        self.dc(0)
        self.cs(0)
        self.spi.write(bytearray([command]))
        self.cs(1)
        # Handle any passed data
        if len(args) > 0:
            self.write_data(bytearray(args))

    def write_cmd_cpy(self, command, *args):
        """Write command to OLED (CircuitPython).

        Args:
            command (byte): ILI9341 command code.
            *args (optional bytes): Data to transmit.
        """
        self.dc.value = False
        self.cs.value = False
        # Confirm SPI locked before writing
        while not self.spi.try_lock():
            pass
        self.spi.write(bytearray([command]))
        self.spi.unlock()
        self.cs.value = True
        # Handle any passed data
        if len(args) > 0:
            self.write_data(bytearray(args))

    def write_data_mpy(self, data):
        """Write data to OLED (MicroPython).

        Args:
            data (bytes): Data to transmit.
        """
        self.dc(1)
        self.cs(0)
        self.spi.write(data)
        self.cs(1)

    def write_data_cpy(self, data):
        """Write data to OLED (CircuitPython).

        Args:
            data (bytes): Data to transmit.
        """
        self.dc.value = True
        self.cs.value = False
        # Confirm SPI locked before writing
        while not self.spi.try_lock():
            pass
        self.spi.write(data)
        self.spi.unlock()
        self.cs.value = True

#driver for Sainsmart 1.8" TFT display ST7735
#Translated by Guy Carver from the ST7735 sample code.
#Modirfied for micropython-esp32 by boochow 

import machine
import time
from math import sqrt

#TFTRotations and TFTRGB are bits to set
# on MADCTL to control display rotation/color layout
#Looking at display with pins on top.
#00 = upper left printing right
#10 = does nothing (MADCTL_ML)
#20 = upper left printing down (backwards) (Vertical flip)
#40 = upper right printing left (backwards) (X Flip)
#80 = lower left printing right (backwards) (Y Flip)
#04 = (MADCTL_MH)

#60 = 90 right rotation
#C0 = 180 right rotation
#A0 = 270 right rotation
TFTRotations = [0x00, 0x60, 0xC0, 0xA0]
TFTBGR = 0x08 #When set color is bgr else rgb.
TFTRGB = 0x00

#@micropython.native
def clamp( aValue, aMin, aMax ) :
  return max(aMin, min(aMax, aValue))

#@micropython.native
def TFTColor( aR, aG, aB ) :
  '''Create a 16 bit rgb value from the given R,G,B from 0-255.
     This assumes rgb 565 layout and will be incorrect for bgr.'''
  return ((aR & 0xF8) << 8) | ((aG & 0xFC) << 3) | (aB >> 3)

ScreenSize = (128, 160)

class TFT(object) :
  """Sainsmart TFT 7735 display driver."""

  NOP = 0x0
  SWRESET = 0x01
  RDDID = 0x04
  RDDST = 0x09

  SLPIN  = 0x10
  SLPOUT  = 0x11
  PTLON  = 0x12
  NORON  = 0x13

  INVOFF = 0x20
  INVON = 0x21
  DISPOFF = 0x28
  DISPON = 0x29
  CASET = 0x2A
  RASET = 0x2B
  RAMWR = 0x2C
  RAMRD = 0x2E

  VSCRDEF = 0x33
  VSCSAD = 0x37

  COLMOD = 0x3A
  MADCTL = 0x36

  FRMCTR1 = 0xB1
  FRMCTR2 = 0xB2
  FRMCTR3 = 0xB3
  INVCTR = 0xB4
  DISSET5 = 0xB6

  PWCTR1 = 0xC0
  PWCTR2 = 0xC1
  PWCTR3 = 0xC2
  PWCTR4 = 0xC3
  PWCTR5 = 0xC4
  VMCTR1 = 0xC5

  RDID1 = 0xDA
  RDID2 = 0xDB
  RDID3 = 0xDC
  RDID4 = 0xDD

  PWCTR6 = 0xFC

  GMCTRP1 = 0xE0
  GMCTRN1 = 0xE1

  BLACK = 0
  RED = TFTColor(0xFF, 0x00, 0x00)
  MAROON = TFTColor(0x80, 0x00, 0x00)
  GREEN = TFTColor(0x00, 0xFF, 0x00)
  FOREST = TFTColor(0x00, 0x80, 0x80)
  BLUE = TFTColor(0x00, 0x00, 0xFF)
  NAVY = TFTColor(0x00, 0x00, 0x80)
  CYAN = TFTColor(0x00, 0xFF, 0xFF)
  YELLOW = TFTColor(0xFF, 0xFF, 0x00)
  PURPLE = TFTColor(0xFF, 0x00, 0xFF)
  WHITE = TFTColor(0xFF, 0xFF, 0xFF)
  GRAY = TFTColor(0x80, 0x80, 0x80)

  @staticmethod
  def color( aR, aG, aB ) :
    '''Create a 565 rgb TFTColor value'''
    return TFTColor(aR, aG, aB)

  def __init__( self, spi, aDC, aReset, aCS) :
    """aLoc SPI pin location is either 1 for 'X' or 2 for 'Y'.
       aDC is the DC pin and aReset is the reset pin."""
    self._size = ScreenSize
    self._offset = bytearray([0,0])
    self.rotate = 0                    #Vertical with top toward pins.
    self._rgb = True                   #color order of rgb.
    self.tfa = 0                       #top fixed area
    self.bfa = 0                       #bottom fixed area
    self.dc  = machine.Pin(aDC, machine.Pin.OUT, machine.Pin.PULL_DOWN)
    self.reset = machine.Pin(aReset, machine.Pin.OUT, machine.Pin.PULL_DOWN)
    self.cs = machine.Pin(aCS, machine.Pin.OUT, machine.Pin.PULL_DOWN)
    self.cs(1)
    self.spi = spi
    self.colorData = bytearray(2)
    self.windowLocData = bytearray(4)

  def size( self ) :
    return self._size

#   @micropython.native
  def on( self, aTF = True ) :
    '''Turn display on or off.'''
    self._writecommand(TFT.DISPON if aTF else TFT.DISPOFF)

#   @micropython.native
  def invertcolor( self, aBool ) :
    '''Invert the color data IE: Black = White.'''
    self._writecommand(TFT.INVON if aBool else TFT.INVOFF)

#   @micropython.native
  def rgb( self, aTF = True ) :
    '''True = rgb else bgr'''
    self._rgb = aTF
    self._setMADCTL()

#   @micropython.native
  def rotation( self, aRot ) :
    '''0 - 3. Starts vertical with top toward pins and rotates 90 deg
       clockwise each step.'''
    if (0 <= aRot < 4):
      rotchange = self.rotate ^ aRot
      self.rotate = aRot
      #If switching from vertical to horizontal swap x,y
      # (indicated by bit 0 changing).
      if (rotchange & 1):
        self._size =(self._size[1], self._size[0])
      self._setMADCTL()

#  @micropython.native
  def pixel( self, aPos, aColor ) :
    '''Draw a pixel at the given position'''
    if 0 <= aPos[0] < self._size[0] and 0 <= aPos[1] < self._size[1]:
      self._setwindowpoint(aPos)
      self._pushcolor(aColor)

#   @micropython.native
  def text( self, aPos, aString, aColor, aFont, aSize = 1, nowrap = False ) :
    '''Draw a text at the given position.  If the string reaches the end of the
       display it is wrapped to aPos[0] on the next line.  aSize may be an integer
       which will size the font uniformly on w,h or a or any type that may be
       indexed with [0] or [1].'''

    if aFont == None:
      return

    #Make a size either from single value or 2 elements.
    if (type(aSize) == int) or (type(aSize) == float):
      wh = (aSize, aSize)
    else:
      wh = aSize

    px, py = aPos
    width = wh[0] * aFont["Width"] + 1
    for c in aString:
      self.char((px, py), c, aColor, aFont, wh)
      px += width
      #We check > rather than >= to let the right (blank) edge of the
      # character print off the right of the screen.
      if px + width > self._size[0]:
        if nowrap:
          break
        else:
          py += aFont["Height"] * wh[1] + 1
          px = aPos[0]

#   @micropython.native
  def char( self, aPos, aChar, aColor, aFont, aSizes ) :
    '''Draw a character at the given position using the given font and color.
       aSizes is a tuple with x, y as integer scales indicating the
       # of pixels to draw for each pixel in the character.'''

    if aFont == None:
      return

    startchar = aFont['Start']
    endchar = aFont['End']

    ci = ord(aChar)
    if (startchar <= ci <= endchar):
      fontw = aFont['Width']
      fonth = aFont['Height']
      ci = (ci - startchar) * fontw

      charA = aFont["Data"][ci:ci + fontw]
      px = aPos[0]
      if aSizes[0] <= 1 and aSizes[1] <= 1 :
        buf = bytearray(2 * fonth * fontw)
        for q in range(fontw) :
          c = charA[q]
          for r in range(fonth) :
            if c & 0x01 :
              pos = 2 * (r * fontw + q)
              buf[pos] = aColor >> 8
              buf[pos + 1] = aColor & 0xff
            c >>= 1
        self.image(aPos[0], aPos[1], aPos[0] + fontw - 1, aPos[1] + fonth - 1, buf)
      else:
        for c in charA :
          py = aPos[1]
          for r in range(fonth) :
            if c & 0x01 :
              self.fillrect((px, py), aSizes, aColor)
            py += aSizes[1]
            c >>= 1
          px += aSizes[0]

#   @micropython.native
  def line( self, aStart, aEnd, aColor ) :
    '''Draws a line from aStart to aEnd in the given color.  Vertical or horizontal
       lines are forwarded to vline and hline.'''
    if aStart[0] == aEnd[0]:
      #Make sure we use the smallest y.
      pnt = aEnd if (aEnd[1] < aStart[1]) else aStart
      self.vline(pnt, abs(aEnd[1] - aStart[1]) + 1, aColor)
    elif aStart[1] == aEnd[1]:
      #Make sure we use the smallest x.
      pnt = aEnd if aEnd[0] < aStart[0] else aStart
      self.hline(pnt, abs(aEnd[0] - aStart[0]) + 1, aColor)
    else:
      px, py = aStart
      ex, ey = aEnd
      dx = ex - px
      dy = ey - py
      inx = 1 if dx > 0 else -1
      iny = 1 if dy > 0 else -1

      dx = abs(dx)
      dy = abs(dy)
      if (dx >= dy):
        dy <<= 1
        e = dy - dx
        dx <<= 1
        while (px != ex):
          self.pixel((px, py), aColor)
          if (e >= 0):
            py += iny
            e -= dx
          e += dy
          px += inx
      else:
        dx <<= 1
        e = dx - dy
        dy <<= 1
        while (py != ey):
          self.pixel((px, py), aColor)
          if (e >= 0):
            px += inx
            e -= dy
          e += dx
          py += iny

#   @micropython.native
  def vline( self, aStart, aLen, aColor ) :
    '''Draw a vertical line from aStart for aLen. aLen may be negative.'''
    start = (clamp(aStart[0], 0, self._size[0]), clamp(aStart[1], 0, self._size[1]))
    stop = (start[0], clamp(start[1] + aLen, 0, self._size[1]))
    #Make sure smallest y 1st.
    if (stop[1] < start[1]):
      start, stop = stop, start
    self._setwindowloc(start, stop)
    self._setColor(aColor)
    self._draw(aLen)

#   @micropython.native
  def hline( self, aStart, aLen, aColor ) :
    '''Draw a horizontal line from aStart for aLen. aLen may be negative.'''
    start = (clamp(aStart[0], 0, self._size[0]), clamp(aStart[1], 0, self._size[1]))
    stop = (clamp(start[0] + aLen, 0, self._size[0]), start[1])
    #Make sure smallest x 1st.
    if (stop[0] < start[0]):
      start, stop = stop, start
    self._setwindowloc(start, stop)
    self._setColor(aColor)
    self._draw(aLen)

#   @micropython.native
  def rect( self, aStart, aSize, aColor ) :
    '''Draw a hollow rectangle.  aStart is the smallest coordinate corner
       and aSize is a tuple indicating width, height.'''
    self.hline(aStart, aSize[0], aColor)
    self.hline((aStart[0], aStart[1] + aSize[1] - 1), aSize[0], aColor)
    self.vline(aStart, aSize[1], aColor)
    self.vline((aStart[0] + aSize[0] - 1, aStart[1]), aSize[1], aColor)

#   @micropython.native
  def fillrect( self, aStart, aSize, aColor ) :
    '''Draw a filled rectangle.  aStart is the smallest coordinate corner
       and aSize is a tuple indicating width, height.'''
    start = (clamp(aStart[0], 0, self._size[0]), clamp(aStart[1], 0, self._size[1]))
    end = (clamp(start[0] + aSize[0] - 1, 0, self._size[0]), clamp(start[1] + aSize[1] - 1, 0, self._size[1]))

    if (end[0] < start[0]):
      tmp = end[0]
      end = (start[0], end[1])
      start = (tmp, start[1])
    if (end[1] < start[1]):
      tmp = end[1]
      end = (end[0], start[1])
      start = (start[0], tmp)

    self._setwindowloc(start, end)
    numPixels = (end[0] - start[0] + 1) * (end[1] - start[1] + 1)
    self._setColor(aColor)
    self._draw(numPixels)

#   @micropython.native
  def circle( self, aPos, aRadius, aColor ) :
    '''Draw a hollow circle with the given radius and color with aPos as center.'''
    self.colorData[0] = aColor >> 8
    self.colorData[1] = aColor
    xend = int(0.7071 * aRadius) + 1
    rsq = aRadius * aRadius
    for x in range(xend) :
      y = int(sqrt(rsq - x * x))
      xp = aPos[0] + x
      yp = aPos[1] + y
      xn = aPos[0] - x
      yn = aPos[1] - y
      xyp = aPos[0] + y
      yxp = aPos[1] + x
      xyn = aPos[0] - y
      yxn = aPos[1] - x

      self._setwindowpoint((xp, yp))
      self._writedata(self.colorData)
      self._setwindowpoint((xp, yn))
      self._writedata(self.colorData)
      self._setwindowpoint((xn, yp))
      self._writedata(self.colorData)
      self._setwindowpoint((xn, yn))
      self._writedata(self.colorData)
      self._setwindowpoint((xyp, yxp))
      self._writedata(self.colorData)
      self._setwindowpoint((xyp, yxn))
      self._writedata(self.colorData)
      self._setwindowpoint((xyn, yxp))
      self._writedata(self.colorData)
      self._setwindowpoint((xyn, yxn))
      self._writedata(self.colorData)

#   @micropython.native
  def fillcircle( self, aPos, aRadius, aColor ) :
    '''Draw a filled circle with given radius and color with aPos as center'''
    rsq = aRadius * aRadius
    for x in range(aRadius) :
      y = int(sqrt(rsq - x * x))
      y0 = aPos[1] - y
      ey = y0 + y * 2
      y0 = clamp(y0, 0, self._size[1])
      ln = abs(ey - y0) + 1;

      self.vline((aPos[0] + x, y0), ln, aColor)
      self.vline((aPos[0] - x, y0), ln, aColor)

  def fill( self, aColor = BLACK ) :
    '''Fill screen with the given color.'''
    self.fillrect((0, 0), self._size, aColor)

  def image( self, x0, y0, x1, y1, data ) :
    self._setwindowloc((x0, y0), (x1, y1))
    self._writedata(data)

  def setvscroll(self, tfa, bfa) :
    ''' set vertical scroll area '''
    self._writecommand(TFT.VSCRDEF)
    data2 = bytearray([0, tfa])
    self._writedata(data2)
    data2[1] = 162 - tfa - bfa
    self._writedata(data2)
    data2[1] = bfa
    self._writedata(data2)
    self.tfa = tfa
    self.bfa = bfa

  def vscroll(self, value) :
    a = value + self.tfa
    if (a + self.bfa > 162) :
      a = 162 - self.bfa
    self._vscrolladdr(a)

  def _vscrolladdr(self, addr) :
    self._writecommand(TFT.VSCSAD)
    data2 = bytearray([addr >> 8, addr & 0xff])
    self._writedata(data2)
    
#   @micropython.native
  def _setColor( self, aColor ) :
    self.colorData[0] = aColor >> 8
    self.colorData[1] = aColor
    self.buf = bytes(self.colorData) * 32

#   @micropython.native
  def _draw( self, aPixels ) :
    '''Send given color to the device aPixels times.'''

    self.dc(1)
    self.cs(0)
    for i in range(aPixels//32):
      self.spi.write(self.buf)
    rest = (int(aPixels) % 32)
    if rest > 0:
        buf2 = bytes(self.colorData) * rest
        self.spi.write(buf2)
    self.cs(1)

#   @micropython.native
  def _setwindowpoint( self, aPos ) :
    '''Set a single point for drawing a color to.'''
    x = self._offset[0] + int(aPos[0])
    y = self._offset[1] + int(aPos[1])
    self._writecommand(TFT.CASET)            #Column address set.
    self.windowLocData[0] = self._offset[0]
    self.windowLocData[1] = x
    self.windowLocData[2] = self._offset[0]
    self.windowLocData[3] = x
    self._writedata(self.windowLocData)

    self._writecommand(TFT.RASET)            #Row address set.
    self.windowLocData[0] = self._offset[1]
    self.windowLocData[1] = y
    self.windowLocData[2] = self._offset[1]
    self.windowLocData[3] = y
    self._writedata(self.windowLocData)
    self._writecommand(TFT.RAMWR)            #Write to RAM.

#   @micropython.native
  def _setwindowloc( self, aPos0, aPos1 ) :
    '''Set a rectangular area for drawing a color to.'''
    self._writecommand(TFT.CASET)            #Column address set.
    self.windowLocData[0] = self._offset[0]
    self.windowLocData[1] = self._offset[0] + int(aPos0[0])
    self.windowLocData[2] = self._offset[0]
    self.windowLocData[3] = self._offset[0] + int(aPos1[0])
    self._writedata(self.windowLocData)

    self._writecommand(TFT.RASET)            #Row address set.
    self.windowLocData[0] = self._offset[1]
    self.windowLocData[1] = self._offset[1] + int(aPos0[1])
    self.windowLocData[2] = self._offset[1]
    self.windowLocData[3] = self._offset[1] + int(aPos1[1])
    self._writedata(self.windowLocData)

    self._writecommand(TFT.RAMWR)            #Write to RAM.

  #@micropython.native
  def _writecommand( self, aCommand ) :
    '''Write given command to the device.'''
    self.dc(0)
    self.cs(0)
    self.spi.write(bytearray([aCommand]))
    self.cs(1)

  #@micropython.native
  def _writedata( self, aData ) :
    '''Write given data to the device.  This may be
       either a single int or a bytearray of values.'''
    self.dc(1)
    self.cs(0)
    self.spi.write(aData)
    self.cs(1)

  #@micropython.native
  def _pushcolor( self, aColor ) :
    '''Push given color to the device.'''
    self.colorData[0] = aColor >> 8
    self.colorData[1] = aColor
    self._writedata(self.colorData)

  #@micropython.native
  def _setMADCTL( self ) :
    '''Set screen rotation and RGB/BGR format.'''
    self._writecommand(TFT.MADCTL)
    rgb = TFTRGB if self._rgb else TFTBGR
    self._writedata(bytearray([TFTRotations[self.rotate] | rgb]))

  #@micropython.native
  def _reset( self ) :
    '''Reset the device.'''
    self.dc(0)
    self.reset(1)
    time.sleep_us(500)
    self.reset(0)
    time.sleep_us(500)
    self.reset(1)
    time.sleep_us(500)

  def initb( self ) :
    '''Initialize blue tab version.'''
    self._size = (ScreenSize[0] + 2, ScreenSize[1] + 1)
    self._reset()
    self._writecommand(TFT.SWRESET)              #Software reset.
    time.sleep_us(50)
    self._writecommand(TFT.SLPOUT)               #out of sleep mode.
    time.sleep_us(500)

    data1 = bytearray(1)
    self._writecommand(TFT.COLMOD)               #Set color mode.
    data1[0] = 0x05                             #16 bit color.
    self._writedata(data1)
    time.sleep_us(10)

    data3 = bytearray([0x00, 0x06, 0x03])       #fastest refresh, 6 lines front, 3 lines back.
    self._writecommand(TFT.FRMCTR1)              #Frame rate control.
    self._writedata(data3)
    time.sleep_us(10)

    self._writecommand(TFT.MADCTL)
    data1[0] = 0x08                             #row address/col address, bottom to top refresh
    self._writedata(data1)

    data2 = bytearray(2)
    self._writecommand(TFT.DISSET5)              #Display settings
    data2[0] = 0x15                             #1 clock cycle nonoverlap, 2 cycle gate rise, 3 cycle oscil, equalize
    data2[1] = 0x02                             #fix on VTL
    self._writedata(data2)

    self._writecommand(TFT.INVCTR)               #Display inversion control
    data1[0] = 0x00                             #Line inversion.
    self._writedata(data1)

    self._writecommand(TFT.PWCTR1)               #Power control
    data2[0] = 0x02   #GVDD = 4.7V
    data2[1] = 0x70   #1.0uA
    self._writedata(data2)
    time.sleep_us(10)

    self._writecommand(TFT.PWCTR2)               #Power control
    data1[0] = 0x05                             #VGH = 14.7V, VGL = -7.35V
    self._writedata(data1)

    self._writecommand(TFT.PWCTR3)           #Power control
    data2[0] = 0x01   #Opamp current small
    data2[1] = 0x02   #Boost frequency
    self._writedata(data2)

    self._writecommand(TFT.VMCTR1)               #Power control
    data2[0] = 0x3C   #VCOMH = 4V
    data2[1] = 0x38   #VCOML = -1.1V
    self._writedata(data2)
    time.sleep_us(10)

    self._writecommand(TFT.PWCTR6)               #Power control
    data2[0] = 0x11
    data2[1] = 0x15
    self._writedata(data2)

    #These different values don't seem to make a difference.
#     dataGMCTRP = bytearray([0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22, 0x1f,
#                             0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10])
    dataGMCTRP = bytearray([0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29,
                            0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10])
    self._writecommand(TFT.GMCTRP1)
    self._writedata(dataGMCTRP)

#     dataGMCTRN = bytearray([0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e, 0x30,
#                             0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10])
    dataGMCTRN = bytearray([0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29, 0x2d, 0x2e,
                            0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10])
    self._writecommand(TFT.GMCTRN1)
    self._writedata(dataGMCTRN)
    time.sleep_us(10)

    self._writecommand(TFT.CASET)                #Column address set.
    self.windowLocData[0] = 0x00
    self.windowLocData[1] = 2                   #Start at column 2
    self.windowLocData[2] = 0x00
    self.windowLocData[3] = self._size[0] - 1
    self._writedata(self.windowLocData)

    self._writecommand(TFT.RASET)                #Row address set.
    self.windowLocData[1] = 1                   #Start at row 2.
    self.windowLocData[3] = self._size[1] - 1
    self._writedata(self.windowLocData)

    self._writecommand(TFT.NORON)                #Normal display on.
    time.sleep_us(10)

    self._writecommand(TFT.RAMWR)
    time.sleep_us(500)

    self._writecommand(TFT.DISPON)
    self.cs(1)
    time.sleep_us(500)

  def initr( self ) :
    '''Initialize a red tab version.'''
    self._reset()

    self._writecommand(TFT.SWRESET)              #Software reset.
    time.sleep_us(150)
    self._writecommand(TFT.SLPOUT)               #out of sleep mode.
    time.sleep_us(500)

    data3 = bytearray([0x01, 0x2C, 0x2D])       #fastest refresh, 6 lines front, 3 lines back.
    self._writecommand(TFT.FRMCTR1)              #Frame rate control.
    self._writedata(data3)

    self._writecommand(TFT.FRMCTR2)              #Frame rate control.
    self._writedata(data3)

    data6 = bytearray([0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d])
    self._writecommand(TFT.FRMCTR3)              #Frame rate control.
    self._writedata(data6)
    time.sleep_us(10)

    data1 = bytearray(1)
    self._writecommand(TFT.INVCTR)               #Display inversion control
    data1[0] = 0x07                             #Line inversion.
    self._writedata(data1)

    self._writecommand(TFT.PWCTR1)               #Power control
    data3[0] = 0xA2
    data3[1] = 0x02
    data3[2] = 0x84
    self._writedata(data3)

    self._writecommand(TFT.PWCTR2)               #Power control
    data1[0] = 0xC5   #VGH = 14.7V, VGL = -7.35V
    self._writedata(data1)

    data2 = bytearray(2)
    self._writecommand(TFT.PWCTR3)               #Power control
    data2[0] = 0x0A   #Opamp current small
    data2[1] = 0x00   #Boost frequency
    self._writedata(data2)

    self._writecommand(TFT.PWCTR4)               #Power control
    data2[0] = 0x8A   #Opamp current small
    data2[1] = 0x2A   #Boost frequency
    self._writedata(data2)

    self._writecommand(TFT.PWCTR5)               #Power control
    data2[0] = 0x8A   #Opamp current small
    data2[1] = 0xEE   #Boost frequency
    self._writedata(data2)

    self._writecommand(TFT.VMCTR1)               #Power control
    data1[0] = 0x0E
    self._writedata(data1)

    self._writecommand(TFT.INVOFF)

    self._writecommand(TFT.MADCTL)               #Power control
    data1[0] = 0xC8
    self._writedata(data1)

    self._writecommand(TFT.COLMOD)
    data1[0] = 0x05
    self._writedata(data1)

    self._writecommand(TFT.CASET)                #Column address set.
    self.windowLocData[0] = 0x00
    self.windowLocData[1] = 0x00
    self.windowLocData[2] = 0x00
    self.windowLocData[3] = self._size[0] - 1
    self._writedata(self.windowLocData)

    self._writecommand(TFT.RASET)                #Row address set.
    self.windowLocData[3] = self._size[1] - 1
    self._writedata(self.windowLocData)

    dataGMCTRP = bytearray([0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22, 0x1f,
                            0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10])
    self._writecommand(TFT.GMCTRP1)
    self._writedata(dataGMCTRP)

    dataGMCTRN = bytearray([0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e, 0x30,
                            0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10])
    self._writecommand(TFT.GMCTRN1)
    self._writedata(dataGMCTRN)
    time.sleep_us(10)

    self._writecommand(TFT.DISPON)
    time.sleep_us(100)

    self._writecommand(TFT.NORON)                #Normal display on.
    time.sleep_us(10)

    self.cs(1)
    
  def initb2( self ) :
    '''Initialize another blue tab version.'''
    self._size = (ScreenSize[0] + 2, ScreenSize[1] + 1)
    self._offset[0] = 2
    self._offset[1] = 1
    self._reset()
    self._writecommand(TFT.SWRESET)              #Software reset.
    time.sleep_us(50)
    self._writecommand(TFT.SLPOUT)               #out of sleep mode.
    time.sleep_us(500)

    data3 = bytearray([0x01, 0x2C, 0x2D])        #
    self._writecommand(TFT.FRMCTR1)              #Frame rate control.
    self._writedata(data3)
    time.sleep_us(10)

    self._writecommand(TFT.FRMCTR2)              #Frame rate control.
    self._writedata(data3)
    time.sleep_us(10)

    self._writecommand(TFT.FRMCTR3)              #Frame rate control.
    self._writedata(data3)
    time.sleep_us(10)

    self._writecommand(TFT.INVCTR)               #Display inversion control
    data1 = bytearray(1)                         #
    data1[0] = 0x07
    self._writedata(data1)

    self._writecommand(TFT.PWCTR1)               #Power control
    data3[0] = 0xA2   #
    data3[1] = 0x02   #
    data3[2] = 0x84   #
    self._writedata(data3)
    time.sleep_us(10)

    self._writecommand(TFT.PWCTR2)               #Power control
    data1[0] = 0xC5                              #
    self._writedata(data1)

    self._writecommand(TFT.PWCTR3)           #Power control
    data2 = bytearray(2)
    data2[0] = 0x0A   #
    data2[1] = 0x00   #
    self._writedata(data2)

    self._writecommand(TFT.PWCTR4)           #Power control
    data2[0] = 0x8A   #
    data2[1] = 0x2A   #
    self._writedata(data2)

    self._writecommand(TFT.PWCTR5)           #Power control
    data2[0] = 0x8A   #
    data2[1] = 0xEE   #
    self._writedata(data2)

    self._writecommand(TFT.VMCTR1)               #Power control
    data1[0] = 0x0E   #
    self._writedata(data1)
    time.sleep_us(10)

    self._writecommand(TFT.MADCTL)
    data1[0] = 0xC8                             #row address/col address, bottom to top refresh
    self._writedata(data1)

#These different values don't seem to make a difference.
#     dataGMCTRP = bytearray([0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22, 0x1f,
#                             0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10])
    dataGMCTRP = bytearray([0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29,
                            0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10])
    self._writecommand(TFT.GMCTRP1)
    self._writedata(dataGMCTRP)

#     dataGMCTRN = bytearray([0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e, 0x30,
#                             0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10])
    dataGMCTRN = bytearray([0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29, 0x2d, 0x2e,
                            0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10])
    self._writecommand(TFT.GMCTRN1)
    self._writedata(dataGMCTRN)
    time.sleep_us(10)

    self._writecommand(TFT.CASET)                #Column address set.
    self.windowLocData[0] = 0x00
    self.windowLocData[1] = 0x02                   #Start at column 2
    self.windowLocData[2] = 0x00
    self.windowLocData[3] = self._size[0] - 1
    self._writedata(self.windowLocData)

    self._writecommand(TFT.RASET)                #Row address set.
    self.windowLocData[1] = 0x01                   #Start at row 2.
    self.windowLocData[3] = self._size[1] - 1
    self._writedata(self.windowLocData)

    data1 = bytearray(1)
    self._writecommand(TFT.COLMOD)               #Set color mode.
    data1[0] = 0x05                             #16 bit color.
    self._writedata(data1)
    time.sleep_us(10)

    self._writecommand(TFT.NORON)                #Normal display on.
    time.sleep_us(10)

    self._writecommand(TFT.RAMWR)
    time.sleep_us(500)

    self._writecommand(TFT.DISPON)
    self.cs(1)
    time.sleep_us(500)

  #@micropython.native
  def initg( self ) :
    '''Initialize a green tab version.'''
    self._reset()

    self._writecommand(TFT.SWRESET)              #Software reset.
    time.sleep_us(150)
    self._writecommand(TFT.SLPOUT)               #out of sleep mode.
    time.sleep_us(255)

    data3 = bytearray([0x01, 0x2C, 0x2D])       #fastest refresh, 6 lines front, 3 lines back.
    self._writecommand(TFT.FRMCTR1)              #Frame rate control.
    self._writedata(data3)

    self._writecommand(TFT.FRMCTR2)              #Frame rate control.
    self._writedata(data3)

    data6 = bytearray([0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d])
    self._writecommand(TFT.FRMCTR3)              #Frame rate control.
    self._writedata(data6)
    time.sleep_us(10)

    self._writecommand(TFT.INVCTR)               #Display inversion control
    self._writedata(bytearray([0x07]))
    self._writecommand(TFT.PWCTR1)               #Power control
    data3[0] = 0xA2
    data3[1] = 0x02
    data3[2] = 0x84
    self._writedata(data3)

    self._writecommand(TFT.PWCTR2)               #Power control
    self._writedata(bytearray([0xC5]))

    data2 = bytearray(2)
    self._writecommand(TFT.PWCTR3)               #Power control
    data2[0] = 0x0A   #Opamp current small
    data2[1] = 0x00   #Boost frequency
    self._writedata(data2)

    self._writecommand(TFT.PWCTR4)               #Power control
    data2[0] = 0x8A   #Opamp current small
    data2[1] = 0x2A   #Boost frequency
    self._writedata(data2)

    self._writecommand(TFT.PWCTR5)               #Power control
    data2[0] = 0x8A   #Opamp current small
    data2[1] = 0xEE   #Boost frequency
    self._writedata(data2)

    self._writecommand(TFT.VMCTR1)               #Power control
    self._writedata(bytearray([0x0E]))

    self._writecommand(TFT.INVOFF)

    self._setMADCTL()

    self._writecommand(TFT.COLMOD)
    self._writedata(bytearray([0x05]))

    self._writecommand(TFT.CASET)                #Column address set.
    self.windowLocData[0] = 0x00
    self.windowLocData[1] = 0x01                #Start at row/column 1.
    self.windowLocData[2] = 0x00
    self.windowLocData[3] = self._size[0] - 1
    self._writedata(self.windowLocData)

    self._writecommand(TFT.RASET)                #Row address set.
    self.windowLocData[3] = self._size[1] - 1
    self._writedata(self.windowLocData)

    dataGMCTRP = bytearray([0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29,
                            0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10])
    self._writecommand(TFT.GMCTRP1)
    self._writedata(dataGMCTRP)

    dataGMCTRN = bytearray([0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29, 0x2d, 0x2e,
                            0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10])
    self._writecommand(TFT.GMCTRN1)
    self._writedata(dataGMCTRN)

    self._writecommand(TFT.NORON)                #Normal display on.
    time.sleep_us(10)

    self._writecommand(TFT.DISPON)
    time.sleep_us(100)

    self.cs(1)

def maker(  ) :
  t = TFT(1, "X1", "X2")
  print("Initializing")
  t.initr()
  t.fill(0)
  return t

def makeb(  ) :
  t = TFT(1, "X1", "X2")
  print("Initializing")
  t.initb()
  t.fill(0)
  return t

def makeg(  ) :
  t = TFT(1, "X1", "X2")
  print("Initializing")
  t.initg()
  t.fill(0)
  return t

:mod:`LCD` --- functions related to the nRF52832 Bluefruit52
=========================================================

.. module:: LCD
    :synopsis: functions related to the nRF52832 Bluefruit52

The ``LCD`` module contains specific functions related to the nRF52832 Bluefruit52 modules.  Some functions are only available on one or the other of these
ports.


Functions
---------

.. function:: init(direction)

    Initialize the TFT displayer ST7789 register, and clear screen.

    If the *direction* parameter is provided, sets the Screen display direction to its
    value. If the function is called without parameters, default direction = 3, Horizontal screen shows.

    The the *direction* parameter value available:

        * ``direction = 0`` -- Vertical screen show to left direction,
        * ``direction = 1`` -- Vertical screen show to right direction,
        * ``direction = 2`` -- Vertical screen show to up direction,
        * ``direction = 3`` -- Vertical screen show to down direction.

.. function:: clear(LCD.BLACK)

    Clear all screen to color you want.

.. function:: drawBigPoint(x, y, color)

    Draw a big point, input parameters position coordinate :'x','y' and set color value 'color'.

.. function:: drawPoint(x, y, color)

    Draw a point, input parameters position coordinate :'x','y' and set color value 'color'.

.. function:: drawCircle(x, y, rad, color)

    Draw a Circle, input parameters :'x', 'y', 'radius', 'color'.

.. function:: drawLine(x1, y1, x2, y2, color)

    Draw a line, input parameters :*x1, y1, x2, y2, color*.
    Start position coordinate :*x1, y1*,
    End position coordinate :*x2,y2*,
    set line color value :*color*.

.. function:: drawRectangle(x1, y1, x2, y2, color)

    Draw a rectangle, 
    input parameters Rectangular diagonal coordinates:*x1, y1, x2, y2, color*.
    Start position coordinate :*x1, y1*,
    End position coordinate :*x2,y2*,
    set line color value :*color*.

.. function:: fill(x1, y1, x2, y2, color)

    Fill a rectangular area and set fill color.
    input parameters Rectangular diagonal coordinates:*x1, y1, x2, y2, color*.
    Start position coordinate :*x1, y1*,
    End position coordinate :*x2,y2*,
    set fill color value :*color*.

.. function:: fillScreen(color)

    Fill all Screen to one color.
    input parameters :*color*.

.. function:: showBLE(x, y)

    Show BLE logo image.set start coordinates.image resolution 40*40.
    input parameters start coordinates:*x, y*.

.. function:: showChar(x, y, char, mode, color)

    Show a Char, input parameters :*x, y, char, mode, color*.
    x,y : Starting point coordinates.
    char: ASCII Code Decimal number(e. 48: show number 0).
    mode: 1: Superposition background color, 0: Non-superimposed(default white color).
    color: character color.


.. function:: showFloat(x, y, float, len, color)

    show Float, input parameters : *x, y, float, len, color*.
    float: float number,just two decimal places.
    len : show number size len.

.. function:: showLogo()

    Show Bluetooth logo image.fill all screen.image resolution 135*240.

.. function:: showNum(x, y, num, len, color)

    show Float, input parameters : *x, y, num, len, color*.

.. function:: showPicture(start, length)

    Show penguin logo image.set start coordinates.image resolution 40*40.
    input parameters start coordinates:*x, y*.

.. function:: showString(x, y, 'Hello', color)

    show string, input parameters : *x, y, string, color*.
    e. LCD.showString(10, 10, 'Hello World!', LCD.RED)

.. function:: showChinese(start, length)
    
    **Note**: Not open now.

Constants
---------

.. data:: LCD.WHITE      (0xFFFF)
          LCD.BLACK      (0x0000)
          LCD.BLUE       (0x001F)
          LCD.BRED       (0XF81F)
          LCD.GRED       (0XFFE0)
          LCD.GBLUE      (0X07FF)
          LCD.RED        (0xF800)
          LCD.MAGENTA    (0xF81F)
          LCD.GREEN      (0x07E0)
          LCD.CYAN       (0x7FFF)
          LCD.YELLOW     (0xFFE0)
          LCD.BROWN      (0XBC40)
          LCD.BRRED      (0XFC07)
          LCD.GRAY       (0X8430)
          LCD.DARKBLUE   (0X01CF)
          LCD.LIGHTBLUE  (0X7D7C)
          LCD.GRAYBLUE   (0X5458)
          LCD.IGHTGREEN  (0X841F)
          LCD.LGRAY      (0XC618)
          LCD.LGRAYBLUE  (0XA651)
          LCD.LBBLUE     (0X2B12)

    Selects the color for display.
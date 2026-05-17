Making a UART - USB pass through
================================

It's as simple as::

    import pyb
    import select

    def pass_through(usb, uart):
        usb.setinterrupt(-1)
        while True:
            select.select([usb, uart], [], [])
            if usb.any():
                uart.write(usb.read(256))
            if uart.any():
                usb.write(uart.read(256))

    pass_through(pyb.USB_VCP(), pyb.UART(1, 9600, timeout=0))

from machine import UART
import time

uart_pins_args = {"tx": "P17_1", "rx": "P17_0"}
uart_basic_conf = {
    "baudrate": 9600,
    "bits": 8,
    "parity": None,
    "stop": 1,
    "timeout": 100,
    "rxbuf": 512,
}

uart = UART(**uart_pins_args, **uart_basic_conf)

# Check that reconfiguration of same UART object SCB is allowed by init()
uart_basic_conf["baudrate"] = 115200
uart.init(**uart_pins_args, **uart_basic_conf)


def uart_tests():
    # 2. Basic tests
    ##############################################

    # sendbreak()
    uart.sendbreak()
    time.sleep_ms(100)
    break_received = uart.read()
    print("Break Sent is received by Rx: ", break_received == b"\x00")

    # timeout expired()
    rx_data = uart.read()
    print("Timeout Expired with no data received: ", rx_data == None)

    # write_char()
    # read_char()
    uart.writechar(33)
    time.sleep_ms(100)
    rx_char = uart.readchar()
    print("Tx char is received by Rx char: ", rx_char == 33)

    # read()
    tx_data = b"abcdefg"
    tx_written = uart.write(tx_data)
    print("Tx written bytes by write(): ", tx_written == len(tx_data))
    time.sleep_ms(100)
    rx_data = uart.read()
    print("Tx is received by Rx(read()): ", rx_data == tx_data)

    # readline(nbytes)
    tx_data = "abcd\ne"
    uart.write(tx_data)
    time.sleep_ms(100)
    rx_data = uart.readline()
    print("Tx is received by Rx(readline()): ", rx_data == b"abcd\n")
    if rx_data != b"abcd\n":
        print("Received data:", rx_data)
        print("Expected data:", b"abcd\n")
    uart.read()  # read all data available to clear buffer

    # read(n bytes)
    tx_data = "abcdefghijklmn"
    uart.write(tx_data)
    time.sleep_ms(8)
    rx_data = uart.read(7)
    print("Tx is received by Rx(read(nbytes)):", rx_data == b"abcdefg")

    # tx_done()
    tx_data = "abcdefg"
    uart.write(tx_data)
    print("Tx Ongoing: ", uart.txdone() == False)
    time.sleep_ms(100)
    print("Tx Done: ", uart.txdone() == True)
    uart.read()  # read all data available to clear buffer

    # write(buf)
    # readinto(buf)
    uart_rx_buf = bytearray(8)
    tx_data = b"\x01\x44\x17\x88\x98\x11\x34\xff"
    uart.write(tx_data)
    time.sleep_ms(100)
    uart.readinto(uart_rx_buf)
    print("Tx is received by Rx(readinto(buf)): ", uart_rx_buf == tx_data)

    # read() return None when timeout expired.
    rx_data = uart.read(1)
    print("Rx buffer is empty, read(nbytes) returns None: ", rx_data == None)

    # write()/read() large data
    uart_rx_buf = bytearray(512)
    tx_data = bytes([x % 256 for x in range(512)])
    tx_written = uart.write(tx_data)
    print("Tx written bytes by write(): ", tx_written)
    # An added sleep here makes the last bytes to be lost.
    read_num = uart.readinto(uart_rx_buf)
    print("Tx is received by Rx(readinto(buf)) for large data: ", uart_rx_buf == tx_data)
    if uart_rx_buf != tx_data:
        print("Received data:", uart_rx_buf)
        print("Num of bytes read:", read_num)
        print("Expected data:", tx_data)

    # Check flush() is working.
    uart.write(tx_data)
    # Enable the timer and change the tx_data length if you
    # want to see the different flush time.
    # start = time.ticks_ms()
    print("Tx Ongoing: ", uart.txdone() == False)
    uart.flush()
    # print("Flush time (ms): ", time.ticks_diff(time.ticks_ms(), start))
    print("Tx Done after flush: ", uart.txdone() == True)


def uart_irq():
    global handler_irq_flag
    handler_irq_flag = False

    def uart_irq_handler(arg):
        global handler_irq_flag
        handler_irq_flag = True
        print(f"IRQ {event} handler called")

    def wait_for_irq_handler():
        global handler_irq_flag
        while not handler_irq_flag:
            time.sleep_ms(10)
        handler_irq_flag = False

    # BREAK Received check
    event = "BREAK"
    irq = uart.irq(handler=uart_irq_handler, trigger=(UART.IRQ_BREAK))
    uart.sendbreak()
    wait_for_irq_handler()
    print("IRQ BREAK detected: ", irq.flags() & UART.IRQ_BREAK == UART.IRQ_BREAK)

    # TXIDLE check
    event = "TXIDLE"
    uart.irq(handler=uart_irq_handler, trigger=(UART.IRQ_TXIDLE))
    uart.write("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
    wait_for_irq_handler()
    print("IRQ TXIDLE detected: ", irq.flags() & UART.IRQ_TXIDLE == UART.IRQ_TXIDLE)

    # RXIDLE check
    event = "RXIDLE"
    uart.irq(handler=uart_irq_handler, trigger=(UART.IRQ_RXIDLE))
    uart.write("1")
    wait_for_irq_handler()
    print("IRQ RXIDLE detected: ", irq.flags() & UART.IRQ_RXIDLE == UART.IRQ_RXIDLE)


uart_tests()
uart_irq()


uart.deinit()

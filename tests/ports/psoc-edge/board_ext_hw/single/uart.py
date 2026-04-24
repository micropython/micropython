from machine import UART
import time

uart_pins_args = {"tx": "P17_1", "rx": "P17_0"}
uart_basic_conf = {
    "baudrate": 115200,
    "bits": 8,
    "parity": None,
    "stop": 1,
    "timeout": 100,
    "rxbuf": 512,
}

uart = UART(**uart_pins_args, **uart_basic_conf)


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


uart_tests()


uart.deinit()

"""
UART/Serial Communication Examples for MicroPython Unix Port

Demonstrates using machine.UART for serial communication on Linux/Pi
"""

from machine import UART
import time


# ====================
# Example 1: Basic UART
# ====================
def example_basic():
    """Basic UART usage - open, read, write"""
    print("Example 1: Basic UART")

    # Open serial port
    uart = UART(0, baudrate=115200)  # /dev/ttyS0
    # or: uart = UART('/dev/ttyUSB0', baudrate=9600)

    # Write data
    uart.write(b'Hello World\r\n')

    # Read data
    data = uart.read(100)
    if data:
        print(f"Received: {data}")

    uart.deinit()


# ====================
# Example 2: GPS Module
# ====================
def example_gps():
    """Read GPS data from serial GPS module"""
    print("\nExample 2: GPS Module")

    uart = UART('/dev/ttyUSB0', baudrate=9600, timeout=2000)

    print("Reading GPS data...")
    for i in range(10):
        data = uart.read(200)
        if data and b'$GP' in data:
            # Parse NMEA sentence
            lines = data.split(b'\r\n')
            for line in lines:
                if line.startswith(b'$GPGGA'):
                    print(f"GPS: {line.decode()}")
        time.sleep(1)

    uart.deinit()


# ====================
# Example 3: Arduino Communication
# ====================
def example_arduino():
    """Communicate with Arduino via serial"""
    print("\nExample 3: Arduino Communication")

    uart = UART('/dev/ttyACM0', baudrate=9600, timeout=1000)

    # Send command to Arduino
    uart.write(b'LED ON\n')
    time.sleep(0.1)

    # Read response
    response = uart.read(100)
    if response:
        print(f"Arduino says: {response.decode()}")

    # Request sensor data
    uart.write(b'READ TEMP\n')
    time.sleep(0.1)

    temp_data = uart.read(50)
    if temp_data:
        print(f"Temperature: {temp_data.decode()}")

    uart.deinit()


# ====================
# Example 4: Flow Control
# ====================
def example_flow_control():
    """UART with hardware flow control"""
    print("\nExample 4: Hardware Flow Control")

    uart = UART(
        '/dev/ttyUSB0',
        baudrate=115200,
        bits=8,
        parity=UART.PARITY_NONE,
        stop=1,
        flow=UART.FLOW_RTS_CTS  # Hardware flow control
    )

    # High-speed data transfer with flow control
    large_data = b'X' * 10000
    uart.write(large_data)
    print(f"Sent {len(large_data)} bytes")

    uart.deinit()


# ====================
# Example 5: RS-485 Communication
# ====================
def example_rs485():
    """RS-485 communication (using USB-RS485 adapter)"""
    print("\nExample 5: RS-485")

    uart = UART('/dev/ttyUSB0', baudrate=9600)

    # Send Modbus RTU query (example)
    # Device address 0x01, function 0x03, read registers
    query = bytes([0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0xCD])

    uart.write(query)
    time.sleep(0.1)

    response = uart.read(100)
    if response:
        print(f"Modbus response: {response.hex()}")

    uart.deinit()


# ====================
# Example 6: Multiple UARTs
# ====================
def example_multiple():
    """Using multiple UART ports simultaneously"""
    print("\nExample 6: Multiple UARTs")

    uart1 = UART(0, baudrate=9600)    # /dev/ttyS0
    uart2 = UART(1, baudrate=115200)  # /dev/ttyS1

    # Bridge two serial ports
    for i in range(10):
        # Read from uart1, write to uart2
        data1 = uart1.read(100)
        if data1:
            uart2.write(data1)

        # Read from uart2, write to uart1
        data2 = uart2.read(100)
        if data2:
            uart1.write(data2)

        time.sleep(0.1)

    uart1.deinit()
    uart2.deinit()


# ====================
# Example 7: Stream Interface
# ====================
def example_stream():
    """Using UART as a stream"""
    print("\nExample 7: Stream Interface")

    uart = UART('/dev/ttyUSB0', baudrate=9600)

    # UART supports stream protocol
    import select

    # Poll for readable data
    poll = select.poll()
    poll.register(uart, select.POLLIN)

    while True:
        events = poll.poll(1000)  # 1 second timeout
        if events:
            data = uart.read(100)
            if data:
                print(f"Data: {data}")
                if b'quit' in data:
                    break

    uart.deinit()


# ====================
# Example 8: Debugging Output
# ====================
def example_debug_logger():
    """Use UART for debug logging"""
    print("\nExample 8: Debug Logger")

    debug_uart = UART('/dev/ttyUSB0', baudrate=115200)

    def debug_log(message):
        timestamp = time.time()
        log_msg = f"[{timestamp}] {message}\n".encode()
        debug_uart.write(log_msg)

    debug_log("System started")
    debug_log("Initializing sensors...")
    time.sleep(1)
    debug_log("Sensors ready")
    debug_log("Running main loop")

    debug_uart.deinit()


# ====================
# Run all examples
# ====================
if __name__ == '__main__':
    print("="*50)
    print("MicroPython UART Examples")
    print("="*50)

    # Note: These examples require actual serial devices
    # Uncomment the one you want to test

    # example_basic()
    # example_gps()
    # example_arduino()
    # example_flow_control()
    # example_rs485()
    # example_multiple()
    # example_stream()
    # example_debug_logger()

    print("\nUncomment examples to run them")
    print("Make sure serial devices are connected!")

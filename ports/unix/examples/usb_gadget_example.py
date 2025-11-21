"""
USB Gadget Mode Examples for MicroPython Unix Port

Demonstrates using machine.USB to turn Raspberry Pi into a USB device
"""

from machine import USB
import time


# ====================
# Example 1: Basic USB CDC
# ====================
def example_basic_cdc():
    """Turn Pi into a USB serial device"""
    print("Example 1: Basic USB CDC/ACM")

    usb = USB

    # Configure USB device
    usb.config(
        vid=0x1234,
        pid=0x5678,
        manufacturer="MicroPython",
        product="USB Serial Device",
        serial="123456789"
    )

    # Initialize USB CDC/ACM
    usb.init()

    print("USB device initialized!")
    print("Connect Pi USB port to computer")
    print("It will appear as /dev/ttyACM0 (Linux) or COM port (Windows)")

    # Keep running
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        usb.deinit()
        print("USB disabled")


# ====================
# Example 2: Custom USB Descriptors
# ====================
def example_custom_descriptors():
    """Configure custom USB device descriptors"""
    print("\nExample 2: Custom USB Descriptors")

    usb = USB

    # Configure with custom values
    usb.config(
        vid=0x2E8A,  # Raspberry Pi Foundation VID
        pid=0x000A,  # Custom PID
        manufacturer="My Company",
        product="My USB Device",
        serial="SN-2025-001"
    )

    usb.init()

    print("USB device with custom descriptors active")
    print(f"Configuration: {usb.config()}")

    time.sleep(5)
    usb.deinit()


# ====================
# Example 3: USB Serial Bridge
# ====================
def example_usb_serial_bridge():
    """Bridge USB and hardware UART"""
    print("\nExample 3: USB-Serial Bridge")

    from machine import UART

    usb = USB
    usb.config(product="USB-UART Bridge")
    usb.init()

    # Open hardware UART
    uart = UART(0, baudrate=115200)

    print("USB-UART bridge active")
    print("Data from USB goes to UART, data from UART goes to USB")

    # Note: Actual data bridging would require reading/writing
    # to both the USB device file (/dev/g_serial or similar)
    # and the UART. This is a conceptual example.

    time.sleep(10)

    uart.deinit()
    usb.deinit()


# ====================
# Example 4: USB Debug Console
# ====================
def example_debug_console():
    """Use USB as a debug output console"""
    print("\nExample 4: USB Debug Console")

    usb = USB
    usb.config(product="MicroPython Debug Console")
    usb.init()

    print("USB debug console active")
    print("Open serial terminal on host: screen /dev/ttyACM0 115200")

    # Simulate debug output
    # In real usage, you'd write to the USB CDC device file
    for i in range(10):
        print(f"Debug message {i}")
        time.sleep(1)

    usb.deinit()


# ====================
# Example 5: USB Device Status
# ====================
def example_status():
    """Check USB gadget status"""
    print("\nExample 5: USB Status")

    usb = USB

    # Check if active
    if usb.active():
        print("USB gadget is active")
    else:
        print("USB gadget is inactive")

    # Get configuration
    config = usb.config()
    print(f"VID: 0x{config[0]:04X}")
    print(f"PID: 0x{config[1]:04X}")
    print(f"Serial: {config[2]}")
    print(f"Manufacturer: {config[3]}")
    print(f"Product: {config[4]}")
    print(f"Active: {config[5]}")


# ====================
# Example 6: USB Gadget On/Off
# ====================
def example_enable_disable():
    """Enable and disable USB gadget"""
    print("\nExample 6: Enable/Disable USB")

    usb = USB

    # Disable
    usb.active(False)
    print("USB disabled")
    time.sleep(2)

    # Enable
    usb.config(product="Test Device")
    usb.active(True)
    print("USB enabled")
    time.sleep(5)

    # Disable again
    usb.active(False)
    print("USB disabled")


# ====================
# Example 7: Multiple USB Functions (Future)
# ====================
def example_multi_function():
    """
    Future: USB composite device (CDC + Mass Storage + HID)

    This is a conceptual example. Full implementation would require
    additional code to support multiple USB functions simultaneously.
    """
    print("\nExample 7: Multi-Function USB (Future)")

    usb = USB

    # Future API might look like:
    # usb.add_function('cdc')
    # usb.add_function('mass_storage', file='/dev/sda1')
    # usb.add_function('hid_keyboard')
    # usb.init()

    print("Multi-function USB not yet implemented")
    print("Currently supports: CDC/ACM serial only")


# ====================
# Example 8: USB Gadget with Systemd
# ====================
def example_systemd_service():
    """Example systemd service file for automatic USB gadget setup"""

    service_file = """
# /etc/systemd/system/micropython-usb.service
[Unit]
Description=MicroPython USB Gadget
After=network.target

[Service]
Type=simple
ExecStart=/usr/local/bin/micropython -c "from machine import USB; usb=USB; usb.config(product='MicroPython Device'); usb.init(); import time; time.sleep(99999)"
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
"""

    print("\nExample 8: Systemd Service")
    print("="*50)
    print(service_file)
    print("="*50)
    print("\nTo use:")
    print("1. Save above to /etc/systemd/system/micropython-usb.service")
    print("2. sudo systemctl daemon-reload")
    print("3. sudo systemctl enable micropython-usb")
    print("4. sudo systemctl start micropython-usb")


# ====================
# Run examples
# ====================
if __name__ == '__main__':
    print("="*50)
    print("MicroPython USB Gadget Examples")
    print("="*50)
    print("\nNote: Requires root/sudo to access /sys/kernel/config")
    print("Run with: sudo micropython usb_gadget_example.py")
    print()

    # example_basic_cdc()
    # example_custom_descriptors()
    # example_usb_serial_bridge()
    # example_debug_console()
    example_status()
    # example_enable_disable()
    # example_multi_function()
    example_systemd_service()

    print("\nUncomment examples to run them!")

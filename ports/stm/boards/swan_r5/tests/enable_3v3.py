# Background: I have a Swan R5 board running circuit python
# And I import the "board" module
import board
import digitalio
import supervisor
import time

# Scenario: Enable 3V3 pin definition
# Then the symbol "board.ENABLE_3V3" is defined
assert board.ENABLE_3V3 is not None

# Scenario: Discharge 3V3 definition
# Then the symbol "board.DISCHARGE_3V3" is defined
assert board.DISCHARGE_3V3 is not None
# And the symbol "board.DISABLE_DISCHARGING" is defined to be "True"
assert board.DISABLE_DISCHARGING is not None and board.DISABLE_DISCHARGING == True
# And the symbol "board.ENABLE_DISCHARGING" is defined to be "False"
assert board.ENABLE_DISCHARGING is not None and board.ENABLE_DISCHARGING == False

# Scenario: Toggle ENBLE_3V3
# Given I have a LED connected between the 3V3 and GND pins
# And ENABLE_3V3 is configured for output
_3v3 = digitalio.DigitalInOut(board.ENABLE_3V3)
_3v3.direction = digitalio.Direction.OUTPUT
# When I run code to toggle the pin at 0.5Hz
# Then I see the LED switch on and off at 0.5Hz
print("Toggling 3V3. Press a key to stop.")

while not supervisor.runtime.serial_bytes_available:
    _3v3.value = True
    time.sleep(1.0)
    _3v3.value = False
    time.sleep(1.0)

print("Toggling stopped.")

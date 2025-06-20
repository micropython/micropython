from machine import SPI,Pin
import time

# Define SPI pins and settings
spi = SPI(1, 
          baudrate=1000000,  # Adjust baudrate as needed
          polarity=0,        # Clock polarity (CPOL)
          phase=0,           # Clock phase (CPHA)
          bits=8,            # Number of bits per word
          firstbit=SPI.MSB)  # Most Significant Bit first

# Define chip select (CS) pin
cs = Pin('SPI1_CS', Pin.OUT)  # Adjust pin name according to your setup
cs.value(1)

def W25Q_Reset():
    tData = bytearray([0x66, 0x99])  # Prepare the reset command
    cs.value(0)  # Activate chip select
    spi.write(tData)  # Send the reset command
    cs.value(1)  # Deactivate chip select
    time.sleep(0.1)  # Wait for the reset operation to complete
    

def W25Q_ReadID(): # Read JEDEC ID
    # Send command to read ID (this is an example; check your chip's datasheet)
    tData = bytearray([0x9F])
    rData = bytearray(3) 
    cs.value(0)
    spi.write(tData)
    rData = spi.read(3)
    cs.value(1)
    return (rData[0] << 16) | (rData[1] << 8) | rData[2]

# Read and print the Flash ID
W25Q_Reset()
rData = W25Q_ReadID()
print("Flash ID:", hex(rData)) #It should return 0xef4017 | EFh = Winbond Serial Flash, 4017= W25Q64JV-IQ/JQ

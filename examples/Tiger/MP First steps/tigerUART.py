from machine import UART


print("START PROGRAM")

data = bytearray() #buffer to receive the raw data

flag = False #flag to detect a character interrupt

def callback(device): #callbacl for the interruption
    global flag
    flag = True

uart = UART("UART") #define the UART
uart.init(9600, bits=8, parity=None, stop=1) #initializes the UART
uart.irq(trigger=UART.IRQ_RXIDLE,handler=callback,hard=True)  #sets the interruption for everytime the RX becomes idle(?) which was the only available.... hard is using the hardware interruption


byte = '' #byte to be read

while byte != b'\x1b': #while ESC is not pressed
    #print(len(data))
    if flag is not True: #if an interruptions has not been detected
        continue
    flag = False #returns the flag to true
    byte = uart.readline() #reads a byte
    if byte == b'\r': #if the byte read is the end line
        print(str(data.decode('utf-8'))) #print the line read until now
        data = bytearray() #clean data
    else:
        if byte is not None:
            data.extend(byte)#appends the byte in the data buffer

print("END PROGRAM")
import pyb
from pyb import udelay
from machine import Pin

class LCD1: #LCD1 class

    CLR = 0x01 #deletes the LCD screen
    HOME = 0x02 #sets the cursor in the top left corner
    FS = 0x05 #Cursor 1 position to the right
    BS = 0x08 #Cursor 1 position to the left
    LF = DO = 0x0A #Cursor 1 position down
    UP = 0x0B #Cursor 1 position up
    FF = 0x0C #Form Feed
    CR = 0x0D #Carriage Return

    COMMANDS = { #dictionary to relate the commands to the names
        CLR  : "clear",
        HOME : "home",
        FS   : "cursor right",
        BS   : "cursor left",
        LF   : "cursor down",
        DO   : "cursor down",
        UP   : "cursor up",
        FF   : "form feed",
        CR   : "carriage return"
    }

    __LCD_TABLE = [ #init table of lcd, from lcd01 driver
    [0,  0,     0, [0,  0,  0,  0]], #0
    [1,  8,  0x38, [0, 20, 40, 60]], #1
    [1, 12,  0x38, [0, 20, 40, 60]], #2
    [1, 16,  0x38, [0, 20, 40, 60]], #3
    [1, 20,  0x38, [0, 20, 40, 60]], #4
    [1, 40,  0x38, [0, 20, 40, 60]], #5
    [2,  8,  0x38, [0, 20, 40, 60]], #6
    [2, 12,  0x38, [0, 20, 40, 60]], #7
    [2, 16,  0x38, [0, 20, 40, 60]], #8
    [2, 20,  0x38, [0, 20, 40, 60]], #9
    [2, 40,  0x38, [0, 64,  0,  0]], #10
    [4, 20,  0x38, [0, 20, 40, 60]], #11
    [2,  8,  0x38, [0, 64,  0,  0]], #12
    [2, 12,  0x38, [0, 64,  0,  0]], #13
    [2, 16,  0x38, [0, 64,  0,  0]], #14
    [2, 20,  0x38, [0, 64,  0,  0]], #15
    [2, 40,  0x38, [0, 64,  0,  0]], #16
    [4, 20,  0x38, [0, 64, 20, 84]], #17
    [2, 24,  0x38, [0, 64,  0,  0]], #18
    [4, 16,  0x38, [0, 64, 16, 80]], #19
    [4, 24,  0x39, [0, 32, 64, 96]], #20
    [2,  8,  0x38, [0, 64,  0,  0]], #21
    [2, 24,  0x30, [0, 64,  0,  0]], #22
    [2, 32,  0x31, [0, 64,  0,  0]], #23
    [2, 40,  0x32, [0, 64,  0,  0]], #24
    [4,  8,  0x33, [0, 64,  0,  0]], #25
    [4, 16,  0x38, [0, 32, 64, 96]], #26
    [4, 20,  0x38, [0, 32, 64, 96]], #27
    [4, 32,  0x38, [0, 32, 64, 96]], #28
    [4, 32,  0x30, [0, 32, 64, 96]], #29
    [4, 32,  0x31, [0, 32, 64, 96]], #30
    [4, 32,  0x32, [0, 32, 64, 96]], #31
    [4, 32,  0x33, [0, 32, 64, 96]], #32
    [4, 32,  0x38, [0, 32, 64, 96]], #33
    [4, 32,  0x39, [0, 32, 64, 96]], #34
    [4, 32,  0x3A, [0, 32, 64, 96]], #35
    [4, 32,  0x3B, [0, 32, 64, 96]], #36
    [4, 20,  0x38, [0, 32, 64, 96]], #37
    [3, 16,  0x39, [0, 16, 32,  0]], #38
    [4, 20,     0, [0, 32, 64, 96]]  #39
    ]

    __CHARSET0 = [   10,0,14,1,15,17,15,0,10,0,14,17,17,17,14,0,10,0,17,17,17,19,13,0,17,14,17,17,31,17,17,0,
                    17,14,17,17,17,17,14,0,17,0,17,17,17,17,14,0,23,19,17,16,17,19,23,31,29,25,17,1,17,25,29,31,
                    0,4,6,31,6,4,0,0,0,4,12,31,12,4,0,0,0,4,14,31,4,4,0,0,0,4,4,31,14,4,0,0,
                    8,12,14,15,14,12,8,0,2,6,14,30,14,6,2,0,0,4,14,31,14,4,0,0,0,0,14,14,14,0,0,0,
                    14,14,15,15,15,0,0,0,14,14,30,30,30,0,0,0,0,0,15,15,15,14,14,14,0,0,30,30,30,14,14,14,
                    14,14,14,14,14,14,14,14,0,0,31,31,31,0,0,0,14,14,31,31,31,14,14,14,0,0,0,0,0,0,0,0,
                    4,4,4,7,0,0,0,0,4,4,4,28,0,0,0,0,0,0,0,7,4,4,4,4,0,0,0,28,4,4,4,4,
                    4,4,4,4,4,4,4,4,0,0,0,31,0,0,0,0,4,4,4,31,4,4,4,4,0,0,0,0,0,0,0,0,
                    4,4,4,4,4,4,4,0,1,2,2,4,8,8,16,0,0,1,2,4,8,16,0,0,0,0,3,4,24,0,0,0,
                    0,0,0,31,0,0,0,0,0,0,24,4,3,0,0,0,0,16,8,4,2,1,0,0,16,8,8,4,2,2,1,0,
                    31,27,19,27,27,17,31,0,31,19,29,27,23,17,31,0,31,19,29,27,29,19,31,0,31,23,23,21,16,29,31,0,
                    31,17,23,19,29,19,31,0,31,27,23,19,21,25,31,0,31,17,29,27,27,27,31,0,31,27,21,27,21,27,31,0,
                    31,27,21,25,29,19,31,0,31,27,21,21,21,27,31,0,31,27,21,21,17,21,31,0,31,19,21,19,21,19,31,0,
                    31,25,23,23,23,25,31,0,31,19,21,21,21,19,31,0,31,17,23,19,23,17,31,0,31,17,23,19,23,23,31,0,
                    31,27,25,0,25,27,31,31,31,27,19,0,19,27,31,31,31,27,17,0,27,27,31,31,31,27,27,0,17,27,31,31,
                    23,19,17,16,17,19,23,31,29,25,17,1,17,25,29,31,31,27,17,0,17,27,31,31,31,31,17,17,17,31,31,31,
                    0,0,0,0,0,0,0,31,0,0,0,0,0,0,31,31,0,0,0,0,0,31,31,31,0,0,0,0,31,31,31,31,
                    0,0,0,31,31,31,31,31,0,0,31,31,31,31,31,31,0,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
                    0,0,0,0,0,0,0,14,0,0,0,0,0,0,14,14,0,0,0,0,0,14,14,14,0,0,0,0,14,14,14,14,
                    0,0,0,14,14,14,14,14,0,0,14,14,14,14,14,14,0,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
                    4,4,4,4,4,4,4,31,4,4,4,4,4,4,31,31,4,4,4,4,4,31,31,31,4,4,4,4,31,31,31,31,
                    4,4,4,31,31,31,31,31,4,4,31,31,31,31,31,31,4,31,31,31,31,31,31,31,4,4,4,4,4,4,4,4,
                    31,0,0,0,0,0,31,0,31,16,16,16,16,16,31,0,31,24,24,24,24,24,31,0,31,28,28,28,28,28,31,0,
                    31,30,30,30,30,30,31,0,31,31,31,31,31,31,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    16,16,16,16,16,16,16,0,24,24,24,24,24,24,24,0,28,28,28,28,28,28,28,0,30,30,30,30,30,30,30,0,
                    31,31,31,31,31,31,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    31,31,31,31,0,0,0,0,0,0,0,0,31,31,31,31,28,28,28,28,28,28,28,28,7,7,7,7,7,7,7,7,
                    28,28,28,28,0,0,0,0,0,0,0,0,28,28,28,28,7,7,7,7,0,0,0,0,0,0,0,0,7,7,7,7,
                    28,30,31,15,7,3,1,0,0,0,0,0,16,24,28,30,15,7,3,1,0,0,0,0,0,16,24,28,30,31,15,7,
                    7,15,31,30,28,24,16,0,0,0,0,0,1,3,7,15,30,28,24,16,0,0,0,0,0,1,3,7,15,31,30,28,
                    0,0,0,4,0,0,0,0,0,0,14,14,14,0,0,0,0,31,31,31,31,31,0,0,31,31,31,31,31,31,31,0,
                    31,31,31,27,31,31,31,0,31,31,17,17,17,31,31,0,31,17,17,17,17,17,31,0,0,17,10,4,10,17,0,0
    ] #charset array from lcd1

    init_commands_lcd_37 = [0x34, 0x09, 0x30, 0x0F, 0x01, 0x06] #commands for lcd type 37
    init_commands_lcd_38 = [0x39, 0x1D, 0x50, 0x6C, 0x7C, 0x0F, 0x01, 0x06] #commands for lcd type 38
    init_commands_lcd_39 = [0x3C, 0x09, 0x38, 0x02, 0x0f, 0x01, 0x3C, 0x06, 0x38] #commands for lcd type 39

    PINS = ['RS', 'E', 'D0', 'D1', 'D2', 'D3', 'D4', 'D5', 'D6', 'D7'] #list of pins

    __ESC_MODE_NEED_CMD = 0 #define for need command in esc control char
    __ESC_MODE_NEED_PARAM = 1#define for need parameter in esc control char
    __ESC_MODE_NEED_EOS = 2 #define for need end of  in esc control char

    def __init_lcd(self, lcd_type): #function to init lcd
        self.__lcd = self.__LCD_TABLE[lcd_type] #reads from the table according to the type
        self.__line_offset = self.__lcd[3] #line offset

        self.__lcd_h = self.__lcd[0] #heigth of the lcd
        self.__lcd_w = self.__lcd[1] #width of the lcd

        init_commands_default = [self.__lcd[2], self.__lcd[2], 0x0F, 0x06, 0x01] #init commmands for most of the lcd types
        init_seq = init_commands_default #init sequence of most lcd types

        #specific lcd types with different init sequences
        if lcd_type == 37:
            init_seq = self.init_commands_lcd_37
        elif lcd_type == 38:
            init_seq = self.init_commands_lcd_38
        elif lcd_type == 39:
            init_seq = self.init_commands_lcd_39


        for i in range(len(init_seq)):#loop to send the init sequence
            if self.__lcd_4_bit: #if lcd is 4 bits
                init_seq[i] &= 0xEF  # in C -> ~0x10, but it does not work in python since it interpretates it as a signed number
            self.__send_byte(init_seq[i], True) #send command bytes


        #For some reason LCD 1 makes it twice
        for i in range(len(init_seq)):
            if self.__lcd_4_bit:
                init_seq[i] &= 0xEF  # in C -> ~0x10, but it does not work in python since it interpretates it as a signed number
            self.__send_byte(init_seq[i], True)

    def __init__(self,lcd_display_type = 17, lcd_4_bit=False): #LCD constructor

        self.__lcd_4_bit = lcd_4_bit #if lcd is 4 bits

        self.__pins = {} #start list of pins

        self.__lcd = self.__LCD_TABLE[0] #inits the lcd with a 0 value
        self.__line_offset = self.__lcd[3] #line offset

        self.__lcd_h = 0 #inits lcd height with 0
        self.__lcd_w = 0 #inits lcd width with 0

        self.__lcd_x = 0 #inits x position with 0
        self.__lcd_y = 0 #inits x position with 0

        self.__in_esc = False #in escape sequence is False now
        self.__esc_mode = self.__ESC_MODE_NEED_CMD #first mode of every escape is needs command
        self.__esc_cmd = 0 #no escape command right now

        self.__param_buffer = [0] * 128 #param buffer of the esape commands
        self.__param_buffer_len = 0 #length of the buffer right now is 0
        self.__param_bytes_needed = 0 #bytes needed by the command

        self.__ram1_lcd_sonder = True #if the special character table should be used

        self.__special_charset = 0xFF #the chaset to be used
        self.__charset_table = [0] * 1024 #table of the charsets

        for pin in self.PINS: #run thorugh all the pins
            name = 'LCD1_' + pin #gets the name
            self.__pins[name] = Pin(name, Pin.OUT) #salves every pin according to its name

        self.__init_lcd(lcd_display_type) #init lcd

        self.__esc_init() #init the charset table


    def __send_byte(self, byte , is_cmd):
        if is_cmd: #if it is a command
            self.__pins['LCD1_RS'].value(0) #RS low
        else:
            self.__pins['LCD1_RS'].value(1) #RS high

        if not self.__lcd_4_bit: #if it is a 8 bit lcd
            for i in range(8): #runs through every bit
                pin = 'LCD1_D' + str(i) #runs through every Data pin
                self.__pins[pin].value( (byte >> i) & 1 ) # sets the pin according to the message
            self.__pulse_enable() # pulses the Enable pin

        else: #it is a 4 bit display
            upper = byte & 0xF0 #upper byte
            lower = byte & 0x0F #lower byte

            byte = (lower << 4) | (upper >> 4) #upper byte is sent first

            for i in range(2): #runs 2 for of 4 instead of 1 of 8
                for j in range(4): #sends 4 bits each time
                    pin = 'LCD1_D' + str(4+j) #the pins to be used
                    self.__pins[pin].value(byte & 1) #only the LSB
                    byte >>= 1 #shifts the byte to get the next bit
                self.__pulse_enable() # pulses the Enable pin
        udelay(2000)  # After send a byte, it needs to have a delay, only worked with 2ms, maybe because TigerPython is way faster than normal Tiger


    def __print__(self, message):#tries to emulate lcd print

        is_instance_str = isinstance(message, str)

        if is_instance_str: #if it is a stance of a string, gets the length of the string
            m_length = len(message)
        else: #it is an int
            m_length = 1 #len of an int is 1

        i = 0 #counter

        ch = [0] #char to be used in the commands

        while i < m_length: #while message is not at the end

            if is_instance_str: #check if message is an instance of string
                ch[0] = ord(message[i]) #char to be sent is it's ascii number
            else:
                ch[0] = message #message is int already

            if ch[0] == ord('<'): # checks if command is a hex tiger style
                pos = message.find('>', i, m_length) #if < has its pair, it could be
                nibble_size = pos - i #size of the content inside <>
                if pos != -1 and nibble_size > 0: #if there is something inside
                    cmd = message[i + 1:pos] #command is what is inside
                    len_cmd = len(cmd) #size of the command
                    if len_cmd <= 3: #if it is at least a hexadecimal number
                        base = 10 #base to convert it to a number
                        if cmd[len_cmd - 1] == 'h' or cmd[len_cmd - 1] == 'H': #if it is a hexadecimal Tiger style
                            base = 16 #base to be converted
                            len_cmd -= 1 #cmd size without the H
                        try:
                            ch[0] = int(cmd[0:len_cmd], base) #try to convert the command to int
                            i += nibble_size #advances the counter according to the size of the number: ex <1Bh> occupies 5 chars
                        except ValueError:
                            pass
            if ch[0] == 27 or self.__in_esc:#if it is an escape char
                if self.__process_esc_char(ch):#process the control char
                    self.__send_byte(ch[0], True) #if it return True send ch as a command byte

            elif ch[0] < ord(' '): #if it is a control character
                if self.__process_control_char(ch):#process the control char
                    self.__send_byte(ch[0], True) #sends the outcome as a command byte

            else:#it is a normal char

                if ch[0] >= 0x80 and self.__ram1_lcd_sonder: #if it is a special character
                    self.__set_special_charset(ch[0] >> 3) #set the special character
                    ch[0] &= 0x07 #filters it from the ASCII value to a value in our table

                self.__send_byte(ch[0], False)#send it is a character
                self.__inc_pos()  # increments the position HAD TO PUT HERE INSTEAD OF THE START, idk why, but some examples only work like this

            i += 1 #goes to the next byte


    def print(self, args): #print function trying to emulate print from tiger
        if isinstance(args, list): #if the argument is a list
            for arg in args: #runs throught every item
                self.__print__(arg)#calls the auxiliary function for every item
        else:#it is a string
            self.__print__(args)#calls the auxiliary function for the string command


    def __pulse_enable(self): #pulses the enable pin
        udelay(1)#1 micro delay
        self.__pins['LCD1_E'].value(1) # sets pin high
        udelay(1)#1 micro delay
        self.__pins['LCD1_E'].value(0)# sets pin low
        udelay(1)#1 micro delay

    def __calc_pos(self): #calc the position of the cursor
        return 0x80 + self.__lcd_x + self.__line_offset[self.__lcd_y] #direct from tiger lcd

    def __inc_pos(self): #incement the position

        self.__lcd_x += 1 #sets cursor forward

        if self.__lcd_x >= self.__lcd_w:#if it is bigger than the width
            self.__lcd_x = 0 #sets the cursor in the beginning of the line
            self.__lcd_y += 1 #goes to the next line
            if self.__lcd_y >= self.__lcd_h: # if it is the end of the line
                self.__lcd_y = 0 #sets to the beginning again
            self.__send_byte(self.__calc_pos(), True) #sends the new position as a command


    def __lcd_disable_special_charset(self): #deactivates the char set
        if self.__param_buffer[0] == 0: #if the command is 0 than it nees do be turned off
            self.__ram1_lcd_sonder = False
        else: #otherwise it is on
            self.__ram1_lcd_sonder = True

    def __set_special_charset(self, charset): #set a special chaset

        charset &= 0xF #limits charsets to 16

        if self.__special_charset == charset: #if it is already the one being used
            return

        self.__special_charset = charset #charset being used
        self.__send_byte(0x40, True) #set charset command
        len_char_table = len(self.__charset_table) #len of charset table

        ptr = self.__charset_table[ self.__special_charset*8*8 : len_char_table] #takes a line of charset, maybe needs to change here for more efficiency

        for i in range(64):#runs through a line
            self.__send_byte(ptr[i], False) #paints a square on the screen with the special character

        pos = self.__calc_pos() #calcs the new position

        self.__send_byte(pos, True) #sends it to the screen

    def __reset_charset(self):

        charset = self.__param_buffer[0]

        charset &= 0xF

        len_char_table = len(self.__charset_table)
        len_chart0 = len(self.__CHARSET0)

        dest = self.__charset_table[ charset * 64 : len_char_table ]
        src = self.__CHARSET0[ charset * 64 : len_chart0 ]

        for i in range(64):
            dest[i] = src[i]

        self.__special_charset = 0xFF
        self.__set_special_charset(charset)

    def __set_custom_charset(self):

        charset = self.__param_buffer[0]

        charset &= 0xF

        len_char_table = len(self.__charset_table)

        dest = self.__charset_table[ charset * 64 : len_char_table ]

        for i in range(64):
            dest[i] = self.__param_buffer[i+1]


        self.__special_charset = 0xFF

        self.__set_special_charset(charset)

    def __esc_init(self):
        for i in range(1024):
            self.__charset_table[i] = self.__CHARSET0[i] #inits charset table with the charset0
        self.__set_special_charset(0)#sets the charset being used to the first line

    def __process_esc_char(self, ch):
        if not self.__in_esc: #if it is not in escape sequence yet
            self.__esc_mode = self.__ESC_MODE_NEED_CMD #next mode is NEED COMMAND
            self.__in_esc = True #activates the esc sequence
            return False

        if self.__esc_mode == self.__ESC_MODE_NEED_CMD: #if is in NEED COMMAND part

            self.__esc_mode = self.__ESC_MODE_NEED_PARAM #the nex mode is NEED PARAMETER
            self.__esc_cmd = chr(ch[0]) # takes the byte as the command to be used
            self.__param_buffer_len = 0 #no data in the buffer

            #defines the bytes needed according to every command

            if self.__esc_cmd == 'A' or self.__esc_cmd == 'r':
                self.__param_bytes_needed = 2
                return False
            elif self.__esc_cmd == 'B':
                self.__param_bytes_needed = 22
                return False
            elif (self.__esc_cmd == 'C' or self.__esc_cmd == 'S' or self.__esc_cmd == 'R'
               or self.__esc_cmd == 'M' or self.__esc_cmd == 'c' or self.__esc_cmd == 'K' or self.__esc_cmd == 's'):
                self.__param_bytes_needed = 1
                return False
            elif self.__esc_cmd == 'O':
                self.__param_bytes_needed = 4
                return False
            elif self.__esc_cmd == 'L':
                self.__param_bytes_needed = 65
                return False
            elif self.__esc_cmd == 'Z' or self.__esc_cmd == 'z' or self.__esc_cmd == 'a':
                self.__param_bytes_needed = 128
                return False
            elif self.__esc_cmd == 'D':
                self.__param_bytes_needed = 17
                return False
            elif self.__esc_cmd == 'k':
                self.__param_bytes_needed = 16
                return False
            else:
                self.__in_esc = False
                return False

        elif self.__esc_mode == self.__ESC_MODE_NEED_PARAM: #if it is in NEEDS PARAMETER part

            self.__param_buffer[self.__param_buffer_len] = ch[0] #sets the next param_buffer free byte as the command sent
            self.__param_buffer_len += 1 #buffer size + 1

            if self.__param_buffer_len == self.__param_bytes_needed: #if the bytes needed were already been fulfilled
                self.__esc_mode = self.__ESC_MODE_NEED_EOS #goes to the next part of the esc char process
            return False

        elif self.__ESC_MODE_NEED_EOS: #if the command needs an ending
            self.__in_esc = False #it finishes the escape sequence

            if self.__esc_cmd == 'A': #positions the cursor
                # if the cursor is out of boundaries
                if self.__param_buffer[0] >= self.__lcd_w or self.__param_buffer[1] >= self.__lcd_h:
                    return False

                self.__lcd_x = self.__param_buffer[0] #position of x
                self.__lcd_y = self.__param_buffer[1] #position of y
                ch[0] = self.__calc_pos() #returns the new position
                return True

            elif self.__esc_cmd == 'B': #beeo function
                #add beep function
                return False

            elif self.__esc_cmd == 'C':#beep function
                #add beep function
                return False

            elif self.__esc_cmd == 'S': #set special charset
                self.__set_special_charset(self.__param_buffer[0])
                return False

            elif self.__esc_cmd == 'L':#set a custom charset
                self.__set_custom_charset()
                return False

            elif self.__esc_cmd == 'R':#resets charset
                self.__reset_charset()
                return False
            #elif self.__esc_cmd == 'M': I dont have a clue what the code does, but it is menu, so not now
                #add beep function
                #return False
            elif self.__esc_cmd == 'c':#cursor activate, deactivate or blink
                if self.__param_buffer[0] == 0:
                    ch[0] = 0x0C
                    return True
                elif self.__param_buffer[0] == 1:
                    ch[0] = (0x0C | 0x02)
                    return True
                ch[0] = (0x0C | 0x03)
                return True

            elif self.__esc_cmd == 'r':#keyboard function
                #keyboard auto repeat
                return False
            elif self.__esc_cmd == 'Z':#keyboard function
                #keyboard set key table
                return False
            elif self.__esc_cmd == 'z':#keyboard function
                #keyboard set key shift table
                return False
            elif self.__esc_cmd == 'a':#keyboard function
                #keyboard set key attribute table
                return False
            elif self.__esc_cmd == 'O':#keyboard function
                #keyboard set control
                return False
            elif self.__esc_cmd == 'K':#keyboard function
                #sound on keyboard input
                return False
            elif self.__esc_cmd == 'k':#keyboard function
                #keyboard set column addresses
                return False
            elif self.__esc_cmd == 'D':#keyboard function
                #KBD set switches
                return False
            elif self.__esc_cmd == 'd':#disable special charst
                self.__lcd_disable_special_charset()
                return False

        return False

    def __process_control_char(self, ch): #process control char
        if ch[0] == 0x00: #beep function
            #beep on
            return False

        elif ch[0] == self.CLR or ch[0] == self.HOME: #clear or set cursor home
            self.__lcd_x = 0
            self.__lcd_y = 0 # set both positions to 0
            return True

        elif ch[0] == self.FS:#cursor forward
            self.__inc_pos()#increments position
            ch[0] = self.__calc_pos()#returns the position
            return True

        elif ch[0] == 0x07:#beep function
            #beep on
            return False

        elif ch[0] == self.BS:# cursor backwards
            if self.__lcd_x == 0: #if x is already at te left
                self.__lcd_x = self.__lcd_w - 1 #x to the end of the previous line
                if self.__lcd_y == 0:#if the cursor is in the first line
                    self.__lcd_y = self.__lcd_h - 1 #goes to the last line
                else:
                    self.__lcd_y -= 1 #else goes to the previous line
            else:
                self.__lcd_x -= 1 #walks back
            ch[0] = self.__calc_pos()#returns the new position
            return True
        elif ch[0] == self.DO:# down
            self.__lcd_y += 1 #y goes to the next line
            if self.__lcd_y >= self.__lcd_h: #if it is out of the boundaries
                self.__lcd_y = 0 # goes to the start of the line
            ch[0] = self.__calc_pos() #returns the new pos
            return True
        elif ch[0] == self.UP:#y goes to the previous line
            if self.__lcd_y == 0: #if its already at the top line
                return False
            self.__lcd_y -= 1 #goes one line up
            ch[0] = self.__calc_pos() #returns the new position
            return True
        elif ch[0] == self.FF: #sets the cursor to the initial position and clean the screen
            self.__lcd_x = 0
            self.__lcd_y = 0
            ch[0] = 0x01
            return True
        elif ch[0] == self.CR: #goes to the start of the actual line
            if self.__lcd_x == 0: #if x is already 0 does nothing
                return False
            self.__lcd_x = 0 #else x = 0
            ch[0] = self.__calc_pos() #returns the new position
            return True
        elif ch[0] == 0x14: #beeper command
            #beep on
            return False
        return False


def starxy(x,y):
    lcd.print(["<1Bh>A", chr(x), chr(y), "<F0h>*"])
    pyb.delay(200)
    lcd.print(["<1Bh>A", chr(x), chr(y), "<F0h> "])

cset = [
        0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,
        0x01,0x02,0x02,0x04,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x04,0x02,0x02,0x01,0x00,
        0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x00,
        0x00,0x00,0x00,0x04,0x08,0x08,0x10,0x00,
        0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,
        0x10,0x08,0x08,0x04,0x00,0x00,0x00,0x00
]


lcd = LCD1(lcd_4_bit=False) #inits lcd instance

print("Start")

# ------------------------- SELECT_FONT.TIG ------------------------------------------------

pyb.delay(1000)
lcd.print("<1Bh>c<0><F0h>") #sets cursor to the beginning and clears the screen
lcd.print("<1>Select Font:") #clear
pyb.delay(2000)

lcd.print("<1Bh>c<0><F0h>")
lcd.print("<1>")

for x in range(8):
    lcd.print([0x80+x, "-"])

for x in range(16):
   ch = '<' + str(x) + '>'
   lcd.print(["<1Bh>S",ch,"<F0h>"])
   pyb.delay(2000)

# ------------------------- LOAD_FONT.TIG ------------------------------------------------

pyb.delay(1000)
lcd.print("<1Bh>c<0><F0h>")
lcd.print("<1>Load Font:")
pyb.delay(2000)

lcd.print("<1Bh>c<0><F0h>")
lcd.print("<1>")

for x in range(8):
    lcd.print([0x80+x, " "])

lcd.print("<1Bh>S<7><F0h>")

pyb.delay(2000)
lcd.print("<1Bh>L<7><F0h>")
lcd.print(cset)
lcd.print("<F0h>")
pyb.delay(2000)
lcd.print("<1Bh>R<7><F0h>")


# ------------------------- CURSOR.TIG ------------------------------------------------

pyb.delay(1000)
lcd.print("<1Bh>c<0><F0h>")
lcd.print("<1>Cursor TIG:")
pyb.delay(2000)

for x in range(3):
    lcd.print(["<1Bh>c", x, "<F0h>"])
    if x == 0:
        lcd.print("<1>Cursor off")
    elif x == 1:
        lcd.print("<1>Cursor on")
    elif x == 2:
        lcd.print("<1>Cursor blinks")
    pyb.delay(3000)


pyb.delay(1000)
lcd.print("<1Bh>c<0><F0h>")
lcd.print("<1>Go to XY:")
pyb.delay(2000)

lcd.print("<1Bh>c<0><F0h>")
lcd.print("<1>")

for a in range(2):
    for x in range(20):
        starxy(x,1)
    for x in range(18, 0, -1):
        starxy(x, 1)

#-----------------------------------------------------------------------------------------



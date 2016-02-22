import pyb
import struct
import sys
import utime

MODE_RTU   = 'rtu'
MODE_ASCII = 'ascii'

CLOSE_PORT_AFTER_EACH_CALL = False

_NUMBER_OF_BYTES_PER_REGISTER = 2

_LATEST_READ_TIMES = {}

class Instrument():

    def __init__(self, uartObj, slaveaddress, mode=MODE_RTU):
        self.serial = uartObj
        self.address = slaveaddress

        self.mode = mode

        self.debug = False

        self.close_port_after_each_call = CLOSE_PORT_AFTER_EACH_CALL

        self.precalculate_read_size = True
        
        self.handle_local_echo = False

        self.rtc = pyb.RTC()

        _LATEST_READ_TIMES[str(self.serial)] = 0

    def __repr__(self):
        return "{}<id=0x{:x}, address={}, mode={}, close_port_after_each_call={}, precalculate_read_size={}, debug={}, serial={}>".format(
            self.__module__,
            id(self),
            self.address,
            self.mode,
            self.close_port_after_each_call,
            self.precalculate_read_size,
            self.debug,
            self.serial,
            )

    def read_bit(self, registeraddress, functioncode=2):
        _checkFunctioncode(functioncode, [1, 2])
        return self._genericCommand(functioncode, registeraddress)

    def write_bit(self, registeraddress, value, functioncode=5):
        _checkFunctioncode(functioncode, [5, 15])
        _checkInt(value, minvalue=0, maxvalue=1, description='input value')
        self._genericCommand(functioncode, registeraddress, value)

    def read_register(self, registeraddress, numberOfDecimals=0, functioncode=3, signed=False):
        _checkFunctioncode(functioncode, [3, 4])
        _checkInt(numberOfDecimals, minvalue=0, maxvalue=10, description='number of decimals')
        #_checkBool(signed, description='signed')
        return self._genericCommand(functioncode, registeraddress, numberOfDecimals=numberOfDecimals, signed=signed)

    def write_register(self, registeraddress, value, numberOfDecimals=0, functioncode=16, signed=False):
        _checkFunctioncode(functioncode, [6, 16])
        _checkInt(numberOfDecimals, minvalue=0, maxvalue=10, description='number of decimals')
        #_checkBool(signed, description='signed')
        _checkNumerical(value, description='input value')
        self._genericCommand(functioncode, registeraddress, value, numberOfDecimals, signed=signed)








    def _genericCommand(self, functioncode, registeraddress, value=None, numberOfDecimals=0, numberOfRegisters=1, signed=False, payloadformat=None):
        NUMBER_OF_BITS = 1
        NUMBER_OF_BYTES_FOR_ONE_BIT = 1
        NUMBER_OF_BYTES_BEFORE_REGISTERDATA = 1
        ALL_ALLOWED_FUNCTIONCODES = list(range(1, 7)) + [15, 16]  # To comply with both Python2 and Python3
        MAX_NUMBER_OF_REGISTERS = 255

        # Payload format constants, so datatypes can be told apart.
        # Note that bit datatype not is included, because it uses other functioncodes.
        PAYLOADFORMAT_LONG      = 'long'
        PAYLOADFORMAT_FLOAT     = 'float'
        PAYLOADFORMAT_STRING    = 'string'
        PAYLOADFORMAT_REGISTER  = 'register'
        PAYLOADFORMAT_REGISTERS = 'registers'

        ALL_PAYLOADFORMATS = [PAYLOADFORMAT_LONG, PAYLOADFORMAT_FLOAT,PAYLOADFORMAT_STRING, PAYLOADFORMAT_REGISTER, PAYLOADFORMAT_REGISTERS]

        ## Check input values ##
        _checkFunctioncode(functioncode, ALL_ALLOWED_FUNCTIONCODES)  # Note: The calling facade functions should validate this
        #_checkRegisteraddress(registeraddress)
        _checkInt(numberOfDecimals, minvalue=0, description='number of decimals')
        #_checkInt(numberOfRegisters, minvalue=1, maxvalue=MAX_NUMBER_OF_REGISTERS, description='number of registers')
        #_checkBool(signed, description='signed')

        if payloadformat is not None:
            if payloadformat not in ALL_PAYLOADFORMATS:
                raise ValueError('Wrong payload format variable. Given: {0!r}'.format(payloadformat))

        ## Check combinations of input parameters ##
        numberOfRegisterBytes = numberOfRegisters * _NUMBER_OF_BYTES_PER_REGISTER

        # Payload format
        if functioncode in [3, 4, 6, 16] and payloadformat is None:
            payloadformat = PAYLOADFORMAT_REGISTER

        if functioncode in [3, 4, 6, 16]:
            if payloadformat not in ALL_PAYLOADFORMATS:
                raise ValueError('The payload format is unknown. Given format: {0!r}, functioncode: {1!r}.'.\
                    format(payloadformat, functioncode))
        else:
            if payloadformat is not None:
                raise ValueError('The payload format given is not allowed for this function code. ' + \
                    'Given format: {0!r}, functioncode: {1!r}.'.format(payloadformat, functioncode))

        # Signed and numberOfDecimals
        if signed:
            if payloadformat not in [PAYLOADFORMAT_REGISTER, PAYLOADFORMAT_LONG]:
                raise ValueError('The "signed" parameter can not be used for this data format. ' + \
                    'Given format: {0!r}.'.format(payloadformat))

        if numberOfDecimals > 0 and payloadformat != PAYLOADFORMAT_REGISTER:
            raise ValueError('The "numberOfDecimals" parameter can not be used for this data format. ' + \
                'Given format: {0!r}.'.format(payloadformat))

                    # Number of registers
        if functioncode not in [3, 4, 16] and numberOfRegisters != 1:
            raise ValueError('The numberOfRegisters is not valid for this function code. ' + \
                'NumberOfRegisters: {0!r}, functioncode {1}.'.format(numberOfRegisters, functioncode))

        if functioncode == 16 and payloadformat == PAYLOADFORMAT_REGISTER and numberOfRegisters != 1:
            raise ValueError('Wrong numberOfRegisters when writing to a ' + \
                'single register. Given {0!r}.'.format(numberOfRegisters))
            # Note: For function code 16 there is checking also in the content conversion functions.

        # Value
        if functioncode in [5, 6, 15, 16] and value is None:
            raise ValueError('The input value is not valid for this function code. ' + \
                'Given {0!r} and {1}.'.format(value, functioncode))

        if functioncode == 16 and payloadformat in [PAYLOADFORMAT_REGISTER, PAYLOADFORMAT_FLOAT, PAYLOADFORMAT_LONG]:
            _checkNumerical(value, description='input value')

        if functioncode == 6 and payloadformat == PAYLOADFORMAT_REGISTER:
            _checkNumerical(value, description='input value')

        # Value for string
        if functioncode == 16 and payloadformat == PAYLOADFORMAT_STRING:
            _checkString(value, 'input string', minlength=1, maxlength=numberOfRegisterBytes)
            # Note: The string might be padded later, so the length might be shorter than numberOfRegisterBytes.

                    # Value for registers
        if functioncode == 16 and payloadformat == PAYLOADFORMAT_REGISTERS:
            if not isinstance(value, list):
                raise TypeError('The value parameter must be a list. Given {0!r}.'.format(value))

            if len(value) != numberOfRegisters:
                raise ValueError('The list length does not match number of registers. ' + \
                    'List: {0!r},  Number of registers: {1!r}.'.format(value, numberOfRegisters))

        ## Build payload to slave ##
        payloadToSlave = bytearray()
        __append2BytesFromInt(payloadToSlave,registeraddress)
        
        if functioncode in [1, 2]:
            __append2BytesFromInt(payloadToSlave,NUMBER_OF_BITS)

        elif functioncode in [3, 4]:
            __append2BytesFromInt(payloadToSlave,numberOfRegisters)
           

        elif functioncode == 5:
            if value == 0:
                __append2BytesFromInt(payloadToSlave,0x0000)
            else:
                __append2BytesFromInt(payloadToSlave,0xFF00)


        elif functioncode == 6:
            __append2BytesFromInt(payloadToSlave,value,numberOfDecimals)


        elif functioncode == 15:
            payloadToSlave = _numToTwoByteString(registeraddress) + \
                            _numToTwoByteString(NUMBER_OF_BITS) + \
                            _numToOneByteString(NUMBER_OF_BYTES_FOR_ONE_BIT) + \
                            _createBitpattern(functioncode, value)

        elif functioncode == 16:
            registerData = value
            ''' # por ahora no se usan
            if payloadformat == PAYLOADFORMAT_REGISTER:
                registerdata = _numToTwoByteString(value, numberOfDecimals, signed=signed)

            elif payloadformat == PAYLOADFORMAT_STRING:
                registerdata = _textstringToBytestring(value, numberOfRegisters)

            elif payloadformat == PAYLOADFORMAT_LONG:
                registerdata = _longToBytestring(value, signed, numberOfRegisters)

            elif payloadformat == PAYLOADFORMAT_FLOAT:
                registerdata = _floatToBytestring(value, numberOfRegisters)

            elif payloadformat == PAYLOADFORMAT_REGISTERS:
                registerdata = _valuelistToBytestring(value, numberOfRegisters)
            '''
            #assert len(registerdata) == numberOfRegisterBytes
            __append2BytesFromInt(payloadToSlave,numberOfRegisters)
            payloadToSlave.append(numberOfRegisterBytes)
            __append2BytesFromInt(payloadToSlave,registerData,numberOfDecimals) # cuando registerData tenga muchos bytes hay que cambiar esto


        print("payload to slave")
        print(payloadToSlave)
        print("________________")

        #Comunicate		
        payloadFromSlave = self._performCommand(functioncode, payloadToSlave)

        ## Check the contents in the response payload ##
        ## TODO 
        print("FIN")
        return payloadFromSlave

    def _performCommand(self, functioncode, payloadToSlave):
        DEFAULT_NUMBER_OF_BYTES_TO_READ = 1000
        _checkFunctioncode(functioncode, None)

        # Build request
        request = _embedPayload(self.address, self.mode, functioncode, payloadToSlave)

        # Calculate number of bytes to read
        number_of_bytes_to_read = DEFAULT_NUMBER_OF_BYTES_TO_READ
        if self.precalculate_read_size:
            try:
                number_of_bytes_to_read = _predictResponseSize(self.mode, functioncode, payloadToSlave)
            except:
                if self.debug:
                    template = 'MinimalModbus debug mode. Could not precalculate response size for Modbus {} mode. ' + \
                        'Will read {} bytes. request: {!r}'
                    _print_out(template.format(self.mode, number_of_bytes_to_read, request))

        # Communicate
        response = self._communicate(request, number_of_bytes_to_read)

        print("tengo respuesta!, tipo:")
        print(type(response))

        # Extract payload
        payloadFromSlave = _extractPayload(response, self.address, self.mode, functioncode)
        return payloadFromSlave


    def _communicate(self, request, number_of_bytes_to_read):
        _checkInt(number_of_bytes_to_read)

        if self.debug:
            _print_out('\nMinimalModbus debug mode. Writing to instrument (expecting {} bytes back): {!r} ({})'. \
                format(number_of_bytes_to_read, request, _hexlify(request)))


        # Sleep to make sure 3.5 character times have passed
        minimum_silent_period   = _calculate_minimum_silent_period(self.serial.get_baudrate())
        time_since_read         = utime.time() - _LATEST_READ_TIMES[str(self.serial)]

        if time_since_read < minimum_silent_period:
            sleep_time = minimum_silent_period - time_since_read

            if self.debug:
                template = 'MinimalModbus debug mode. Sleeping for {:.1f} ms. ' + \
                        'Minimum silent period: {:.1f} ms, time since read: {:.1f} ms.'
                text = template.format(
                    sleep_time * _SECONDS_TO_MILLISECONDS,
                    minimum_silent_period * _SECONDS_TO_MILLISECONDS,
                    time_since_read * _SECONDS_TO_MILLISECONDS)
                _print_out(text)

            utime.sleep(sleep_time)

        elif self.debug:
            template = 'MinimalModbus debug mode. No sleep required before write. ' + \
                'Time since previous read: {:.1f} ms, minimum silent period: {:.2f} ms.'
            text = template.format(
                time_since_read * _SECONDS_TO_MILLISECONDS,
                minimum_silent_period * _SECONDS_TO_MILLISECONDS)
            _print_out(text)

        # Write request
        latest_write_time = utime.time()

        print("estoy por enviar request:")
        s = ""
        for b in request:
            s+=" "+hex(b)
        print(s)
        print("largo:"+str(len(request)))
                
        self.serial.write(request)

        # Read and discard local echo
        if self.handle_local_echo:
            localEchoToDiscard = self.serial.read(len(request))
            if self.debug:
                template = 'MinimalModbus debug mode. Discarding this local echo: {!r} ({} bytes).' 
                text = template.format(localEchoToDiscard, len(localEchoToDiscard))
                _print_out(text)
            if localEchoToDiscard != request:
                template = 'Local echo handling is enabled, but the local echo does not match the sent request. ' + \
                    'Request: {!r} ({} bytes), local echo: {!r} ({} bytes).' 
                text = template.format(request, len(request), localEchoToDiscard, len(localEchoToDiscard))
                raise IOError(text)

        # Read response
        answer = self.serial.read(number_of_bytes_to_read)
        _LATEST_READ_TIMES[str(self.serial)] = utime.time()

        if self.close_port_after_each_call:
            self.serial.close()


        if self.debug:
            template = 'MinimalModbus debug mode. Response from instrument: {!r} ({}) ({} bytes), ' + \
                'roundtrip time: {:.1f} ms. Timeout setting: {:.1f} ms.\n'
            text = template.format(
                answer,
                _hexlify(answer),
                len(answer),
                (_LATEST_READ_TIMES.get(self.serial.port, 0) - latest_write_time) * _SECONDS_TO_MILLISECONDS,
                self.serial.timeout * _SECONDS_TO_MILLISECONDS)
            _print_out(text)

        if len(answer) == 0:
            raise IOError('No communication with the instrument (no answer)')

        return answer







def __append2BytesFromInt(payloadToSlave,value,numberOfDecimals=0,flagMSF=True):
    multiplier = 10 ** numberOfDecimals
    intVal = int(float(value) * multiplier)
    if flagMSF:
        payloadToSlave.append((intVal>>8)&0xFF)
        payloadToSlave.append(intVal&0xFF)
    else:
        payloadToSlave.append(intVal&0xFF)
        payloadToSlave.append((intVal>>8)&0xFF)










def _embedPayload(slaveaddress, mode, functioncode, payloaddata):
    #_checkSlaveaddress(slaveaddress)
    #_checkMode(mode)
    #_checkFunctioncode(functioncode, None)
    
    #firstPart = _numToOneByteString(slaveaddress) + _numToOneByteString(functioncode) + payloaddata
    firstPart = bytearray()
    firstPart.append(slaveaddress)
    firstPart.append(functioncode)
    for b in payloaddata:
        firstPart.append(b)

    if mode == MODE_ASCII:
        request = _ASCII_HEADER + \
                _hexencode(firstPart) + \
                _hexencode(_calculateLrcString(firstPart)) + \
                _ASCII_FOOTER
    else:
        #request = firstPart + _calculateCrcString(firstPart)
        crc = _calculateCrcString(firstPart)
        request = bytearray()
        for b in firstPart:
            request.append(b)
        request.append(crc&0xFF)
        request.append((crc>>8)&0xFF)
    return request




def _calculateCrcString(inputstring):
    # Preload a 16-bit register with ones
    register = 0xFFFF

    for char in inputstring:
        register = (register >> 8) ^ _CRC16TABLE[(register ^ int(char)) & 0xFF]
 
    return register #_numToTwoByteString(register, LsbFirst=True)



def _checkNumerical(inputvalue, minvalue=None, maxvalue=None, description='inputvalue'):
    # Type checking
    if not isinstance(description, str):
        raise TypeError('The description should be a string. Given: {0!r}'.format(description))

    if not isinstance(inputvalue, (int, float)):
        raise TypeError('The {0} must be numerical. Given: {1!r}'.format(description, inputvalue))

    if not isinstance(minvalue, (int, float, type(None))):
        raise TypeError('The minvalue must be numeric or None. Given: {0!r}'.format(minvalue))

    if not isinstance(maxvalue, (int, float, type(None))):
        raise TypeError('The maxvalue must be numeric or None. Given: {0!r}'.format(maxvalue))

    # Consistency checking
    if (not minvalue is None) and (not maxvalue is None):
        if maxvalue < minvalue:
            raise ValueError('The maxvalue must not be smaller than minvalue. Given: {0} and {1}, respectively.'.format( \
                maxvalue, minvalue))

    # Value checking
    if not minvalue is None:
        if inputvalue < minvalue:
            raise ValueError('The {0} is too small: {1}, but minimum value is {2}.'.format( \
                description, inputvalue, minvalue))

    if not maxvalue is None:
        if inputvalue > maxvalue:
            raise ValueError('The {0} is too large: {1}, but maximum value is {2}.'.format( \
                description, inputvalue, maxvalue))


def _checkInt(inputvalue, minvalue=None, maxvalue=None, description='inputvalue'):
    if not isinstance(description, str):
        raise TypeError('The description should be a string. Given: {0!r}'.format(description))

    if not isinstance(inputvalue, (int)):
        raise TypeError('The {0} must be an integer. Given: {1!r}'.format(description, inputvalue))

    if not isinstance(minvalue, (int, type(None))):
        raise TypeError('The minvalue must be an integer or None. Given: {0!r}'.format(minvalue))

    if not isinstance(maxvalue, (int, type(None))):
        raise TypeError('The maxvalue must be an integer or None. Given: {0!r}'.format(maxvalue))

    _checkNumerical(inputvalue, minvalue, maxvalue, description)



def _checkFunctioncode(functioncode, listOfAllowedValues=[]):
    FUNCTIONCODE_MIN = 1
    FUNCTIONCODE_MAX = 127
    _checkInt(functioncode, FUNCTIONCODE_MIN, FUNCTIONCODE_MAX, description='functioncode')
    if listOfAllowedValues is None:
        return
    if not isinstance(listOfAllowedValues, list):
        raise TypeError('The listOfAllowedValues should be a list. Given: {0!r}'.format(listOfAllowedValues))
    for value in listOfAllowedValues:
        _checkInt(value, FUNCTIONCODE_MIN, FUNCTIONCODE_MAX, description='functioncode inside listOfAllowedValues')
    if functioncode not in listOfAllowedValues:
        raise ValueError('Wrong function code: {0}, allowed values are {1!r}'.format(functioncode, listOfAllowedValues))


















def _numToTwoByteString(value, numberOfDecimals=0, LsbFirst=False, signed=False):
    _checkNumerical(value, description='inputvalue')
    _checkInt(numberOfDecimals, minvalue=0, description='number of decimals')
    #_checkBool(LsbFirst, description='LsbFirst')
    #_checkBool(signed, description='signed parameter')

    multiplier = 10 ** numberOfDecimals
    integer = int(float(value) * multiplier)

    if LsbFirst:
        formatcode = '<'  # Little-endian
    else:
        formatcode = '>'  # Big-endian
    if signed:
        formatcode += 'h'  # (Signed) short (2 bytes)
    else:
        formatcode += 'H'  # Unsigned short (2 bytes)

    outstring = _pack(formatcode, integer)
    #print(outstring)
    assert len(outstring) == 2
    return outstring


def _pack(formatstring, value):
    _checkString(formatstring, description='formatstring', minlength=1)

    try:
        result = struct.pack(formatstring, value)
    except:
        errortext = 'The value to send is probably out of range, as the num-to-bytestring conversion failed.'
        errortext += ' Value: {0!r} Struct format code is: {1}'
        raise ValueError(errortext.format(value, formatstring))

    print("pack!")
    print("result en bytes:")
    print(result)
    
    if sys.version_info[0] > 2:
        #return str(result, encoding='latin1')  # Convert types to make it Python3 compatible
        result = str(result,"utf-8")  # Convert types to make it Python3 compatible
    print("result en str:")
    print(result)
    return result

def _createBitpattern(functioncode, value):
    _checkFunctioncode(functioncode, [5, 15])
    _checkInt(value, minvalue=0, maxvalue=1, description='inputvalue')

    if functioncode == 5:
        if value == 0:
            print("aaaaaaaaaaaaaa devuelve 0")
            return str('\x00\x00',"utf-8")
        else:
            print("bbbbbbbbbbbbbbb devuelve ff")
            s =  str('\xff\x00',"utf-8")
            print(s)
            print("len:")
            print(len(s))
            return s

    elif functioncode == 15:
        if value == 0:
            return str('\x00',"utf-8")
        else:
            return str('\x01',"utf-8")  # Is this correct??

def _numToOneByteString(inputvalue):
    _checkInt(inputvalue, minvalue=0, maxvalue=0xFF)
    return chr(inputvalue)













def _checkString(inputstring, description, minlength=0, maxlength=None):
    # Type checking
    if not isinstance(description, str):
        raise TypeError('The description should be a string. Given: {0!r}'.format(description))

    if not isinstance(inputstring, str):
        raise TypeError('The {0} should be a string. Given: {1!r}'.format(description, inputstring))

    if not isinstance(maxlength, (int, type(None))):
        raise TypeError('The maxlength must be an integer or None. Given: {0!r}'.format(maxlength))

    # Check values
    _checkInt(minlength, minvalue=0, maxvalue=None, description='minlength')

    if len(inputstring) < minlength:
        raise ValueError('The {0} is too short: {1}, but minimum value is {2}. Given: {3!r}'.format( \
            description, len(inputstring), minlength, inputstring))

    if not maxlength is None:
        if maxlength < 0:
            raise ValueError('The maxlength must be positive. Given: {0}'.format(maxlength))

        if maxlength < minlength:
            raise ValueError('The maxlength must not be smaller than minlength. Given: {0} and {1}'.format( \
                maxlength, minlength))

        if len(inputstring) > maxlength:
            raise ValueError('The {0} is too long: {1}, but maximum value is {2}. Given: {3!r}'.format( \
                description, len(inputstring), maxlength, inputstring))



_CRC16TABLE = (
        0, 49345, 49537,   320, 49921,   960,   640, 49729, 50689,  1728,  1920, 
    51009,  1280, 50625, 50305,  1088, 52225,  3264,  3456, 52545,  3840, 53185, 
    52865,  3648,  2560, 51905, 52097,  2880, 51457,  2496,  2176, 51265, 55297, 
     6336,  6528, 55617,  6912, 56257, 55937,  6720,  7680, 57025, 57217,  8000, 
    56577,  7616,  7296, 56385,  5120, 54465, 54657,  5440, 55041,  6080,  5760, 
    54849, 53761,  4800,  4992, 54081,  4352, 53697, 53377,  4160, 61441, 12480, 
    12672, 61761, 13056, 62401, 62081, 12864, 13824, 63169, 63361, 14144, 62721, 
    13760, 13440, 62529, 15360, 64705, 64897, 15680, 65281, 16320, 16000, 65089, 
    64001, 15040, 15232, 64321, 14592, 63937, 63617, 14400, 10240, 59585, 59777, 
    10560, 60161, 11200, 10880, 59969, 60929, 11968, 12160, 61249, 11520, 60865, 
    60545, 11328, 58369,  9408,  9600, 58689,  9984, 59329, 59009,  9792,  8704, 
    58049, 58241,  9024, 57601,  8640,  8320, 57409, 40961, 24768, 24960, 41281, 
    25344, 41921, 41601, 25152, 26112, 42689, 42881, 26432, 42241, 26048, 25728, 
    42049, 27648, 44225, 44417, 27968, 44801, 28608, 28288, 44609, 43521, 27328, 
    27520, 43841, 26880, 43457, 43137, 26688, 30720, 47297, 47489, 31040, 47873, 
    31680, 31360, 47681, 48641, 32448, 32640, 48961, 32000, 48577, 48257, 31808, 
    46081, 29888, 30080, 46401, 30464, 47041, 46721, 30272, 29184, 45761, 45953, 
    29504, 45313, 29120, 28800, 45121, 20480, 37057, 37249, 20800, 37633, 21440, 
    21120, 37441, 38401, 22208, 22400, 38721, 21760, 38337, 38017, 21568, 39937, 
    23744, 23936, 40257, 24320, 40897, 40577, 24128, 23040, 39617, 39809, 23360, 
    39169, 22976, 22656, 38977, 34817, 18624, 18816, 35137, 19200, 35777, 35457, 
    19008, 19968, 36545, 36737, 20288, 36097, 19904, 19584, 35905, 17408, 33985, 
    34177, 17728, 34561, 18368, 18048, 34369, 33281, 17088, 17280, 33601, 16640, 
    33217, 32897, 16448)

def _hexencode(bytestring, insert_spaces = False):
    _checkString(bytestring, description='byte string')
    separator = '' if not insert_spaces else ' '
    byte_representions = []
    for c in bytestring:
        byte_representions.append( '{0:02X}'.format(ord(c)) )
    return separator.join(byte_representions).strip()


def _hexdecode(hexstring):
    _checkString(hexstring, description='hexstring')

    if len(hexstring) % 2 != 0:
        raise ValueError('The input hexstring must be of even length. Given: {!r}'.format(hexstring))

    try:
        return hexstring.decode('hex')
    except TypeError as err:
        raise TypeError('Hexdecode reported an error: {}. Input hexstring: {}'.format(err.message, hexstring))


def _calculate_minimum_silent_period(baudrate):
    _checkNumerical(baudrate, minvalue=1, description='baudrate')  # Avoid division by zero
    BITTIMES_PER_CHARACTERTIME = 11
    MINIMUM_SILENT_CHARACTERTIMES = 3.5
    bittime = 1 / float(baudrate)
    return bittime * BITTIMES_PER_CHARACTERTIME * MINIMUM_SILENT_CHARACTERTIMES

def _extractPayload(response, slaveaddress, mode, functioncode):
    BYTEPOSITION_FOR_ASCII_HEADER          = 0  # Relative to plain response

    BYTEPOSITION_FOR_SLAVEADDRESS          = 0  # Relative to (stripped) response
    BYTEPOSITION_FOR_FUNCTIONCODE          = 1

    NUMBER_OF_RESPONSE_STARTBYTES          = 2  # Number of bytes before the response payload (in stripped response)
    NUMBER_OF_CRC_BYTES                    = 2
    NUMBER_OF_LRC_BYTES                    = 1
    BITNUMBER_FUNCTIONCODE_ERRORINDICATION = 7

    MINIMAL_RESPONSE_LENGTH_RTU            = NUMBER_OF_RESPONSE_STARTBYTES + NUMBER_OF_CRC_BYTES
    MINIMAL_RESPONSE_LENGTH_ASCII          = 9

    # Argument validity testing
    #_checkSlaveaddress(slaveaddress)
    #_checkMode(mode)
    _checkFunctioncode(functioncode, None)

    plainresponse = response

    # Validate response length
    if mode == MODE_ASCII:
        if len(response) < MINIMAL_RESPONSE_LENGTH_ASCII:
            raise ValueError('Too short Modbus ASCII response (minimum length {} bytes). Response: {!r}'.format( \
                MINIMAL_RESPONSE_LENGTH_ASCII,
                response))
    elif len(response) < MINIMAL_RESPONSE_LENGTH_RTU:
            raise ValueError('Too short Modbus RTU response (minimum length {} bytes). Response: {!r}'.format( \
                MINIMAL_RESPONSE_LENGTH_RTU,
                response))

    # Validate the ASCII header and footer.
    if mode == MODE_ASCII:
        if response[BYTEPOSITION_FOR_ASCII_HEADER] != _ASCII_HEADER:
            raise ValueError('Did not find header ({!r}) as start of ASCII response. The plain response is: {!r}'.format( \
                _ASCII_HEADER,
                response))
        elif response[-len(_ASCII_FOOTER):] != _ASCII_FOOTER:
            raise ValueError('Did not find footer ({!r}) as end of ASCII response. The plain response is: {!r}'.format( \
                _ASCII_FOOTER,
                response))

        # Strip ASCII header and footer
        response = response[1:-2]

        if len(response) % 2 != 0:
            template = 'Stripped ASCII frames should have an even number of bytes, but is {} bytes. ' + \
                    'The stripped response is: {!r} (plain response: {!r})'
            raise ValueError(template.format(len(response), response, plainresponse))

        # Convert the ASCII (stripped) response string to RTU-like response string
        response = _hexdecode(response)

    # Validate response checksum
    if mode == MODE_ASCII:
        calculateChecksum = _calculateLrcString
        numberOfChecksumBytes = NUMBER_OF_LRC_BYTES
    else:
        calculateChecksum = _calculateCrcString
        numberOfChecksumBytes = NUMBER_OF_CRC_BYTES

    receivedChecksum = response[-numberOfChecksumBytes:]
    responseWithoutChecksum = response[0 : len(response) - numberOfChecksumBytes]
    calculatedChecksum = calculateChecksum(responseWithoutChecksum)

    if (receivedChecksum[0] != calculatedChecksum&0xFF) or (receivedChecksum[1] != (calculatedChecksum>>8)&0xFF):
        template = 'Checksum error in {} mode: {!r} instead of {!r} . The response is: {!r} (plain response: {!r})'
        text = template.format(
                mode,
                receivedChecksum,
                calculatedChecksum,
                response, plainresponse)
        raise ValueError(text)

    # Check slave address
    responseaddress = int(response[BYTEPOSITION_FOR_SLAVEADDRESS])

    if responseaddress != slaveaddress:
        raise ValueError('Wrong return slave address: {} instead of {}. The response is: {!r}'.format( \
            responseaddress, slaveaddress, response))

    # Check function code
    receivedFunctioncode = int(response[BYTEPOSITION_FOR_FUNCTIONCODE])

    if receivedFunctioncode == (functioncode | (1 << BITNUMBER_FUNCTIONCODE_ERRORINDICATION)): # _setBitOn(functioncode, BITNUMBER_FUNCTIONCODE_ERRORINDICATION):
        raise ValueError('The slave is indicating an error. The response is: {!r}'.format(response))

    elif receivedFunctioncode != functioncode:
        raise ValueError('Wrong functioncode: {} instead of {}. The response is: {!r}'.format( \
            receivedFunctioncode, functioncode, response))

    # Read data payload
    firstDatabyteNumber = NUMBER_OF_RESPONSE_STARTBYTES

    if mode == MODE_ASCII:
        lastDatabyteNumber = len(response) - NUMBER_OF_LRC_BYTES
    else:
        lastDatabyteNumber = len(response) - NUMBER_OF_CRC_BYTES

    payload = response[firstDatabyteNumber:lastDatabyteNumber]
    return payload

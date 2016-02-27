import pyb
import struct
import sys
#import utime

MODE_RTU   = 'rtu'
MODE_ASCII = 'ascii'

CLOSE_PORT_AFTER_EACH_CALL = False

_NUMBER_OF_BYTES_PER_REGISTER = 2

_LATEST_READ_TIMES = {}

_ASCII_HEADER = ':'
_ASCII_FOOTER = '\r\n'

class Instrument():

    def __init__(self, uartObj, slaveaddress, mode=MODE_RTU):
        self.serial = uartObj
        self.address = slaveaddress

        self.mode = mode

        self.debug = False

        self.close_port_after_each_call = CLOSE_PORT_AFTER_EACH_CALL

        self.precalculate_read_size = True
        
        self.handle_local_echo = False

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


    def read_registers(self, registeraddress, numberOfRegisters, functioncode=3):
        _checkFunctioncode(functioncode, [3, 4])
        _checkInt(numberOfRegisters, minvalue=1, description='number of registers')
        return self._genericCommand(functioncode, registeraddress, \
            numberOfRegisters=numberOfRegisters, payloadformat='registers')



    def write_registers(self, registeraddress, values):
        if not isinstance(values, list):
            raise TypeError('The "values parameter" must be a list. Given: {0!r}'.format(values))
        _checkInt(len(values), minvalue=1, description='length of input list')
        self._genericCommand(16, registeraddress, values, numberOfRegisters=len(values), payloadformat='registers')







    def _genericCommand(self, functioncode, registeraddress, value=None, numberOfDecimals=0, numberOfRegisters=1, signed=False, payloadformat=None):
        NUMBER_OF_BITS = 1
        NUMBER_OF_BYTES_FOR_ONE_BIT = 1
        NUMBER_OF_BYTES_BEFORE_REGISTERDATA = 1
        ALL_ALLOWED_FUNCTIONCODES = list(range(1, 7)) + [15, 16]  # To comply with both Python2 and Python3
        MAX_NUMBER_OF_REGISTERS = 255

        # Payload format constants, so datatypes can be told apart.
        PAYLOADFORMAT_LONG      = 'long'
        PAYLOADFORMAT_FLOAT     = 'float'
        PAYLOADFORMAT_STRING    = 'string'
        PAYLOADFORMAT_REGISTER  = 'register'
        PAYLOADFORMAT_REGISTERS = 'registers'

        ALL_PAYLOADFORMATS = [PAYLOADFORMAT_LONG, PAYLOADFORMAT_FLOAT,PAYLOADFORMAT_STRING, PAYLOADFORMAT_REGISTER, PAYLOADFORMAT_REGISTERS]

        ## Check input values ##
        _checkFunctioncode(functioncode, ALL_ALLOWED_FUNCTIONCODES)  # Note: The calling facade functions should validate this
        _checkRegisteraddress(registeraddress)
        _checkInt(numberOfDecimals, minvalue=0, description='number of decimals')
        _checkInt(numberOfRegisters, minvalue=1, maxvalue=MAX_NUMBER_OF_REGISTERS, description='number of registers')
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
            __append2BytesFromInt(payloadToSlave,NUMBER_OF_BITS)
            payloadToSlave.append(NUMBER_OF_BYTES_FOR_ONE_BIT)
            if value == 0:
                __append2BytesFromInt(payloadToSlave,0x0000)
            else:
                __append2BytesFromInt(payloadToSlave,0xFF00)


        elif functioncode == 16:
            __append2BytesFromInt(payloadToSlave,numberOfRegisters)
            payloadToSlave.append(numberOfRegisterBytes)

            if payloadformat == PAYLOADFORMAT_REGISTER:
                registerData = value
                __append2BytesFromInt(payloadToSlave,registerData,numberOfDecimals)
            elif payloadformat == PAYLOADFORMAT_REGISTERS:
                for reg in value:
                    __append2BytesFromInt(payloadToSlave,reg,numberOfDecimals)


        #Comunicate		
        payloadFromSlave = self._performCommand(functioncode, payloadToSlave)

        ## Check the contents in the response payload ##
        if functioncode in [1, 2, 3, 4]:
            _checkResponseByteCount(payloadFromSlave)
         
        if functioncode in [5, 6, 15, 16]:
            _checkResponseRegisterAddress(payloadFromSlave, registeraddress)

        if functioncode == 5:
            if value == 0:
                _checkResponseWriteData(payloadFromSlave, 0x0000)
            else:
                _checkResponseWriteData(payloadFromSlave, 0xFF00)
         
        if functioncode == 6:
            aux = bytearray()
            __append2BytesFromInt(aux,value,numberOfDecimals)
            _checkResponseWriteData(payloadFromSlave, aux[0]<<8 | aux[1] )

        if functioncode == 15:
            _checkResponseNumberOfRegisters(payloadFromSlave, NUMBER_OF_BITS)

        if functioncode == 16:
            _checkResponseNumberOfRegisters(payloadFromSlave, numberOfRegisters)


        ## Calculate return value ##
        if functioncode in [1, 2]:
            registerdata = payloadFromSlave[NUMBER_OF_BYTES_BEFORE_REGISTERDATA:]
            if len(registerdata) != NUMBER_OF_BYTES_FOR_ONE_BIT:
                raise ValueError('The registerdata length does not match NUMBER_OF_BYTES_FOR_ONE_BIT. ' + \
                    'Given {0}.'.format(len(registerdata)))
            return _bitResponseToValue(registerdata)


        if functioncode in [3, 4]:
            registerdata = payloadFromSlave[NUMBER_OF_BYTES_BEFORE_REGISTERDATA:]
            if len(registerdata) != numberOfRegisterBytes:
                raise ValueError('The registerdata length does not match number of register bytes. ' + \
                    'Given {0!r} and {1!r}.'.format(len(registerdata), numberOfRegisterBytes))
            ''' 
            if payloadformat == PAYLOADFORMAT_STRING:
                return _bytestringToTextstring(registerdata, numberOfRegisters)

            elif payloadformat == PAYLOADFORMAT_LONG:
                return _bytestringToLong(registerdata, signed, numberOfRegisters)

            elif payloadformat == PAYLOADFORMAT_FLOAT:
                return _bytestringToFloat(registerdata, numberOfRegisters)
            '''

            if payloadformat == PAYLOADFORMAT_REGISTERS:
                return _bytesResponseToValuelist(registerdata, numberOfRegisters)

            elif payloadformat == PAYLOADFORMAT_REGISTER:
                return _bytesResponseToInt(registerdata, numberOfDecimals, signed)

            raise ValueError('Wrong payloadformat for return value generation. ' + \
                'Given {0}'.format(payloadformat))

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
                pass

        # Communicate
        print("llamo a comunicate, bytes a leer:"+str(number_of_bytes_to_read))
        response = self._communicate(request, number_of_bytes_to_read)

        # Extract payload
        payloadFromSlave = _extractPayload(response, self.address, self.mode, functioncode)
        return payloadFromSlave


    def _communicate(self, request, number_of_bytes_to_read):
        _checkInt(number_of_bytes_to_read)

        # Sleep to make sure 3.5 character times have passed
        minimum_silent_period   = _calculate_minimum_silent_period(self.serial.get_baudrate())
        #time_since_read         = utime.time() - _LATEST_READ_TIMES[str(self.serial)]

        #if time_since_read < minimum_silent_period:
        #    sleep_time = minimum_silent_period - time_since_read
        #    utime.sleep(sleep_time)
        pyb.delay(minimum_silent_period)

        # Write request
        #latest_write_time = utime.time()

        print("estoy por enviar request:")
        s = ""
        if self.mode==MODE_RTU:
            for b in request:
                s+=" "+hex(b)
        else:
            s = request
        print(s)
        print("largo:"+str(len(request)))
                
        self.serial.write(request)

        # Read and discard local echo
        if self.handle_local_echo:
            localEchoToDiscard = self.serial.read(len(request))
            if localEchoToDiscard != request:
                template = 'Local echo handling is enabled, but the local echo does not match the sent request. ' + \
                    'Request: {!r} ({} bytes), local echo: {!r} ({} bytes).' 
                text = template.format(request, len(request), localEchoToDiscard, len(localEchoToDiscard))
                raise Exception(text)

        # Read response
        print("leo respuesta....")
        answer = bytearray()
        timeOutRead = 200
        while True:
            if self.serial.any():
                answer = self.serial.read(number_of_bytes_to_read)
                break
            else:
                pyb.delay(10)
                timeOutRead-=1
                if timeOutRead<=0:
                    break
        #_________                       
        print("llego respuesta:")
        print(answer)

        #_LATEST_READ_TIMES[str(self.serial)] = utime.time()

        if len(answer) == 0:
            raise Exception('No communication with the instrument (no answer)')

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
        crc = _calculateCrcString(firstPart)
        request = bytearray()
        for b in firstPart:
            request.append(b)
        request.append(crc&0xFF)
        request.append((crc>>8)&0xFF)
    return request



def _calculateCrcString(inputstring):
    register = 0xFFFF
    for char in inputstring:
        register = (register >> 8) ^ _CRC16TABLE[(register ^ int(char)) & 0xFF]
 
    return register

def _calculateLrcString(inputstring):
    register = 0
    for character in inputstring:
        register += character

    lrc = ((register ^ 0xFF) + 1) & 0xFF

    out = bytearray()
    out.append(lrc)
    return out



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


def _checkRegisteraddress(registeraddress):
    REGISTERADDRESS_MAX = 0xFFFF
    REGISTERADDRESS_MIN = 0
    _checkInt(registeraddress, REGISTERADDRESS_MIN, REGISTERADDRESS_MAX, description='registeraddress')




def _checkResponseByteCount(payload):
    givenNumberOfDatabytes = payload[0]
    countedNumberOfDatabytes = len(payload)-1
    if givenNumberOfDatabytes != countedNumberOfDatabytes:
        errortemplate = 'Wrong given number of bytes in the response: {0}, but counted is {1} as data payload length is {2}.' + \
            ' The data payload is: {3!r}'
        errortext = errortemplate.format(givenNumberOfDatabytes, countedNumberOfDatabytes, len(payload), payload)
        raise ValueError(errortext)

def _checkResponseRegisterAddress(payload, registeraddress):
    receivedStartAddress = payload[0]<<8 | payload[1]
    if receivedStartAddress!= registeraddress:
        raise ValueError('Wrong given write start adress: {0}, but commanded is {1}. The data payload is: {2!r}'.format( \
            receivedStartAddress, registeraddress, payload))


def _checkResponseWriteData(payload, writedata):
    receivedWritedata = payload[2]<<8 | payload[3]
    if receivedWritedata != writedata:
        raise ValueError('Wrong write data in the response: {0!r}, but commanded is {1!r}. The data payload is: {2!r}'.format( \
            receivedWritedata, writedata, payload))


def _checkResponseNumberOfRegisters(payload, numberOfRegisters):
    receivedNumberOfWrittenReisters = payload[2]<<8 | payload[3]
    if receivedNumberOfWrittenReisters != numberOfRegisters:
        raise ValueError('Wrong number of registers to write in the response: {0}, but commanded is {1}. The data payload is: {2!r}'.format( \
            receivedNumberOfWrittenReisters, numberOfRegisters, payload))


def _bitResponseToValue(bytesdata):
    if bytesdata[0] == 0x01:
        return 1
    elif bytesdata[0] == 0x00:
        return 0
    else:
        raise ValueError('Could not convert bit response to a value. Input: {0!r}'.format(bytesdata))


def _bytesResponseToInt(bytesdata, numberOfDecimals,signed=False):
    reg = bytesdata[0]<<8 | bytesdata[1]
    if numberOfDecimals>0:
        reg = reg / (10**numberOfDecimals)
    return reg
    

def _bytesResponseToValuelist(bytesdata,numberOfRegisters):
    out = []
    index = 0
    while numberOfRegisters>0:
        reg =bytesdata[index]<<8 | bytesdata[index+1]
        out.append(reg)
        index+=2
        numberOfRegisters-=1
    return out


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
    separator = '' if not insert_spaces else ' '
    byte_representions = []
    for c in bytestring:
        byte_representions.append( '{0:02X}'.format(c) )
    return separator.join(byte_representions).strip()


def _hexdecode(hexstring):

    if len(hexstring) % 2 != 0:
        raise ValueError('The input hexstring must be of even length. Given: {!r}'.format(hexstring))

    #try:
    print("decodifigo hex")
    print(hexstring)
    #return hexstring.decode('hex')
    index = 0
    out = bytearray()
    while len(hexstring)/2 > index:
        out.append( (hexstring[index]-0x30)<<4 | (hexstring[index+1]-0x30) )
        index+=2
    print(out)
    return out
    #except TypeError as err:
    #raise TypeError('Hexdecode reported an error: {}. Input hexstring: {}'.format(err.message, hexstring))
    #raise Exception(err)

def _calculate_minimum_silent_period(baudrate):
    _checkNumerical(baudrate, minvalue=1, description='baudrate')  # Avoid division by zero
    BITTIMES_PER_CHARACTERTIME = 11
    MINIMUM_SILENT_CHARACTERTIMES = 3.5
    bittime = 1 / float(baudrate)
    return int((bittime * BITTIMES_PER_CHARACTERTIME * MINIMUM_SILENT_CHARACTERTIMES)*1000)

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
        if response[BYTEPOSITION_FOR_ASCII_HEADER] != ord(_ASCII_HEADER):
            raise ValueError('Did not find header ({!r}) as start of ASCII response. The plain response is: {!r}'.format( \
                _ASCII_HEADER,
                response))
        elif response[-len(_ASCII_FOOTER):][0] != ord(_ASCII_FOOTER[0]) and response[-len(_ASCII_FOOTER):][1] != ord(_ASCII_FOOTER[1]):
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

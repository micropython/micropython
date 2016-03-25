import pyb
import struct
import sys

MODE_RTU   = 'rtu'
MODE_ASCII = 'ascii'

_NUMBER_OF_BYTES_PER_REGISTER = 2

_ASCII_HEADER = ':'
_ASCII_FOOTER = '\r\n'

class Instrument():

    def __init__(self, uartObj, slaveaddress, mode=MODE_RTU):
        self.serial = uartObj
        self.address = slaveaddress
        self.mode = mode
        self.debug = False
        self.precalculate_read_size = True      

    def __repr__(self):
        return "{}<id=0x{:x}, address={}, mode={}, serial={}>".format(
            self.__module__,
            id(self),
            self.address,
            self.mode,
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
        return self._genericCommand(functioncode, registeraddress, numberOfDecimals=numberOfDecimals, signed=signed)

    def write_register(self, registeraddress, value, numberOfDecimals=0, functioncode=16, signed=False):
        _checkFunctioncode(functioncode, [6, 16])
        _checkInt(numberOfDecimals, minvalue=0, maxvalue=10, description='number of decimals')
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
        MAX_NUMBER_OF_REGISTERS = 255

        # Payload format constants, so datatypes can be told apart.
        PAYLOADFORMAT_LONG      = 'long'
        PAYLOADFORMAT_FLOAT     = 'float'
        PAYLOADFORMAT_STRING    = 'string'
        PAYLOADFORMAT_REGISTER  = 'register'
        PAYLOADFORMAT_REGISTERS = 'registers'

        ## Check input values ##
        _checkRegisteraddress(registeraddress)
        _checkInt(numberOfDecimals, minvalue=0, description='number of decimals')
        _checkInt(numberOfRegisters, minvalue=1, maxvalue=MAX_NUMBER_OF_REGISTERS, description='number of registers')

        ## Check combinations of input parameters ##
        numberOfRegisterBytes = numberOfRegisters * _NUMBER_OF_BYTES_PER_REGISTER

        # Payload format
        if functioncode in [3, 4, 6, 16] and payloadformat is None:
            payloadformat = PAYLOADFORMAT_REGISTER
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
        payloadFromSlave,retFnc = self._performCommand(functioncode, payloadToSlave)

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

            #if payloadformat == PAYLOADFORMAT_STRING:
            #    return _bytestringToTextstring(registerdata, numberOfRegisters)
            #elif payloadformat == PAYLOADFORMAT_LONG:
            #    return _bytestringToLong(registerdata, signed, numberOfRegisters)
            #elif payloadformat == PAYLOADFORMAT_FLOAT:
            #    return _bytestringToFloat(registerdata, numberOfRegisters)

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
        response = self._communicate(request, number_of_bytes_to_read)

        # Extract payload
        payloadFromSlave = _extractPayload(response, self.address, self.mode, functioncode)
        return payloadFromSlave


    def _communicate(self, request, number_of_bytes_to_read):
        _checkInt(number_of_bytes_to_read)

        # Sleep to make sure 3.5 character times have passed
        minimum_silent_period   = _calculate_minimum_silent_period(self.serial.get_baudrate())
        pyb.delay(minimum_silent_period)

        # Write request
        self.serial.write(request)


        # Read response
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

        if len(answer) == 0:
            raise Exception('No communication with the instrument (no answer)')

        return answer


class Slave:
    def __init__(self,uartObj, slaveaddress, mappedRegisters,mode=MODE_RTU):
        self.serial = uartObj
        self.address = slaveaddress
        self.mode = mode
        self.mappedRegisters = mappedRegisters

    def receive(self):
        if self.serial.any():
            answer = self.serial.readall()
            try:
                payloadFromMaster,receivedFunctioncode = _extractPayload(answer, self.address, self.mode, None)
                self.__analysePacket(payloadFromMaster,receivedFunctioncode)
                return True
            except Exception as err:
                pass
        return False

    def __analysePacket(self,payloadFromMaster,receivedFunctioncode):
        flagUnsupFnc = True
        regAddr = payloadFromMaster[0]<<8 | payloadFromMaster[1]
        out = bytearray()
        if receivedFunctioncode == 3 or receivedFunctioncode == 4: # read holding/input reg
            flagUnsupFnc = False
            qty = payloadFromMaster[2]<<8 | payloadFromMaster[3]
            out.append(qty*2)
            for addr in range(regAddr,regAddr+qty):
                val = self.__getRegisterValue(addr)
                out.append(val>>8)
                out.append(val&0xFF)
        if receivedFunctioncode == 6: # write single reg
            flagUnsupFnc = False
            val = payloadFromMaster[2]<<8 | payloadFromMaster[3]
            self.__setRegisterValue(regAddr,val)
            out.append((regAddr>>8)&0xFF)
            out.append(regAddr&0xFF)
            out.append((val>>8)&0xFF)
            out.append(val&0xFF)

        if receivedFunctioncode == 16: # write multiple registers
            flagUnsupFnc = False
            qty = payloadFromMaster[2]<<8 | payloadFromMaster[3]
            i = 5
            for addr in range(regAddr,regAddr+qty):
                val = payloadFromMaster[i]<<8 | payloadFromMaster[i+1]
                self.__setRegisterValue(addr,val)
                i+=2
            out.append(payloadFromMaster[0])
            out.append(payloadFromMaster[1])
            out.append(payloadFromMaster[2])
            out.append(payloadFromMaster[3])

        if flagUnsupFnc:
            receivedFunctioncode+=0x80
            receivedFunctioncode=receivedFunctioncode&0xFF
            out.append(0x01) #err code

        data = _embedPayload(self.address, self.mode, receivedFunctioncode, out)
        self.serial.write(data)


    def __getRegisterValue(self,addr):
        if addr in self.mappedRegisters:
            return self.mappedRegisters[addr]
        return 0

    def __setRegisterValue(self,addr,val):
        if addr in self.mappedRegisters:
            self.mappedRegisters[addr] = val
   





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



def _checkInt(inputvalue, minvalue=None, maxvalue=None, description='inputvalue'):
    flagError=False
    if minvalue!=None and inputvalue<minvalue:
        flagError=True
    if maxvalue!=None and inputvalue>maxvalue:
        flagError=True
    if flagError:
        raise TypeError('{0} out of range. Given: {1!r}'.format(description, inputvalue))



def _checkFunctioncode(functioncode, listOfAllowedValues=[]):
    if functioncode < 1 or functioncode > 127:
        raise ValueError('Wrong function code: {0}'.format(functioncode))
    if listOfAllowedValues is None:
        return
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


def _hexDecodeNibble(hexStrByte):
    if hexStrByte >=0x30 and hexStrByte <=0x39 :
        return hexStrByte - 0x30
    return hexStrByte - 0x41 + 0x0A

def _hexdecode(hexstring):

    if len(hexstring) % 2 != 0:
        raise ValueError('The input hexstring must be of even length. Given: {!r}'.format(hexstring))

    index = 0
    out = bytearray()
    while len(hexstring) > index:
        out.append( _hexDecodeNibble(hexstring[index])<<4 | _hexDecodeNibble(hexstring[index+1]) )
        index+=2
    return out

def _calculate_minimum_silent_period(baudrate):
    _checkInt(baudrate, minvalue=1, description='baudrate')  # Avoid division by zero
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
    if functioncode!=None:
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

    flagCrcError = False
    if mode == MODE_ASCII:
        if (receivedChecksum[0] != calculatedChecksum[0]&0xFF):
            flagCrcError = True
    else:
        if (receivedChecksum[0] != calculatedChecksum&0xFF) or (receivedChecksum[1] != (calculatedChecksum>>8)&0xFF):
            flagCrcError = True

    if flagCrcError:
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
    if functioncode!=None:
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
    return payload,receivedFunctioncode

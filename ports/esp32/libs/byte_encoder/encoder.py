'''module for converting objects into byte arrays'''

# from typing import Optional, Tuple, Type, Union
import struct
# from crypto.Cipher import AES
# from crypto import Random
# import secrets
# from ucryptolib import aes as AES
import libs.maes as AES

ByteOffset = int


class TemplateType:
    '''Base template type'''

    def convert_to_bytes(self,
                         data,
                         config: 'TemplateBase.Config') -> bytes:
        '''Convert data to bytes'''
        raise NotImplementedError

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int,
                           config: 'TemplateBase.Config') -> tuple[any, ByteOffset]:
        '''Convert encoded bytes to data'''
        raise NotImplementedError


class TemplateInt(TemplateType):
    '''Integer Type'''

    byte_width: int

    def __init__(self, byte_width: int) -> None:
        self.byte_width = byte_width

    def convert_to_bytes(self, data: int, config: 'TemplateBase.Config') -> bytes:
        return data.to_bytes(self.byte_width, config.ENDIAN)

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int, config: 'TemplateBase.Config') -> tuple[int, ByteOffset]:
        return int.from_bytes(
            encoded_data[index: index + self.byte_width],
            config.ENDIAN, False), self.byte_width

class TemplateFloat64(TemplateType):
    '''Float 64 Type'''


    def convert_to_bytes(self, data: float, config: 'TemplateBase.Config') -> bytes:
        ba = bytearray(struct.pack("f", data))
        if len(ba) != 8:
            raise Exception
        return ba

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int, config: 'TemplateBase.Config') -> tuple[int, ByteOffset]:
        return struct.unpack('f', encoded_data)


class TemplateString(TemplateType):
    '''String Type of fixed length'''

    byte_width: int

    def __init__(self, byte_width: int) -> None:
        self.byte_width = byte_width

    def convert_to_bytes(self, data: str, config: 'TemplateBase.Config') -> bytes:
        '''Convert data to bytes'''
        return data.encode(config.STR_ENCODING)[:self.byte_width]

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int, config: 'TemplateBase.Config') -> tuple[str, ByteOffset]:
        return encoded_data[index: index + self.byte_width].decode(
            config.STR_ENCODING), self.byte_width

class PrefixLengthString(TemplateType):
    '''String with the first two bytes encoding the length of the string'''

    def convert_to_bytes(self, data: str, config: 'TemplateBase.Config') -> bytes:
        '''Convert data to bytes'''
        encoded_data = data.encode(config.STR_ENCODING)
        size = len(encoded_data).to_bytes(2, config.ENDIAN)
        return size + encoded_data

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int, config: 'TemplateBase.Config') -> tuple[str, ByteOffset]:
        size = int.from_bytes(
            encoded_data[: 2],
            config.ENDIAN, False)
        return encoded_data[2: size].decode(
            config.STR_ENCODING), (2 + size)


class TemplateNullTerminatedString(TemplateType):
    '''Null terminated string type'''

    def convert_to_bytes(self, data: str, config: 'TemplateBase.Config') -> bytes:
        '''Convert data to bytes'''
        return bytearray(data.encode(config.STR_ENCODING)) + b'\x00'

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int, config: 'TemplateBase.Config') -> tuple[str, ByteOffset]:
        byte_width = 0
        while int(encoded_data[index + byte_width]) != 0:
            byte_width += 1
        return encoded_data[index: index + byte_width].decode(config.STR_ENCODING), byte_width


class TemplateBool(TemplateType):
    '''Boolean type'''

    def convert_to_bytes(self, data: bool, config: 'TemplateBase.Config') -> bytes:
        return int(1 if data else 0).to_bytes(1, 'little')

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int, config: 'TemplateBase.Config') -> tuple[bool, ByteOffset]:
        return int.from_bytes(encoded_data[index:index+1], config.ENDIAN, False) > 0, 1


class TemplateVarList(TemplateType):
    '''Template list of variable length'''

    list_type : list

    def __init__(self, list_type) -> None:
        self.list_type = list_type

        if not isinstance(list_type, TemplateType):
            self.encoder = Encoder(list_type)

    def convert_to_bytes(self,
                         data,
                         config: 'TemplateBase.Config') -> bytes:
        encoded_bytes = bytearray()
        encoded_bytes.extend(len(data).to_bytes(
            2, config.ENDIAN))
        if not isinstance(self.list_type, TemplateType):
            for sub_obj in data:
                encoded_bytes.extend(
                    self.encoder.encode_bytes_no_header(sub_obj))
        else:
            for sub_obj in data:
                encoded_bytes.extend(
                    self.list_type.convert_to_bytes(sub_obj, config))
        return encoded_bytes

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int,
                           config: 'TemplateBase.Config'
                           ):
        decoded_list = []
        list_length = int.from_bytes(
            encoded_data[index:index+2],
            config.ENDIAN,
            signed=False,
        )
        start_index = index
        index += 2
        if not isinstance(self.list_type, TemplateType):
            for _ in range(list_length):
                sub_obj, offset = self.encoder.decode_bytes_no_header(
                    encoded_data, index)
                index += offset
                decoded_list.append(sub_obj)
        else:
            for _ in range(list_length):
                sub_obj, offset = self.list_type.convert_from_bytes(
                    encoded_data, index, config)
                index += offset
                decoded_list.append(sub_obj)
        return decoded_list, index - start_index


class TemplateFixedList(TemplateType):
    '''Template for a fixed length list'''

    # list_len: int
    # list_type: Union['TemplateType', 'TemplateBase']

    def __init__(self, list_len: int, list_type) -> None:
        self.list_type = list_type
        self.list_len = list_len
        if not isinstance(list_type, TemplateType):
            self.encoder = Encoder(list_type)

    def convert_to_bytes(self,
                         data,
                         config: 'TemplateBase.Config') -> bytes:
        encoded_bytes = bytearray()
        if len(data) != self.list_len:
            raise Exception
        if not isinstance(self.list_type, TemplateType):
            for sub_obj in data:
                encoded_bytes.extend(
                    self.encoder.encode_bytes_no_header(sub_obj))
        else:
            for sub_obj in data:
                encoded_bytes.extend(
                    self.list_type.convert_to_bytes(sub_obj, config))
        return encoded_bytes

    def convert_from_bytes(self,
                           encoded_data: bytes,
                           index: int,
                           config: 'TemplateBase.Config'
                           ):
        decoded_list = []
        start_index = index
        if not isinstance(self.list_type, TemplateType):
            for _ in range(self.list_len):
                sub_obj, offset = self.encoder.decode_bytes_no_header(
                    encoded_data, index)
                index += offset
                decoded_list.append(sub_obj)
        else:
            for _ in range(self.list_len):
                sub_obj, offset = self.list_type.convert_from_bytes(
                    encoded_data, index, config)
                index += offset
                decoded_list.append(sub_obj)
        return decoded_list, start_index - index


class TemplateBase:
    '''Base Class for an encoding template
    Members should inherit from TemplateType
    '''

    class Config:
        '''Config for encoding and decoding'''
        # HEADER SUM ID IV MESSAGE
        USE_HEADER_BYTE: bool = True
        HEADER_BYTE_VALUE: int = 0
        USE_SUM: bool = True
        USE_ID: bool = False
        ID_LENGTH: int = 4
        USE_ENCRYPTION: bool = False
        IV_SIZE_BYTES: int = 16
        ENDIAN = 'little'
        STR_ENCODING = 'utf-8'


class Encoder:

    '''Class for encoding objects bytes and converting bytes to objects'''

    def __init__(self, template) -> None:
        self.template = template
        self.members = [(attr, self.template.__dict__[attr])
                        for attr in self.template.__dict__.keys()
                        if not callable(getattr(self.template, attr)) and not attr.startswith("__")]

    def _decrypt(self, byte_array: bytes, encryption_key: bytes) -> bytes:
        '''Decrypt the message body given an aes key'''
        index = 0 if not self.template.Config.USE_HEADER_BYTE else 1
        index = index if not self.template.Config.USE_SUM else (index + 1)
        index = index if not self.template.Config.USE_ID else (
            index + self.template.Config.ID_LENGTH)
        header = byte_array[:index]
        iv = byte_array[index:index + self.template.Config.IV_SIZE_BYTES]
        index += self.template.Config.IV_SIZE_BYTES
        cipher = AES.new(encryption_key, AES.MODE_CBC, IV=iv)
        message = cipher.decrypt(byte_array[index:])
        return header + message

    def encode_bytes_no_header(self, obj: TemplateBase) -> bytearray:
        '''Convert an object to a bytearray without a header'''
        encoded_bytes = bytearray()
        member_value: TemplateType
        for member_name, member_value in self.members:
            encoded_bytes.extend(
                member_value.convert_to_bytes(obj.__dict__[member_name], self.template.Config))
        return encoded_bytes

    def encode_bytes(
            self, obj: TemplateBase,
            obj_id = None, encryption_key = None) -> bytes:
        '''Convert an object to bytes with a header and optionally encryption'''
        encoded_bytes = self.encode_bytes_no_header(obj)
        if self.template.Config.USE_ENCRYPTION:
            if encryption_key is None:
                raise Exception
            aes_iv = b'\xf9\xca\x17\xe9\x8b\x81\x164?+k1\xcdy\x8d\x83'
            # aes_iv = Random.new().read(self.template.Config.IV_SIZE_BYTES)
            print('necrypting')
            print(type(encryption_key))
            cipher = AES.new(encryption_key, AES.MODE_CBC, IV=aes_iv)
            encoded_bytes = bytearray(encoded_bytes)
            while len(encoded_bytes) % 16 != 0:
                encoded_bytes.append(bytes([0])[0])
            encoded_bytes = cipher.encrypt(encoded_bytes)
            encoded_bytes = aes_iv + encoded_bytes
            encoded_bytes = bytearray(encoded_bytes)
        if self.template.Config.USE_SUM:
            encoded_bytes = bytearray(
                [sum([int(x) for x in encoded_bytes]) & 255]) + encoded_bytes
        if self.template.Config.USE_ID:
            encoded_bytes = obj_id.to_bytes(
                self.template.Config.ID_LENGTH, self.template.Config.ENDIAN) + encoded_bytes
        if self.template.Config.USE_HEADER_BYTE:
            encoded_bytes = bytearray(encoded_bytes)
            encoded_bytes[0:0] = self.template.Config.HEADER_BYTE_VALUE.to_bytes(
                1, self.template.Config.ENDIAN)
        return bytes(encoded_bytes)

    def decode_bytes_no_header(self,
                               byte_array: bytes,
                               start_index: int):
        '''Convert a bytearray without a header into an object'''
        decoded_obj = self.template()
        index = start_index
        print(self.members)
        for member_name, member_value in self.members:
            obj, offset = member_value.convert_from_bytes(
                byte_array, index, self.template.Config)
            index += offset
            print(self.template.__dict__)
            print(decoded_obj.__dict__)
            print(member_name)
            decoded_obj.__dict__[member_name] = obj
            print('set')

        return decoded_obj, index - start_index

    def decode_bytes(self,
                     byte_array: bytes, encryption_key = None) -> TemplateBase:
        '''Convert a decrypted byte array into '''
        if encryption_key is not None:
            byte_array = self._decrypt(byte_array, encryption_key)
        index = 0
        if self.template.Config.USE_HEADER_BYTE:
            index += 1
        if self.template.Config.USE_SUM:
            index += 1
        if self.template.Config.USE_ID:
            index += self.template.Config.ID_LENGTH
        obj, _ = (self.decode_bytes_no_header(byte_array, index))
        return obj

    def validate_header(self, byte_array: bytes) -> bool:
        '''Ensure the the checks in the header are met such as the sum'''
        count = 0
        ints = [int(x) for x in byte_array]
        if self.template.Config.USE_HEADER_BYTE:
            if byte_array[count] != self.template.Config.HEADER_BYTE_VALUE:
                return False
            count += 1
        if self.template.Config.USE_SUM:
            message_sum = ints[1]
            actual_sum = sum(ints[2:]) & 255
            if actual_sum != message_sum:
                return False
        return True

    def get_id(self, byte_array: bytes) -> int:
        '''Get the object ID from the encoding'''
        index = 0 if not self.template.Config.USE_HEADER_BYTE else 1
        index = index if not self.template.Config.USE_SUM else (index + 1)
        return int.from_bytes(
            byte_array[index:index + self.template.Config.ID_LENGTH],
            self.template.Config.ENDIAN,False)



if __name__ == '__main__':
    class Test(TemplateBase):
        x: int = TemplateInt(2)
        y: int = TemplateBool()

    test = Test()
    test.x = 253
    test.y = True

    encoder = Encoder(Test)
    out = encoder.encode_bytes(test)
    print(list(out))
    response = encoder.decode_bytes(out)
    # assert response.x == test.x
    # assert response.y == test.y

    class Test2(TemplateBase):
        class Config(TemplateBase.Config):
            USE_ENCRYPTION: bool = True
        x = TemplateVarList(TemplateInt(5))
        y = TemplateInt(4)

    key = b'\x12!\xfbLT\xf6\xd1YY}\xc9\xd4i\xdb\xb9\x92'

    test2 = Test2()
    test2.x = [2345, 234, 4363, 32]
    test2.y = 234

    encoder = Encoder(Test2)
    out = encoder.encode_bytes(test2, encryption_key=key)
    response = encoder.decode_bytes(out, encryption_key=key)
    assert len(test2.x) == len(response.x)
    for x, y in zip(test2.x, response.x):
        assert x == y
    assert response.y == test2.y

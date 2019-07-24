import binascii
import ntpath
import os
import struct

def validateFilePath(filePath):
    """
    raise exception if the given file path does not exist.

    :param filePath:
    :return:
    """
    if not os.path.isfile(filePath):
        raise Exception("Error! Can't find given file : " + filePath)


def parseConfigFile(configFilePath):
    """
    :param configFilePath:
    :return: a hash map of the parameters defined in the given file.
    Each entry is organized as <parameter name, parameter value>
    """

    # parse valid lines
    lines = []
    with open(configFilePath) as f:
        for line in f:
            line = line.strip()
            if line == '':  # ignore empty line
                continue
            if line.startswith('#'):  # ignore the comment in config file
                continue

            lines.append(line)

    params = {}
    for line in lines:
        if not line.__contains__("="):
            raise Exception("Invalid parameter definition as \"" + line + "\" in file " + configFilePath)

        paramName = line.split("=")[0].strip()
        value = line.split("=")[1].strip()
        params[paramName] = value

    return params


def format32BitHexStr(hexStr):
    """
    format the given string which represents a valid 32-bit hexadecimal number.
    prefix "0x" will be added and will replace any valid prefix.
    alphabetic letter will be formatted into upper case.
    "0" will be used to fill the hexadecimal number if this number is represented as less than 8-letter.

    Exmaple usage:
    input: 0Xff  -> output:0x000000FF
    input: Ab -> output: 0x000000AB
    input 0xAf -> output: 0x000000AF


    :param hexStr:  a valid string representing a 32-bit hexadecimal number
    :return: a formatted string representing 32-bit hexadecimal number as described
    """
    # remove "0x" or "OX"  prefix if it had any
    hexStr = hexStr.replace("0x", "").replace("0X", "")

    hexStr = hexStr[0:8].zfill(8)

    hexStr = hexStr.upper()

    hexStr = "0x" + hexStr

    return hexStr


def getFileSize(filePath):
    """
    :param filePath: path of a file
    :return:  the size of the given file in bytes
    """
    return os.path.getsize(filePath)


def toLitteEndianByte(hexStr):
    """
    :param hexStr: a string representing a valid 32-bit hexadecimal number
    :return:  byte value of this hexadecimal number in little endian format
    """
    # remove "0x" or "OX"  prefix if there's any
    hexStr = hexStr.replace("0x", "").replace("0X", "")

    # convert it into bytes
    byteData = binascii.unhexlify(hexStr)

    # convert to unsigned int
    x = struct.unpack('>I', byteData)[0]

    # pack it into little endian format
    return struct.pack('<I', x)


def printHeaderFromLittleEndian(imagePath, numLines):
    """
    print the first numLines of given image in big endian order from little endian order

    :param imagePath:
    :param numLines:
    :return:
    """
    print("First ", numLines, " lines (", numLines * 4, " bytes )  : ")
    f = open(imagePath, "rb")
    time = numLines
    try:
        while time > 0:
            byte = f.read(4)  # 32 bit machine uses 4 bytes
            time -= 1
            print(format32BitHexStr(hex(struct.unpack('<I', byte)[0])))

    finally:
        f.close()


def extractFileName(path):
    folder, filename = ntpath.split(path)

    return filename


def extractFolderPath(path):
    folder, filename = ntpath.split(path)

    return folder




import argparse
import os
import sys
import struct
from collections import namedtuple

from util import validateFilePath \
    , parseConfigFile \
    , format32BitHexStr \
    , getFileSize \
    , toLitteEndianByte

OTADescriptor = namedtuple("OTADescriptor"
                           , ["sequenceNumber", "startAddress", "endAddress", "executionAddress", "hardwareID",
                              "reserves"])


def printOTADescriptorImageStruct(processedImagePath, imageLines, offset):
    """
    output the OTA descriptor of processed image along with some of its contents
    for additionalLines

    :param processedImagePath:
    :param imageLines:  number of lines to output when printing the image content
    :param offset: number of bytes to skip from the beginning
    :return:
    """
    cuttingLine = "--------"

    f = open(processedImagePath, "rb")
    f.seek(offset)
    try:
        print(cuttingLine + "Generated OTA descriptor" + cuttingLine)
        byte = f.read(4)
        print(format32BitHexStr(hex(struct.unpack('<I', byte)[0])), " -> sequence number")

        byte = f.read(4)
        print(format32BitHexStr(hex(struct.unpack('<I', byte)[0])), " -> start address")
        byte = f.read(4)
        print(format32BitHexStr(hex(struct.unpack('<I', byte)[0])), " -> end address")

        byte = f.read(4)
        print(format32BitHexStr(hex(struct.unpack('<I', byte)[0])), " -> execution address")

        byte = f.read(4)
        print(format32BitHexStr(hex(struct.unpack('<I', byte)[0])), " -> hardware ID")

        byte = f.read(4)
        print(format32BitHexStr(hex(struct.unpack('<I', byte)[0])), " -> reserved bytes")

        print(cuttingLine + "Image Content" + cuttingLine)
        time = imageLines
        while time > 0:
            byte = f.read(4)  # Every time print 32 bits
            time -= 1
            print(format32BitHexStr(hex(struct.unpack('<I', byte)[0])))
        print("...")
        print("...")
    finally:
        f.close()


def parseParamFromCMD():
    """
    parse input image path and hardware platform name from command line.

    raise exception if these parameters are not provided by user.

    :return: input image path and hardware platform name
    """

    progName = sys.argv[0]

    format = "python " + progName + " [-h] -b binary_path -p hardware_platform"

    example1 = "\t get help: \n" + "\t\tpython " + progName + " -h"

    example2 = "\t use folder/inputImage.bin and MCHP-Curiosity-PIC32MZEF as parameter : \n" \
               + "\t\tpython " + progName + " -b folder/inputImage.bin -p MCHP-Curiosity-PIC32MZEF "

    usageMsg = format + "\n\n" + "example usages:" + "\n" + example1 + "\n" + example2

    parser = argparse.ArgumentParser(usage=usageMsg)

    parser.add_argument('-b', required=True, help=" path to input binary image ")
    parser.add_argument('-p', required=True, help=" hardware platform name ")

    args = vars(parser.parse_args())

    inputImagePath = args["b"]

    hardwarePlatform = args["p"]

    return inputImagePath, hardwarePlatform


def validate32BitHexParam(hex32Str, paramName, fileLocation):
    """
    validate whether given string is valid to represent a 32-bit hexadecimal number( Expected range [0x0,0xFFFFFFFF])

    raise exception if the given string contains invalid character such as "h", or the hex string defines a value is greater than 0xFFFFFFFF

    :param hex32Str: a string used to representing a 32-bit hexadecimal number
    :param paramName: name of the parameter
    :param fileLocation: file location where the hex parameter is defined
    :return:
    """
    # check whether the definition is valid
    try:
        int(hex32Str, 16)
    except Exception:
        raise Exception(
            "Invalid hexadecimal number definition for \"" + paramName + "\" as \"" + hex32Str + "\".  File location: " + fileLocation)

    # we allow hex parameter with prefix "0x" or "0X"
    formatedParam = hex32Str.replace("0x", "")
    formatedParam = formatedParam.replace("0X", "")

    # check whether the value is in [0x0,0xFFFFFFFF]
    invalidValueMsg = "Invalid hexadecimal number value for \"" + paramName + "\" as \"" + hex32Str + "\".  Value cannot exceed 0xFFFFFFFF" + ".  File location: " + fileLocation
    if len(formatedParam) > 8:
        raise Exception(invalidValueMsg)


def formatHardwareID(hardwareIdStr, fileLocation):
    """
    format the hardware id into hexadecimal representation
    
    raise exception if the given  hardwareIdStr is not valid.

    :param hardwareIdStr: a string representing hardware id
    :param fileLocation: location where the hardware Id is defined
    :return:
        formatted hexadecimal string of hardware id

    exmaples:

        hardwarID = "0.0.0"
        return "0x00000000"

        hardwarID = "255.255.65535"
        return "0xFFFFFFFF"

        hardwarID = "171.205.61355"
        return "0xABCDEFAB"

        hardwarID ="1.2.772"
        return "0x01020304"

        hardwarID = "000001.000000000002.00000000000772"
        return "0x01020304"

        hardwarID = "1.1.1"
        return "0x01010001"

        hardwarID = "1.1.00000000"
        return "0x01010000"

        hardwarID = "0.0.1"
        return "0x00000001"
    """
    formatErrorMsg = "Invalid Hardware ID format. Expected format: \"a.b.c\" ,where a,b is 8-bit unsigned integer with range [0,255], c is 16-bit unsigned integer with range range[0,65535]" \
                     + "\nFound: \"" + hardwareIdStr + "\"." \
                     + "\nFile location: " + fileLocation
    fields = hardwareIdStr.split(".")
    if len(fields) != 3:
        raise Exception(formatErrorMsg)

    a, b, c = fields

    try:
        a = int(a, 10)
        b = int(b, 10)
        c = int(c, 10)
    except Exception:
        raise Exception(formatErrorMsg)

    rangeErrorMsg = "Value of hardware ID is not within expected range. \n" \
                    + "Expected range for \"a.b.c\" is a->[0,255], b->[0,255], c->[0,65535]. Found: " + hardwareIdStr + "\n" \
                    + "File location: " + fileLocation
    if a < 0 or a > 255 or b < 0 or b > 255 or c < 0 or c > 65535:
        raise Exception(rangeErrorMsg)

    # convert hardware ID into hex representation
    # remove prefix if there's any
    a = hex(a).replace("0x", "").replace("0X", "")
    b = hex(b).replace("0x", "").replace("0X", "")
    c = hex(c).replace("0x", "").replace("0X", "")

    # fill zeroes to get the desired fixed length since we are going to concatenate those hex strings
    a = a.zfill(2)
    b = b.zfill(2)
    c = c.zfill(4)

    hexStr = a + b + c

    return format32BitHexStr(hexStr)


def validate32BitUIntParam(uInt32Str, paramName, fileLocation):
    """
    validate whether given string is valid to represent a 32-bit unsigned integer. ( Expected range [0,4294967295] )
    raise exception if the given string contains invalid character such as "a", or the string defines a value which is not in [0,4294967295]

    :param uInt32Str: a string used to
    :param paramName: parameter name of uInt32Str
    :param fileLocation:
    :return:
    """
    try:
        int(uInt32Str, 10)
    except Exception:
        raise Exception(
            "Invalid 32 bit unsigned integer definition for \"" + paramName + "\" as \"" + uInt32Str + "\".  File location: " + fileLocation)

    invalidValueMsg = "Invalid 32 bit unsigned integer definition for \"" + paramName + "\". Expected range: [0, 4294967295]. Value found : \"" + uInt32Str + "\".   File location: " + fileLocation
    if int(uInt32Str, 10) < 0 or int(uInt32Str, 10) > 4294967295:
        raise Exception(invalidValueMsg)


def validate32BitHexParamRange(val, paramName, minVal, maxVal, fileLocation):
    """
    raise exception the condition  minVal <= val <= maxVal is not met

    :param val: a string representing a valid 32-bit hexadecimal parameter
    :param paramName: name of the parameter
    :param minVal: a string representing a valid 32-bit hexadecimal lower bound of the parameter
    :param maxVal: a string representing a valid 32-bit hexadecimal lower bound of the parameter
    :param fileLocation: the path where the parameter is defined
    :return:
    """

    if int(val, 16) < int(minVal, 16) or int(val, 16) > int(maxVal, 16):
        raise Exception(
            "Invalid value of \"" + paramName + "\"! Expected range : [" + minVal + "," + maxVal + "]. Found : " + val + ".  File location: " + fileLocation)


def getEndAddress(fileSize, startAddress):
    """
    :param fileSize: size of the FreeRTOS image used to add OTA descriptor in bytes
    :param startAddress: start address in decimal integer
    :return: end address in decimal integer
    """

    endAddress = startAddress  # initialize it as start address
    endAddress += fileSize  # include the file size
    endAddress += 24  # include size of the OTA descriptor, currently we have 24 bytes
    return endAddress


def getOTADescriptor(userConfigFilePath, inputImagePath, ruleFolderPath, hardwarePlatform):
    """
    parse and validate the parameters defined by user.
    calculate the end address

    If all parameters are valid, return them as ota descriptor.

    Otherwise, raise exeception.

    :param userConfigFilePath: path of the user config file
    :param inputImagePath: path of the input image, which will be used to calculate the end address
    :param ruleFolderPath: path of the folder which contains the validation rule file for the user config
    :param hardwarePlatform: hardware platform name
    :return: parameters to be used to generate the OTA descriptor.
             Each parameter will be formatted as a 10-character string to represent a 32-bit hexadecimal number,
             where the first two character is a prefix "0x".
    """

    parsedParams = parseConfigFile(userConfigFilePath)
    # 1. validate the hardware platform is valid and corresponding rule file can be found
    validHardwarePlatforms = os.listdir(ruleFolderPath)
    if hardwarePlatform not in validHardwarePlatforms:
        raise Exception(
            "Invalid hardware platform! \nExpected hardware platforms: " + str(
                validHardwarePlatforms) + ". \nFound: " + hardwarePlatform)

    # 2. Make sure the rule file and the parameters defined in it are valid
    validationFileLocation = os.path.join(ruleFolderPath, hardwarePlatform)
    validateFilePath(validationFileLocation)
    ruleParams = parseConfigFile(validationFileLocation)
    if len(ruleParams.keys()) != 2:
        raise Exception("Invalid validation rule file : " + validationFileLocation +
                        "\nExpected 2 parameters in this file. " + "\nFound: " + str(len(ruleParams.keys())))

    if "MIN_ADDRESS" not in ruleParams:
        raise Exception("Error! parameter \"MIN_ADDRESS\" is not defined in " + validationFileLocation)
    minAddrHardwarePlatform = ruleParams["MIN_ADDRESS"]
    validate32BitHexParam(minAddrHardwarePlatform, "MIN_ADDRESS", validationFileLocation)
    minAddrHardwarePlatform = format32BitHexStr(minAddrHardwarePlatform)

    if "MAX_ADDRESS" not in ruleParams:
        raise Exception("Error! parameter \"MAX_ADDRESS\" is not defined in " + validationFileLocation)
    maxAddrHardwarePlatform = ruleParams["MAX_ADDRESS"]
    validate32BitHexParam(maxAddrHardwarePlatform, "MAX_ADDRESS", validationFileLocation)
    maxAddrHardwarePlatform = format32BitHexStr(maxAddrHardwarePlatform)

    if (int(maxAddrHardwarePlatform, 16) <= int(minAddrHardwarePlatform, 16)):
        raise Exception(
            "MAX_ADDRESS must be greater than the MIN_ADDRESS !" + " File location " + validationFileLocation)

    # 3. validate sequence number
    if "SEQUENCE_NUMBER" not in parsedParams:
        raise Exception("Error! parameter \"SEQUENCE_NUMBER\" is not defined in " + userConfigFilePath)
    sequenceNumber = parsedParams["SEQUENCE_NUMBER"]
    validate32BitUIntParam(sequenceNumber, "SEQUENCE_NUMBER", userConfigFilePath)
    # convert from string of 32-bit unsigned integer to string of 32-bit hexadecimal
    sequenceNumber = hex(int(sequenceNumber, 10))
    sequenceNumber = format32BitHexStr(sequenceNumber)

    # 4. validate hardware id
    if "HARDWARE_ID" not in parsedParams:
        raise Exception("Error! parameter \"HARDWARE_ID\" is not defined in " + userConfigFilePath)
    hardwareID = parsedParams["HARDWARE_ID"]

    hardwareID = formatHardwareID(hardwareID, userConfigFilePath)
    hardwareID = format32BitHexStr(hardwareID)

    # 5. validate reserved bytes
    if "RESERVED_BYTES" not in parsedParams:
        raise Exception("Error! parameter \"RESERVED_BYTES\" is not defined in " + userConfigFilePath)
    reserves = parsedParams["RESERVED_BYTES"]
    validate32BitHexParam(reserves, "RESERVED_BYTES", userConfigFilePath)
    reserves = format32BitHexStr(reserves)

    # 6. validate start address
    if "START_ADDRESS" not in parsedParams:
        raise Exception("Error! parameter \"START_ADDRESS\" is not defined in " + userConfigFilePath)
    startAddress = parsedParams["START_ADDRESS"]
    validate32BitHexParam(startAddress, "START_ADDRESS", userConfigFilePath)
    startAddress = format32BitHexStr(startAddress)
    # make sure minAddrHardwarePlatform <= startAddress < maxAddressHardwarePlatform
    startMin = minAddrHardwarePlatform
    startMax = format32BitHexStr(hex(int(maxAddrHardwarePlatform, 16) - 1))
    validate32BitHexParamRange(startAddress, "START_ADDRESS", startMin, startMax, userConfigFilePath)

    # 7. calculate and validate the end address
    fileSize = getFileSize(inputImagePath)
    endAddress = getEndAddress(fileSize, int(startAddress, 16))  # get end address in decimal format
    endAddress = hex(endAddress).upper()  # convert to hexadecimal format

    # make sure minAddrHardwarePlatform < endAddress <= maxAddressHardwarePlatform
    endMin = format32BitHexStr(hex(int(minAddrHardwarePlatform, 16) + 1))
    endMax = maxAddrHardwarePlatform
    if int(endAddress, 16) < int(endMin, 16) or int(endAddress, 16) > int(endMax, 16):
        raise Exception(
            "Invalid value of \"END_ADDRESS\"! Expected range : [" + endMin + "," + endMax + "]. Calculated Result : " + endAddress
            + "\nPossible reasons: START_ADDRESS is too large or image size is too large (END_ADDRESS = START_ADDRESS + 24 bytes + size of input image)"
            + "\n\nDebug Info: "
            + "\nImage size: " + str(fileSize)
            + "\nSTART_ADDRESS: " + startAddress + ". Config file location" + userConfigFilePath)
    endAddress = format32BitHexStr(endAddress)

    # 8. validate the execution address
    if "EXECUTION_ADDRESS" not in parsedParams:
        raise Exception("Error! parameter \"EXECUTION_ADDRESS\" is not defined in " + userConfigFilePath)
    executionAddress = parsedParams["EXECUTION_ADDRESS"]
    validate32BitHexParam(executionAddress, "EXECUTION_ADDRESS", userConfigFilePath)
    # make sure startAddress <= executionAddress <= endAddress
    execMin = startAddress
    execMax = endAddress
    validate32BitHexParamRange(executionAddress, "EXECUTION_ADDRESS", execMin, execMax, userConfigFilePath)
    executionAddress = format32BitHexStr(executionAddress)

    return OTADescriptor(sequenceNumber, startAddress, endAddress, executionAddress, hardwareID, reserves)


def addOTADescriptorToImage(inputImagePath, otaDescriptor, outputImagePath):
    """
    Add given OTA descriptor into given image

    :param inputImagePath:
    :param otaDescriptor:
    :param outputImagePath:
    :return:
    """

    with open(inputImagePath, "rb") as fIn:
        data = fIn.read()

    with open(outputImagePath, "wb") as fOut:
        # 1. Add sequence number
        sequenceField = toLitteEndianByte(otaDescriptor.sequenceNumber)
        fOut.write(sequenceField)

        # 2. Add start address
        startAddrField = toLitteEndianByte(otaDescriptor.startAddress)
        fOut.write(startAddrField)

        # 3. Add end address
        endAddrField = toLitteEndianByte(otaDescriptor.endAddress)
        fOut.write(endAddrField)

        # 4. Add execution address
        execAddrField = toLitteEndianByte(otaDescriptor.executionAddress)
        fOut.write(execAddrField)

        # 5. Add hardware ID
        HardwareIdField = toLitteEndianByte(otaDescriptor.hardwareID)
        fOut.write(HardwareIdField)

        # 6. Add reserved bytes
        reserveField = toLitteEndianByte(otaDescriptor.reserves)
        fOut.write(reserveField)

        fOut.write(data)


def generateOTADescriptorImage(inputImagePath, hardwarePlatform):
    """

    :param inputImagePath: valid input image path
    :param hardwarePlatform:
    :return:  path of generated ota image, which as OTA descriptor appended
    """
    rootPath = os.path.dirname(__file__)

    userConfigFolder = 'user-config'
    userConfigFileName = 'ota-descriptor.config'
    userConfigFilePath = os.path.join(rootPath, userConfigFolder, userConfigFileName)
    validateFilePath(userConfigFilePath)

    ruleFolder = "config-validation-rules"
    ruleFolderPath = os.path.join(rootPath, ruleFolder)

    otaDescriptor = getOTADescriptor(userConfigFilePath, inputImagePath, ruleFolderPath, hardwarePlatform)

    print("\nAdding OTA descriptors to image " + inputImagePath + " ...")

    if inputImagePath.endswith(".bin"):
        otaImagePath = inputImagePath.replace(".bin", ".ota.bin")
    else:
        otaImagePath = inputImagePath + ".ota.bin"

    addOTADescriptorToImage(inputImagePath, otaDescriptor, otaImagePath)
    print("OTA descriptors added! OTA image generated at : " + otaImagePath)

    return otaImagePath


if __name__ == "__main__":
    inputImagePath, hardwarePlatform = parseParamFromCMD()
    validateFilePath(inputImagePath)
    otaImagePath = generateOTADescriptorImage(inputImagePath, hardwarePlatform)
    print("\nResult of adding OTA descriptor: \n")
    printOTADescriptorImageStruct(processedImagePath=otaImagePath, imageLines=10, offset=0)

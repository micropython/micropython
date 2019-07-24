"""
Amazon FreeRTOS
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
http://aws.amazon.com/freertos
http://www.FreeRTOS.org 

"""
import fileinput
import sys
import subprocess
import os

class OtaAfrProject:
    """OtaAfrProject represents the Amazon FreeRTOS code base for OTA.
    This class is used to update the Amazon FreeRTOS project for OTA.
    Attributes:
        _buildConfig(dict): Build configuration from 'build_config' field in board.json.
        _projectRootDir(str): The root of the Amazon FreeRTOS project i.e. $AFR_ROOT/demos or $AFR_ROOT/tests.
        _boardPortablePath(str): the vendor/board path of the Amazon FreeRTOS project.
    Methods:
        initializeOtaProject()
        buildProject()
        setClientCredentialForThingName(thingName)
        setClientCredentialKeys(certificate, privateKey)
        setApplicationVersion(major, minor, bugfix)
        setCodesignerCertificate(codesignerCertificatePath)
        __setDemoRunnerForOtaDemo()
        setClientCredentialsForWifi()
        setClientCredentialsForAwsIotEndpoint()
        __setIdentifierInFile(prefixToValue, filePath)
    Example:
        from aws_ota_aws_agent import AwsIoTThing
        otaProject = OtaAfrProject('C:/amazon-freertos', buildConfig)
        otaProject.initializeOtaProject()
        iotThing = AWSIoTThing('boardName')
        otaProject.setClientCredentialForThingName(iotThing.thing_name)
        otaProject.setClientCredentialKeys(iotThing.cert, iotThing.priv_key)
        otaProject.buildProject()
    """
    
    DEMO_RUNNER_PATH = 'common/demo_runner/aws_demo_runner.c'
    CLIENT_CREDENTIAL_PATH = 'common/include/aws_clientcredential.h'
    APPLICATION_VERSION_PATH = 'common/include/aws_application_version.h'
    CLIENT_CREDENTIAL_KEYS_PATH ='common/include/aws_clientcredential_keys.h'
    OTA_CODESIGNER_CERTIFICATE_PATH = 'common/include/aws_ota_codesigner_certificate.h'
    OTA_UPDATE_DEMO_PATH = 'demos/common/ota/aws_ota_update_demo.c'
    OTA_BOOTLOADER_CONFIG_PATH = 'demos/common/ota/bootloader/utility/user-config/ota-descriptor.config'
    OTA_BOOTLOADER_CERTIFICATE_PATH = 'demos/common/ota/bootloader/utility/codesigner_cert_utility/aws_ota_codesigner_certificate.pem'
    OTA_FACTORY_IMAGE_GENERATOR_PATH = 'demos/common/ota/bootloader/utility/factory_image_generator.py'

    def __init__(self, projectRootDir, boardPortablePath, buildConfig):
        self._buildConfig = buildConfig
        self._projectRootDir = projectRootDir
        self._boardPortablePath = boardPortablePath
        self._bootloaderSequenceNumber = 0

    def initializeOtaProject(self):
        """Initialize the Amazon FreeRTOS project for OTA.
        """
        base = os.path.basename(self._projectRootDir)
        if base == 'demos':
            self.__setDemoRunnerForOtaDemo()
        elif base == 'tests':
            self.__setTestRunnerForOtaDemo()
        else:
            raise Exception('ERROR: Invalid project root \"{}\". The valid values are \"demos\" and \"tests\".'.format(base))

    def generateFactoryImage(self):
        # If this board uses the Amazon FreeRTOS reference bootlaoder, then we want to 
        # build and flash the factory image.
        if self._buildConfig.get('use_reference_bootloader', False):
            factoryImageGenCommand = \
                'python ' + os.path.join(self._projectRootDir, '..', OtaAfrProject.OTA_FACTORY_IMAGE_GENERATOR_PATH) + ' ' + \
                '-b ' + self._buildConfig['output'] + ' ' + \
                '-p ' + self._buildConfig['bootloader_hardware_platform'] + ' ' + \
                '-k ' + self._buildConfig['bootloader_private_key_path'] + ' ' + \
                '-x ' + self._buildConfig['bootloader_output']
            subprocess.call(
                factoryImageGenCommand,
                shell=True
            )
                

    def buildProject(self):
        """Build the Amazon FreeRTOS project represented by this object.
        """
        # Update the bootloader sequence number for every new build
        self.__incrementBootloaderSequenceNumber()

        # Save the system path to restore.
        system_path = os.environ['PATH']
        # Add the tool_paths to the system PATH
        for path in self._buildConfig['tool_paths']:
            os.environ['PATH'] = path + os.pathsep + os.environ['PATH']

        buildCommands = self._buildConfig['commands']
        print('Building project {}...'.format(self._buildConfig['project_dir']))
        for command in buildCommands:
            command = command.format(**self._buildConfig)
            subprocess.Popen(command + ' >> build_output.txt 2>&1', shell=True).wait()

        output = self._buildConfig['output']
        if not os.path.exists(output):
            print("ERROR: Could not find the output binary, the build might have failed.")
            raise Exception("Error building project check build_output.txt")
        print('Build finished, output: {}'.format(self._buildConfig['output']))

        # We generate the factory image if applicable. This may depend on some build tool paths.
        self.generateFactoryImage()

        # Restore the system path
        os.environ['PATH'] = system_path

    def __incrementBootloaderSequenceNumber(self):
        self._bootloaderSequenceNumber += 1
        self.__setBootloaderSequenceNumber(self._bootloaderSequenceNumber)

    def __setBootloaderSequenceNumber(self, number):
        self.__setIdentifierInFile(
            {
                'SEQUENCE_NUMBER': '= ' + str(number)
            },
            os.path.join(self._projectRootDir, '..', OtaAfrProject.OTA_BOOTLOADER_CONFIG_PATH)
        )

    def copyCodesignerCertificateToBootloader(self, certificate):
        """Copies the input certificate to a file named: aws_ota_codesigner_certificate.pem under 
        demos/common/ota/bootloader/utility/codesigner_cert_utility.
        """
        with open(os.path.join(self._projectRootDir, '..', OtaAfrProject.OTA_BOOTLOADER_CERTIFICATE_PATH), 'w') as f:
            f.write(certificate)            

    def __setDemoRunnerForOtaDemo(self):
        """
        Updates the aws_demo_runner.c to disable the default MQTT Echo demo and enable the OTA 
        update demo.
        """
        demoRunnerFilePath = os.path.join(self._projectRootDir, OtaAfrProject.DEMO_RUNNER_PATH)
        vStartMQTTEchoDemo = "vStartMQTTEchoDemo"
        vStartotaUpdateDemoTask = "vStartOTAUpdateDemoTask"
        for line in fileinput.input(files=demoRunnerFilePath, inplace=True):
            if (vStartMQTTEchoDemo in line) and ("//" not in line) and ("/*" not in line):
                line = "//" + line
            if vStartotaUpdateDemoTask in line:
                line = line.replace("/* ", "")
                line = line.replace(" */", "")
            # print(line) will place an extra newline character in the file.
            sys.stdout.write(line)

    def __setTestRunnerForOtaDemo(self):
        """
        Updates the aws_test_runner_config.h file to enable the OTA demo.
        """
        self.__setIdentifierInFile(
            {
                '#define testrunnerFULL_TCP_ENABLED': '0',
                '#define testrunnerOTA_END_TO_END_ENABLED': '1'
            },
            os.path.join(self._projectRootDir, self._boardPortablePath, 'common', 'config_files', 'aws_test_runner_config.h')
        )
        
    
    def setClientCredentialsForWifi(self, ssid, password, security):
        """
        Updates the aws_clientcredential.h file for the wifi configurations defined in 
        the board.json
        """
        self.__setIdentifierInFile(
            { 
                '#define clientcredentialWIFI_SSID': '\"' + ssid + '\"',
                '#define clientcredentialWIFI_PASSWORD': '\"' + password + '\"',
                '#define clientcredentialWIFI_SECURITY': security
            },
            os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_PATH)
        )

    def setClientCredentialsForAwsIotEndpoint(self, awsIotEndpoint, awsIotEndpointPort = None):
        """
        Updates aws_clientcredential.g for the AWS IoT endpoint defined in board.json
        args:
            awsIotEndpoint(str): Sets clientcredentialMQTT_BROKER_ENDPOINT[]
            awsIotEndpointPort(str): Optionally sets clientcredentialMQTT_BROKER_PORT, if specified.
        """
        self.__setIdentifierInFile(
            { 
                'static const char clientcredentialMQTT_BROKER_ENDPOINT[] =': '\"' + awsIotEndpoint + '\";',
                '#define clientcredentialMQTT_BROKER_PORT' : awsIotEndpointPort
            },
            os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_PATH)
        )
        
    def __setIdentifierInFile(self, prefixToValue, filePath):
        """
        Set the indentied value, from prefix the input prefixToValue map, in the file path 
        specified.
        If the value in the prefixToValue dictionary is None then the prefix is ignored if encountered.
        Args:
            prefixToValue (dict[str:str]): Identifier to value.
        """
        for line in fileinput.input(files=filePath, inplace=True):
            if any(line.startswith(prefix) for prefix in prefixToValue.keys()):
                prefix = next(prefix for prefix in prefixToValue.keys() if line.startswith(prefix))
                if prefixToValue[prefix] != None:
                    line = '{} {}\n'.format(prefix, prefixToValue[prefix])
            # print(line) will place an extra newline character in the file.
            sys.stdout.write(line)

    def setMqttLogsOn(self):
        """Set the MQTT debug logs to on in aws_mqtt_config.h
        """
        self.__setIdentifierInFile(
            { '#define mqttconfigENABLE_DEBUG_LOGS': '1' },
            os.path.join(self._projectRootDir, self._boardPortablePath, 'common', 'config_files', 'aws_mqtt_config.h')
        )

    def setFreeRtosConfigNetworkInterface(self, networkInterface):
        """Set the configNETWORK_INTERFACE_TO_USE in FreeRTOSConfig.h to networkInterface.
        Args:
            networkInterface (int): The number of the network interface
        """
        self.__setIdentifierInFile(
            {
                '#define configNETWORK_INTERFACE_TO_USE': str(networkInterface)
            },
            os.path.join(self._projectRootDir, self._boardPortablePath, 'common', 'config_files', 'FreeRTOSConfig.h')
        )
        
    def setClientCredentialForThingName(self, thingName):
        """Set aws_clientcredential.h with the input thingName.
        """
        self.__setIdentifierInFile(
            { '#define clientcredentialIOT_THING_NAME': '\"' + thingName + '\"' },
            os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_PATH)
        )

    def setClientCredentialKeys(self, certificate, privateKey):
        """Set the certificate and private key in aws_clientcredential_keys.h
        """
        self.__setIdentifierInFile(
            {
                '#define keyCLIENT_CERTIFICATE_PEM': '\"' + certificate.replace('\n', '\\n') + '\"',
                '#define keyCLIENT_PRIVATE_KEY_PEM': '\"' + privateKey.replace('\n', '\\n') + '\"'
            },
            os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_KEYS_PATH)
        )

    def setApplicationVersion(self, major, minor, bugfix):
        """Set aws_application_version.h with the input version.
        """
        self.__setIdentifierInFile(
            { 
                '#define APP_VERSION_MAJOR': major,
                '#define APP_VERSION_MINOR': minor,
                '#define APP_VERSION_BUILD': bugfix
            },
            os.path.join(self._projectRootDir, OtaAfrProject.APPLICATION_VERSION_PATH)
        )

    def setCodesignerCertificate(self, certificate):
        """Set aws_ota_codesigner_certificate.h with the certificate specified.
        """
        codeSignerCertificatePath = os.path.join(self._projectRootDir, OtaAfrProject.OTA_CODESIGNER_CERTIFICATE_PATH)
        signerCertificateTag = 'static const char signingcredentialSIGNING_CERTIFICATE_PEM[] = '
        for line in fileinput.input(files=codeSignerCertificatePath, inplace=True):
            if (signerCertificateTag in line):
                line = '{} {}\n'.format(signerCertificateTag, '\"' + certificate.replace('\n', '\\n') + '\";')
            sys.stdout.write(line)
            
    def setOtaUpdateDemoForRootCA(self):
        """Sets the secure connection certificate in the MQTT connection parameters 
        in the OTA update demo.
        """
        self.__setIdentifierInFile(
            {
                '            xConnectParams.pcCertificate =': '( char* ) clientcredentialROOT_CA_PEM;',
                '            xConnectParams.ulCertificateSize =': 'sizeof(clientcredentialROOT_CA_PEM)-1;'
            },
            os.path.join(self._projectRootDir, '..', OtaAfrProject.OTA_UPDATE_DEMO_PATH)
        )

    def setOtaUpdateDemoForNullCertificate(self):
        """Sets the secure connection certificate in the MQTT connection parameters 
        in the OTA update demo.
        """
        self.__setIdentifierInFile(
            {
                '            xConnectParams.pcCertificate =': 'NULL;',
                '            xConnectParams.ulCertificateSize =': '0;'
            },
            os.path.join(self._projectRootDir, '..', OtaAfrProject.OTA_UPDATE_DEMO_PATH)
        )

    def setOtaDemoRunnerForSNIDisabled(self):
        """Disabled SNI by setting mqttagentURL_IS_IP_ADDRESS in the connection parameters.
        """
        self.__setIdentifierInFile(
            {
                '            xConnectParams.xFlags =': 'mqttagentREQUIRE_TLS | mqttagentURL_IS_IP_ADDRESS;'
            },
            os.path.join(self._projectRootDir, '..', OtaAfrProject.OTA_UPDATE_DEMO_PATH)
        )

    def addRootCAToClientCredentialKeys(self, certificate):
        """Adds the Beta endpoint's root Certificate Authority to aws_clientcredential_keys.h. 
        The root CA was retrieved with openssl s_client -showcerts -connect iotmoonraker.us-east-1.beta.funkypineapple.io:8883
        """
        clientCertificateKeysPath = os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_KEYS_PATH)
        rootCAPemTag = 'static const char clientcredentialROOT_CA_PEM[] ='
        found = False
        for line in fileinput.input(files=clientCertificateKeysPath, inplace=True):
            if (rootCAPemTag in line):
                 line = '{} {}\n'.format(rootCAPemTag, ' \"' + certificate + '\";\n')
                 found = True
            sys.stdout.write(line)

        if not found:
            with open(clientCertificateKeysPath, 'a') as f:
                f.write('\nstatic const char clientcredentialROOT_CA_PEM[] = \"' + certificate + '\";\n')

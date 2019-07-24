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
import boto3
import botocore
import time
import os
import sys
import subprocess
import json
from uuid import uuid4
from datetime import datetime
from collections import namedtuple

def awsIotCliCommandForNonProdStage(command, stageParams):
    """Custom AWS IoT CLI command using the input command parameters and the
    stage parameters. A response is returned in JSON if there is one.
    Args:
        command(str): AWS IoT CLI Command and options omitting 'aws iot'
        stageParams(dict): 'endpoint-url' and 'region' retrieved from the ota_e2e script arguments.
    """
    response = subprocess.check_output(
        'aws ' + \
        'iot ' + \
        command + ' ' + \
        '--region ' + \
        stageParams['region'] + ' ' + \
        '--endpoint-url ' + \
        stageParams['endpoint-url'],
        shell=True # Test machine needs this parameter
    )
    if response:
        response = response.decode('utf-8')
        response = json.loads(response)
    return response

class OtaAwsAgent:
    """OtaAwsAgent manages all AWS resource usage related to OTA.

    * The AWS CLI must be configured before this class can be instantiated.

    - There should be a new instance of this class per OTA target board.

    - Each new instance of this class creates a new thing in AWS.

    - Each new instance of this class creates two s3 buckets in AWS, if these buckets do
      not already exist.
      1. unsigned s3 bucket "AWS_S3_UNSIGNED_BUCKET-boardName"
         This bucket is used when OtaAwsAgent.signFirmwareInS3Bucket() is called. The resulting
         image is transferred to the signed s3 bucket with the same firmwareFileName as in the
         unsigned bucket.
      2. signed s3 bucket "AWS_S3_SIGNED_BUCKET-boardName"
         This bucket is used when OtaAwsAgent.createJob() is called with the firmwareFileName as the key.

    - Exceptions in testing should clean up AWS resources using OtaAwsAgent.cleanup().

    Attributes:
        _awsIotClient(obj): boto3 AWS CLI IoT client
        _cleanOnExit(bool): Should all of the AWS resources get cleaned when this agent exits?
        _iotThing: The IoT thing created with this AwsOtaAgent.
        _s3Bucket: The S3 bucket created with this AwsOtaAgent.
        _otaRoleArn: The OTA Update Role ARN.
        _stage: The AWS service development environment stage.
    Methods:
        getThingName()
        getThingCertificate()
        getThingPrivateKey()
        getS3BucketName()
        uploadFirmwareToSignedBucket(localPathToFirmware, firmwareFileName)
        uploadFirmwareToS3Bucket(localPathToFirmware, firmwareFileName)
        signFirmwareInS3Bucket(firmwareFileName, awsSignerCertArn, awsSignerCertFilename, signerPlatform)
        createIotStream(s3BucketName = None, firmwareFileName = None, customFiles = None)
        createOtaUpdate(deviceImageFileName = None, streamId = None, signerJobId = None, customFiles = None)
        quickCreateOtaUpdate(otaConfig)
        pollOtaUpdateCompletion(otaUpdateId, timeout)
        cancelJob(jobId)
        cleanup()
        __getJobStatus(jobId)
    Example:
        # This example shows all steps needed to OTA.
        otaAwsAgent = OtaAwsAgent("test_mcu", 'arn:aws:iam::123:role/OTACreateJobRole', 'ota-bucket', True)
        # Upload firmware
        otaAwsAgent.uploadFirmwareToS3Bucket("C:/workspace/project/debug/image.out", "image.out")
        # Sign the image
        signerJobId = otaAwsAgent.signFirmwareInS3Bucket("image.out", "arn:aws:acm:us-west-2:1234:certificate/SomeUniqueId-1234", "certpath.cert", "WindowsSimulator")
        # Create a Stream
        streamId = self.createIotStream( self.getS3BucketName(), signerJobId)
        # Create the OTA update job.
        otaUpdateId = self.createOtaUpdate(otaConfig['device_firmware_file_name'], streamId, signerJobId)
    """
    def __init__(self, boardName, otaRoleArn, s3BucketName, stageParams, cleanOnExit):
        self._awsIotClient = boto3.client('iot')
        self._cleanOnExit = cleanOnExit
        self._iotThing = AWSIoTThing(boardName, stageParams)
        self._s3Bucket = AWSS3Bucket(s3BucketName, stageParams)
        self._otaRoleArn = otaRoleArn
        self._stageParams = stageParams
        self._boardName = boardName

        # TODO: Create an OTA Role automatically with agent creation.
        # TODO: Create certificates and upload to ACM with agent creation.

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """Make sure we clean up if the agent is destroyed."""
        self.cleanup()

    def getAwsIotEndpoint(self):
        """Return the AWS IoT Core custom endpoint defined under the "Settings" section in the AWS IoT Core console.
        """
        response = {}
        if self._stageParams:
            response = awsIotCliCommandForNonProdStage(
                'describe-endpoint',
                self._stageParams
            )
        else:
            response = self._awsIotClient.describe_endpoint()
        return response['endpointAddress']

    def getCodeSignerCertificateFromArn(self, certArn):
        """Get the certificate stored in ACM identified by the input ARN.
        If we are running in the beta stage, the certificate ARN for the designated region
        should have been configured the board configuration JSON.
        """
        certificate = ''
        if self._stageParams:
            response = subprocess.check_output(
                'aws acm get-certificate --certificate-arn ' + certArn + ' ' + \
                '--region ' + \
                self._stageParams['region'],
                shell=True
            ).decode('utf-8')
            certificate = json.loads(response)['Certificate']
        else:
            certificate = boto3.client('acm').get_certificate(CertificateArn=certArn)['Certificate']
        return certificate

    def getThingName(self):
        """Get the name of the thing associate with this OTA agent. """
        return self._iotThing.thing_name

    def getThingCertificate(self):
        """Get the certificate of the thing associated with this OTA agent. """
        return self._iotThing.cert

    def getThingPrivateKey(self):
        """Get the private key of the thing associate with this OTA agent. """
        return self._iotThing.prv_key

    def getS3BucketName(self):
        """Get the name of the S3 bucket associated with this OTA agent. """
        return self._s3Bucket.s3_name

    def getS3ObjectVersion(self, key):
        """Get the version of the object, in the s3 bucket held by this OTA AWS agent, denoted by key.
        Will throw an exception if the the object doesn't exist."""
        return self._s3Bucket.get_s3_object(key).version_id

    def downloadS3File(self, key, fileSavePath):
        """Download the firmware file specified by key to the fileSavePath.
        Args:
            key(str): The key of the S3 object.
            fileSavePath(str): The path to save the file downloaded.
        """
        self._s3Bucket.download_file(key, fileSavePath)

    def uploadFirmwareToS3Bucket(self, localPathToFirmware, firmwareFileName):
        """Upload a firmware image to the unsigned S3 bucket associated with this OTA agent.
        Args:
            localPathToFirmware(str): The path on the machine this script is running of the firmware image.
            firmwareFileName(str): The name of the firmware, this is used as the key in the S3 bucket.
        """
        self._s3Bucket.upload_file(localPathToFirmware, firmwareFileName)

    def signFirmwareInS3Bucket(self, firmwareFileName, awsSignerCertArn, awsSignerCertFilename, signerPlatform, profileName = None):
        """Signs the firmware with key firmwareFileName.
        Returns the signer job ID, which is also the key of signed image.
        Args:
            profileName(str): The name of the signing profile to create or to use if it already exists. 
                This is added a parameter in case tests want to use different certificates to sign.
            firmwareFileName(str): The name of the firmware, the key in the unsigned s3 bucket.
            awsSignerCertArn(str): The ARN of the signer certificates in ACM.
            awsSignerCertFilename(str): The path on the MCU device of the signer certificate.
            signerPlatform(str): The AWS Signer service identifier for the MCU.
        """
        # Create an AWS Signer client.
        signer = boto3.client('signer')
        # Timeout on AWS signer service.
        AWS_SIGNER_TIMEOUT = 30

        if not profileName:
            profileName = "%s%s"%(self.getThingName()[-8:], self._boardName[:10])

        # Get the object.
        firmwareObject = self._s3Bucket.get_s3_object(firmwareFileName)

        if self._stageParams:
            try:
                # region parameter needed for beta is now allowed in the boto3 API.
                response = subprocess.check_output(
                    'aws signer put-signing-profile --profile-name ' + profileName + ' ' + \
                    '--signing-material certificateArn=\"' + awsSignerCertArn + '\" ' + \
                    '--platform-id ' + signerPlatform + ' ' + \
                    '--signing-parameters certname=\" ' + awsSignerCertFilename + '\" ' + \
                    '--region ' + self._stageParams['region'] + \
                    (' --endpoint-url ' + self._stageParams['signer-endpoint-url'] if self._stageParams['signer-endpoint-url'] else ''),
                    shell=True
                ).decode('utf-8')
                print('put-signing-profile response: ' + response)
            except Exception as e:
                # Do nothing when put-signing-profile fails because that means that the signing profile probably already
                # exists.
                print(e)

            startSigningResponse = subprocess.check_output(
                'aws signer start-signing-job ' + \
                '--profile-name ' + profileName + ' ' + \
                '--source \"s3={bucketName=' + self._s3Bucket.s3_name + ',key=' + firmwareFileName + ',version=' + firmwareObject.version_id + '}\" ' + \
                '--destination \"s3={bucketName=' + self._s3Bucket.s3_name + '}\" ' + \
                '--region ' + self._stageParams['region'] + \
                (' --endpoint-url ' + self._stageParams['signer-endpoint-url'] if self._stageParams['signer-endpoint-url'] else ''),
                shell=True
            ).decode('utf-8')
            startSigningResponse = json.loads(startSigningResponse)
        else:
            try:
                signer.put_signing_profile(
                    profileName = profileName,
                    signingMaterial = {
                        'certificateArn' : awsSignerCertArn,
                    },
                    platformId = signerPlatform,
                    signingParameters = {
                        'certname' : awsSignerCertFilename
                    }
                )
            except Exception as e:
                # Do nothing when put-signing-profile fails because that means that the signing profile probably already
                # exists.
                print(e)
                print('NOTE: If the last error was a name already exists error that is OK.')

            # Start a signing job on the new firmware image.
            startSigningResponse = signer.start_signing_job(
                clientRequestToken = str(uuid4()),
                profileName= profileName,
                destination = {
                    's3': {
                        'bucketName': self._s3Bucket.s3_name,
                        'prefix': ''
                    }
                },
                source = {
                    's3': {
                        'bucketName': self._s3Bucket.s3_name,
                        'key': firmwareFileName,
                        'version': firmwareObject.version_id
                    }
                }
            )

        # Confirm that the signing job succeeded
        timeout_end = time.time() + AWS_SIGNER_TIMEOUT
        signingInProgress = True
        while signingInProgress and time.time() < timeout_end:
            time.sleep(1)
            describeSigningResponse = {}
            if self._stageParams:
                describeSigningResponse = subprocess.check_output(
                    'aws signer describe-signing-job ' + \
                    '--job-id ' + startSigningResponse['jobId'] + ' ' + \
                    '--region ' + self._stageParams['region'] + \
                    (' --endpoint-url ' + self._stageParams['signer-endpoint-url'] if self._stageParams['signer-endpoint-url'] else ''),
                    shell=True
                ).decode('utf-8')
                describeSigningResponse = json.loads(describeSigningResponse)
            else:
                describeSigningResponse = signer.describe_signing_job(jobId = startSigningResponse['jobId'])
            if describeSigningResponse['status'] != 'InProgress':
                signingInProgress = False

        if signingInProgress == True:
            print("Error: code-signing timed out.")
            return None # Return an invalid signer job ID

        if describeSigningResponse['status'] == 'Failed':
            print("Error: code-signing failed because " + describeSigningResponse['statusReason'])
            return None

        print("Completed AWS Signer job ID {0}.".format(startSigningResponse['jobId']))

        return describeSigningResponse['jobId']

    def createIotStream(self, s3BucketName = None, firmwareFileName = None, customFiles = None):
        """Create an IOT stream of the firmware file in the given s3 bucket.
        Args:
            s3BucketName(str): The name of the bucket to create a stream from.
            firmwareFileName(str): The key of the file in the S3 bucket to create a stream from.
            customFiles(dict): An AWS CLI compliant dictionary of the files we want to stream in one go. If this
                is left blank then we just use the s3BucketName and firmwareFileName supplied.
        """
        if customFiles == None:
            customFiles = [
                {
                    'fileId': 0,
                    's3Location': {
                        'bucket': s3BucketName,
                        'key': firmwareFileName,
                    }
                },
            ]

        if self._stageParams:
            createStreamResponse = awsIotCliCommandForNonProdStage(
                'create-stream --stream-id ' + \
                str(uuid4()) + ' --files \"' + \
                json.dumps(customFiles).replace('\"', '\\\"') + \
                '\" --role-arn ' + \
                self._otaRoleArn,
                self._stageParams
            )
        else:
            createStreamResponse = self._awsIotClient.create_stream(
                streamId = str(uuid4()),
                files= customFiles,
                roleArn = self._otaRoleArn
            );

        return createStreamResponse['streamId']

    def createOtaUpdate(self, deploymentFiles, roleArn = None):
        """Create an OTA update job.
        Returns the AWS IoT OTA Update ID.
        Args:
            deviceImageFileName(str): The full path to the image in the device's file system. For devices not using
                a file system any string can be put in here.
            streamId (str): The AWS ID of the stream returned from AwsOtaAgent.createIotStream().
            signerJobId(str): The AWS Job ID of the completed AWS Signer operation. This ID was returned from
                function AwsOtaAgent.signFirmwareInS3Bucket().
            deploymentFiles (dict): An AWS CLI compliant dictionary of the deployment file(s) information to create an OTA update job for.
        """

        # Timeout for the AWS Job service to create an OTA update job.
        AWS_CREATE_OTA_UPDATE_JOB_TIMEOUT = 60

        createOtaResponse = {}
        if self._stageParams:
            createOtaResponse = awsIotCliCommandForNonProdStage(
                'create-ota-update ' + \
                '--ota-update-id ' + \
                str(uuid4()) + ' ' + \
                '--files ' + \
                '\"' + json.dumps(deploymentFiles).replace('\"', '\\\"') + '\" ' + \
                '--targets ' + \
                self._iotThing.thing_arn + ' ' + \
                '--role-arn ' + \
                self._otaRoleArn,
                self._stageParams
            )
        else:
            createOtaResponse = self._awsIotClient.create_ota_update(
                otaUpdateId = str(uuid4()),
                targets = [
                    self._iotThing.thing_arn
                ],
                targetSelection = 'SNAPSHOT',
                roleArn = self._otaRoleArn,
                files = deploymentFiles
            )

        # Confirm that the OTA update job is ready.
        timeout_end = time.time() + AWS_CREATE_OTA_UPDATE_JOB_TIMEOUT
        otaCreateInProgress = True
        while otaCreateInProgress and time.time() < timeout_end:
            time.sleep(1)
            otaGetStatusResponse = {}
            if self._stageParams:
                otaGetStatusResponse = awsIotCliCommandForNonProdStage(
                    'get-ota-update ' + \
                    '--ota-update-id ' + \
                    createOtaResponse['otaUpdateId'],
                    self._stageParams
                )
            else:
                otaGetStatusResponse = self._awsIotClient.get_ota_update(
                    otaUpdateId = createOtaResponse['otaUpdateId'])
            if otaGetStatusResponse['otaUpdateInfo']['otaUpdateStatus'] in ('CREATE_COMPLETE', 'CREATE_FAILED'):
                otaCreateInProgress = False

        if otaCreateInProgress == True:
            print("Error: OTA update creation timed out for OTA update ID " + createOtaResponse['otaUpdateInfo']['otaUpdateId'])
            return None

        # Check for errors and show us what those errors might be.
        if otaGetStatusResponse['otaUpdateInfo']['otaUpdateStatus'] != 'CREATE_COMPLETE':
            print("Error: OTA update creation failed for OTA update ID " + otaGetStatusResponse['otaUpdateInfo']['otaUpdateId'])
            if ('errorInfo' in otaGetStatusResponse['otaUpdateInfo']):
                print('Error Info: ' + otaGetStatusResponse['otaUpdateInfo']['errorInfo']['code'] + ':' + otaGetStatusResponse['otaUpdateInfo']['errorInfo']['message'])
        else:
            print("Created OTA Update ID {0} (AWS IoT job ID = {1}).".format(
                otaGetStatusResponse['otaUpdateInfo']['otaUpdateId'],
                otaGetStatusResponse['otaUpdateInfo']['awsIotJobId']))

        return otaGetStatusResponse['otaUpdateInfo']['otaUpdateId']

    def quickCreateOtaUpdate(self, otaConfig):
        """Create an OTA update in AWS IoT by using the otaConfig.
        We follow the path of:
            1. Upload unsigned image to the unsigned s3 bucket.
            2. Sign the image in the unsigned s3 bucket to the signed s3 bucket.
            3. Create a IoT stream from the image in the signed s3 bucket.
            4. Create an OTA Update job from the IoT Stream and signer information.

        This function follows the happy path OTA case. It is nice for testing changes
        where the only image is manipulated.
        Returns AWS IoT Ota Update ID
        Args:
            otaConfig - 'ota_config' from board.json

        """
        # Upload to the s3 bucket.
        self.uploadFirmwareToS3Bucket(
            otaConfig['ota_firmware_file_path'],
            os.path.basename(otaConfig['ota_firmware_file_path'])
        )

        signingProfile = "%s%s"%(self.getThingName()[-8:], self._boardName[:10])
        otaUpdateId = self.createOtaUpdate(
            deploymentFiles = [
                {
                    'fileName': otaConfig['device_firmware_file_name'],
                    'fileVersion': '1',
                    'fileLocation': {
                        's3Location': {
                            'bucket': otaConfig['aws_s3_bucket_name'],
                            'key': os.path.basename(otaConfig['ota_firmware_file_path']),
                            'version': self.getS3ObjectVersion(os.path.basename(otaConfig['ota_firmware_file_path']))
                        }
                    },
                    'codeSigning': {
                        "startSigningJobParameter": {
                            'signingProfileName': signingProfile,
                            'signingProfileParameter': {
                                'platform': otaConfig['aws_signer_platform'],
                                'certificateArn': otaConfig['aws_signer_certificate_arn'],
                                'certificatePathOnDevice': otaConfig['aws_signer_certificate_file_name']
                            }
                        }
                    }
                },
            ]
        )

        return otaUpdateId


    def __getJobStatus(self, jobId):
        """Get the status of the OTA Update's job from the AWS IoT jobId.
        Args:
            jobId(str): The AWS IoT Job ID to get the status of.
        Returns: The job status and the reason for the status in a namedtuple.
        """
        JobStatus = namedtuple('JobStatus', 'status reason')
        try:
            response = {}
            if self._stageParams:
                response = awsIotCliCommandForNonProdStage(
                    'describe-job-execution ' + \
                    '--job-id ' + \
                    jobId + ' ' \
                    '--thing-name ' + \
                    self._iotThing.thing_name,
                    self._stageParams
                )
            else:
                response = self._awsIotClient.describe_job_execution(jobId=jobId, thingName=self._iotThing.thing_name)
        except:
            print("aws iot describe-job-execution failed.")
            return JobStatus('UNDEFINED', 'aws iot describe-job-execution failed.')
        executionResponse = response['execution']
        return JobStatus(executionResponse['status'], executionResponse['statusDetails'].get('detailsMap', {}).get('reason', ''))

    def pollOtaUpdateCompletion(self, otaUpdateId, timeout):
        """Poll on the job status waiting for it complete.
        Returns the job status on completion. The job status is a namedtuple with fields
        'status' and 'reason'.
        Returns the job status and reason in a namedtuple (status, reason)
        Args:
            otaUpdateId(str): The AWS IoT OTA Update ID to poll completion of.
            timeout(int): The timeout in seconds to poll for.
        """
        seconds = 0

        # Get the AWS Iot Job ID
        response = {}
        if self._stageParams:
            response = awsIotCliCommandForNonProdStage(
                'get-ota-update ' + \
                '--ota-update-id ' + \
                otaUpdateId,
                self._stageParams
            )
        else:
            response = self._awsIotClient.get_ota_update(otaUpdateId=otaUpdateId)
        jobId = response['otaUpdateInfo']['awsIotJobId']

        # Poll on the job status
        jobStatus = self.__getJobStatus(jobId)
        finishedJobStatuses = {'CANCELED', 'SUCCEEDED', 'FAILED', 'REJECTED', 'REMOVED'}
        while seconds < timeout and jobStatus.status not in finishedJobStatuses:
            time.sleep(5)
            seconds = seconds + 5
            jobStatus = self.__getJobStatus(jobId)

        if jobStatus.status not in finishedJobStatuses:
            if seconds >= timeout :
                print("Timeout on OTA Update's job.")
            # Clean up incomplete jobs.
            self.cancelJob(jobId)
        print("OTA Update's job status: " + jobStatus.status)
        # Clean up the OTA Update
        self.deleteOtaUpdate(otaUpdateId)
        return jobStatus

    def deleteOtaUpdate(self, otaUpdateId):
        """
        Cancel the input OTA Update. Cleans up the stream associated.
        Args:
            otaUpdateId(str): The AWS IoT OTA Update ID to cancel.
        """
        response = {}
        try:
            if self._stageParams:
                response = awsIotCliCommandForNonProdStage(
                    'delete-ota-update ' + \
                    '--ota-update-id ' + \
                    otaUpdateId + ' ' + \
                    '--delete-stream',
                    self._stageParams
                )
            else:
                response = self._awsIotClient.delete_ota_update(otaUpdateId=otaUpdateId, deleteStream=True)
        except Exception as e:
            print("Unable to delete ota update with ID: " + otaUpdateId)
            print(response)
            print(e)


    def cancelJob(self, jobId):
        """
        Cancel the input job ID.
        Args:
            jobId(str): The AWS IoT job ID to cancel.
        """
        response = {}
        try:
            if self._stageParams:
                response = awsIotCliCommandForNonProdStage(
                    'cancel-job ' + \
                    '--job-id ' + \
                    jobId + ' ' + \
                    '--comment ' + \
                    '\"OTA integration testing cancellation of incomplete job.\" ' + \
                    '--force',
                    self._stageParams
                )
            else:
                response = self._awsIotClient.cancel_job(jobId=jobId, comment='OTA integration testing cancellation of incomplete job.', force=True)
        except Exception as e:
            print("Unable to cancel job with ID: " + jobId)
            print(response)
            print(e)

    def cleanup(self):
        """Clean up all AWS IoT resources needed by the OTA test agent.
        """
        if self._cleanOnExit:
            self._iotThing.cleanup()
            self._s3Bucket.cleanup()


class AWSIoTThing:
    """Aws IoT Thing.
    """
    def __init__(self, name_prefix, stageParams):
        self._stageParams = stageParams
        self._iot_client = boto3.client('iot')
        self._account_id = boto3.client('sts').get_caller_identity()['Account']
        if self._stageParams:
            self._region_name = self._stageParams['region']
            self._iot_thing = awsIotCliCommandForNonProdStage(
                'create-thing --thing-name {}-{}'.format(name_prefix, uuid4().hex[:8]),
                self._stageParams
            )
            self._cert_keys = awsIotCliCommandForNonProdStage(
                'create-keys-and-certificate --set-as-active',
                self._stageParams
            )
        else:
            self._region_name = boto3.Session().region_name
            self._iot_thing = self._iot_client.create_thing(thingName='{}-{}'.format(name_prefix, uuid4().hex[:8]))
            self._cert_keys = self._iot_client.create_keys_and_certificate(setAsActive=True)
        self.thing_name = self._iot_thing['thingName']
        self.thing_arn = self._iot_thing['thingArn']
        self.cert = self._cert_keys['certificatePem']
        self.cert_arn = self._cert_keys['certificateArn']
        self.cert_id = self._cert_keys['certificateId']
        self.pub_key = self._cert_keys['keyPair']['PublicKey']
        self.prv_key = self._cert_keys['keyPair']['PrivateKey']
        self._policy_name = self.thing_name + '-ota-policy'
        self._policy_doc = "{ \
            \"Version\": \"2012-10-17\", \
            \"Statement\": [ \
            { \
                \"Effect\": \"Allow\", \
                \"Action\": \"iot:*\", \
                \"Resource\": \"*\" \
            } \
            ] \
        }"
        if self._stageParams:
            awsIotCliCommandForNonProdStage(
                'create-policy --policy-name ' + self._policy_name + ' --policy-document ' + '\"' + self._policy_doc.replace('\"', '\\\"') + '\"',
                self._stageParams
            )
            awsIotCliCommandForNonProdStage(
                'attach-policy --policy-name ' + self._policy_name + ' --target ' + self.cert_arn,
                self._stageParams
            )
        else:
            self._iot_client.create_policy(policyName=self._policy_name, policyDocument=self._policy_doc)
            self._iot_client.attach_policy(policyName=self._policy_name, target=self.cert_arn)
            self._iot_client.attach_thing_principal(thingName=self.thing_name, principal=self.cert_arn)

    def cleanup(self):
        if self._stageParams:
            awsIotCliCommandForNonProdStage(
                'detach-thing-principal --thing-name ' + self.thing_name + ' --principal ' + self.cert_arn,
                self._stageParams
            )
            awsIotCliCommandForNonProdStage(
                'detach-policy --policy-name ' + self._policy_name + ' --target ' + self.cert_arn,
                self._stageParams
            )
            awsIotCliCommandForNonProdStage(
                'delete-policy --policy-name ' + self._policy_name,
                self._stageParams
            )
            awsIotCliCommandForNonProdStage(
                'update-certificate --certificate-id ' + self.cert_id + ' --new-status INACTIVE',
                self._stageParams
            )
            awsIotCliCommandForNonProdStage(
                'delete-certificate --certificate-id ' + self.cert_id,
                self._stageParams
            )
            awsIotCliCommandForNonProdStage(
                'delete-thing --thing-name ' + self.thing_name,
                self._stageParams
            )
        else:
            self._iot_client.detach_thing_principal(thingName=self.thing_name, principal=self.cert_arn)
            self._iot_client.detach_policy(policyName=self._policy_name,target=self.cert_arn)
            self._iot_client.delete_policy(policyName=self._policy_name)
            self._iot_client.update_certificate(certificateId=self.cert_id, newStatus='INACTIVE')
            self._iot_client.delete_certificate(certificateId=self.cert_id)
            self._iot_client.delete_thing(thingName=self.thing_name)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.cleanup()


class AWSS3Bucket:
    """ AWS S3 Versioned Bucket.
    """
    def __init__(self, name, stageParams):
        self._s3_client = boto3.resource('s3')
        if stageParams:
            self._s3_client = boto3.resource('s3', region_name='us-east-1')
        self._stageParams = stageParams
        self.s3_name = name
        self.s3_bucket = self._s3_client.Bucket(self.s3_name)
        self.__create_bucket();

    def __create_bucket(self):
        response = None
        try:
            response = self._s3_client.meta.client.head_bucket(Bucket = self.s3_name)
        except Exception:
            if self._stageParams:
                # To create a bucket in us-east-1 we must specify no LocationConstraint.
                self.s3_bucket = self._s3_client.create_bucket(Bucket = self.s3_name)
            else:
                self.s3_bucket = self._s3_client.create_bucket(
                    Bucket = self.s3_name,
                    CreateBucketConfiguration = {
                        'LocationConstraint': boto3.session.Session().region_name
                    }
                )
        # If the bucket exists we want to make sure it is in the right region for the AWS development stage.
        if response and self._stageParams:
            if response['ResponseMetadata']['HTTPHeaders']['x-amz-bucket-region'] != 'us-east-1':
                raise Exception('ERROR: Please delete S3 bucket {} because it is in region {}, and rerun this script. This script does not delete the bucket because after deleting the bucket it takes 1+ hours for a bucket of the same name of be made again.'.format(self.s3_name, response['ResponseMetadata']['HTTPHeaders']['x-amz-bucket-region']))
        self._s3_client.BucketVersioning(self.s3_name).enable();

    def upload_file(self, file_path, file_name):
        self._s3_client.Bucket(self.s3_name).upload_file(file_path, file_name)

    def download_file(self, key, file_path):
        try:
            self._s3_client.Bucket(self.s3_name).download_file(key, file_path)
        except botocore.exceptions.ClientError as e:
            if e.response['Error']['Code'] == "404":
                print("The object does not exist.")
            else:
                raise

    def get_s3_object(self, key):
        return self._s3_client.Object(self.s3_name, key)

    def cleanup(self):
        try:
            self._s3_client.meta.client.head_bucket(Bucket = self.s3_name)
        except:
            # Bucket doesn't exist nothing to clean.
            return
        self.s3_bucket.object_versions.delete()
        # We will no longer delete the bucket because we want to keep our bucket name.
        #self.s3_bucket.delete()

    def __enter__(self):
        return self

    def __exit__(self):
        self.cleanup()

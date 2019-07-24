## Greengrass Discovery Demo
In order to setup and run the Amazon FreeRTOS demo application for Greengrass discovery, please follow the instructions in [Greengrass Discovery Demo](https://docs.aws.amazon.com/freertos/latest/userguide/gg-demo.html).

## Advanced Diagnostics

It can be instructive to use the AWS command-line interface (CLI) to drill into the cloud-based configuration of your Greengrass Core, and of the AWS IoT Thing that you're using with Amazon FreeRTOS. Check online for instructions on [installing](https://docs.aws.amazon.com/cli/latest/userguide/installing.html) and [configuring](https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-getting-started.html) the AWS CLI. 

In order to perform advanced diagnostics on my Greengrass Core configuration, I start by listing all of the Greengrass groups associated with my account in my default AWS region (i.e., the region I've set as the default for my AWS CLI environment). I happen to only have one Greengrass group, as shown in the following output.

```
C:\>aws greengrass list-groups
{
    "Groups": [
        {
            "LatestVersionArn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/groups/37cb3ae6-5d8a-4639-949e-1aa0d11ea196/versions/a8a30e19-17d2-4c7a-9a01-140eb0267710",
            "Name": "DcgGgGroup",
            "LastUpdatedTimestamp": "2017-08-23T02:32:50.387Z",
            "LatestVersion": "a8a30e19-17d2-4c7a-9a01-140eb0267710",
            "CreationTimestamp": "2017-08-23T02:32:50.387Z",
            "Id": "37cb3ae6-5d8a-4639-949e-1aa0d11ea196",
            "Arn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/groups/37cb3ae6-5d8a-4639-949e-1aa0d11ea196"
        }
    ]
}
```

Using the group ID and group version ID from the output above, I query for details about the latest version of the group:

```
C:\>aws greengrass get-group-version --group-id 37cb3ae6-5d8a-4639-949e-1aa0d11ea196  --group-version-id a8a30e19-17d2-4c7a-9a01-140eb0267710
{
    "Definition": {
        "CoreDefinitionVersionArn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/cores/35821d2d-523a-4a54-91f4-1fde25673a50/versions/8551f1d0-4788-43ee-b734-8d4f72a1a163",
        "LoggerDefinitionVersionArn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/loggers/f26c3ddb-089b-4b7d-b3a3-346a2c6edc09/versions/1a31a484-8abd-410f-964a-e8161b063f0f",
        "FunctionDefinitionVersionArn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/functions/62ee8234-effd-4465-929d-f40b41e989fa/versions/620729fe-62a5-4f25-8e53-edc6454ecc14",
        "DeviceDefinitionVersionArn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/devices/097a9ca7-6551-48a5-b725-37a5b1197541/versions/44f48272-5fb8-4b74-b350-07057ba3debd",
        "SubscriptionDefinitionVersionArn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/subscriptions/b8698952-fd19-48f7-bb9c-0ea3c148f12d/versions/7586b4c9-0088-42b8-bec5-e278eac2f57e"
    },
    "Version": "a8a30e19-17d2-4c7a-9a01-140eb0267710",
    "CreationTimestamp": "2018-02-07T18:05:34.412Z",
    "Id": "37cb3ae6-5d8a-4639-949e-1aa0d11ea196",
    "Arn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/groups/37cb3ae6-5d8a-4639-949e-1aa0d11ea196/versions/a8a30e19-17d2-4c7a-9a01-140eb0267710"
}
```

Using the core definition version, and its ID, from the above output, I query for details:

```
C:\>aws greengrass get-core-definition-version --core-definition-id 35821d2d-523a-4a54-91f4-1fde25673a50 --core-definition-version-id 8551f1d0-4788-43ee-b734-8d4f72a1a163
{
    "Definition": {
        "Cores": [
            {
                "CertificateArn": "arn:aws:iot:us-west-2:785484208847:cert/164d139290d3e6b526ebf3d7487efb8b6d1e2d43cec65e352c9ea46b772cbcba",
                "ThingArn": "arn:aws:iot:us-west-2:785484208847:thing/ip-172-00-11-22",
                "SyncShadow": false,
                "Id": "daf07f48-a385-4de3-a20f-cd75b95d0854"
            }
        ]
    },
    "Version": "8551f1d0-4788-43ee-b734-8d4f72a1a163",
    "CreationTimestamp": "2017-08-23T02:32:50.183Z",
    "Id": "35821d2d-523a-4a54-91f4-1fde25673a50",
    "Arn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/cores/35821d2d-523a-4a54-91f4-1fde25673a50/versions/8551f1d0-4788-43ee-b734-8d4f72a1a163"
}
```

The above core definition is one way for me to get the Thing name of my Greengrass core. This allows me to double-check its connectivity information. Note that Greengrass Core connectivity information can either be explicitly defined or be auto-discovered. Whatever the connectivity info, I should next confirm that it is accessible from the network that my Amazon FreeRTOS device is on. If the Amazon FreeRTOS device can't discover the Greengrass Core, or if it can't connect to it, then this demo will fail. 

```
C:\>aws greengrass get-connectivity-info --thing-name ip-172-00-11-22
{
    "ConnectivityInfo": [
        {
            "PortNumber": 8883,
            "HostAddress": "ec2-34-200-100-100.us-west-2.compute.amazonaws.com",
            "Id": "c512dbd2-2df2-4180-80c7-fba02a787c2c"
        }
    ]
}
```

Next, I query the device definition details from my group:

```
C:\>aws greengrass get-device-definition --device-definition-id 097a9ca7-6551-48a5-b725-37a5b1197541
{
    "LatestVersionArn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/devices/097a9ca7-6551-48a5-b725-37a5b1197541/versions/44f48272-5fb8-4b74-b350-07057ba3debd",
    "LatestVersion": "44f48272-5fb8-4b74-b350-07057ba3debd",
    "LastUpdatedTimestamp": "2017-08-23T02:33:26.814Z",
    "CreationTimestamp": "2017-08-23T02:33:26.814Z",
    "Id": "097a9ca7-6551-48a5-b725-37a5b1197541",
    "Arn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/devices/097a9ca7-6551-48a5-b725-37a5b1197541"
}
```

The latest version of the device definition tells me the Things that are a member of that group. For Greengrass discovery to work, the client device must be authenticating as one of the things in this list:

```
C:\>aws greengrass get-device-definition-version --device-definition-id 097a9ca7-6551-48a5-b725-37a5b1197541 --device-definition-version-id  44f48272-5fb8-4b74-b350-07057ba3debd
{
    "Definition": {
        "Devices": [
            {
                "CertificateArn": "arn:aws:iot:us-west-2:785484208847:cert/003a4c70a976add0ebbeb4957b98c6b6436906bcb8e6ecc6ba5445f995e753d7",
                "ThingArn": "arn:aws:iot:us-west-2:785484208847:thing/dcgecc",
                "SyncShadow": true,
                "Id": "0fad8bc6-502e-41ca-a5ad-bbc716c1c215"
            },
            {
                "CertificateArn": "arn:aws:iot:us-west-2:785484208847:cert/8ddbd09c8570ee06c8324867e6f54908e163814c80a2112bf1c4d14af8409cff",
                "ThingArn": "arn:aws:iot:us-west-2:785484208847:thing/WindowsLaptop",
                "SyncShadow": true,
                "Id": "5ed4c3f6-51ca-4cd2-9315-bddb263f4949"
            }
        ]
    },
    "Version": "44f48272-5fb8-4b74-b350-07057ba3debd",
    "CreationTimestamp": "2017-11-20T00:06:00.628Z",
    "Id": "097a9ca7-6551-48a5-b725-37a5b1197541",
    "Arn": "arn:aws:greengrass:us-west-2:785484208847:/greengrass/definition/devices/097a9ca7-6551-48a5-b725-37a5b1197541/versions/44f48272-5fb8-4b74-b350-07057ba3debd"
}
```

Now that I've explored the configuration of my Greengrass Core, I want to verify the configuration of the AWS IoT Thing that's running this demo. I start by querying the certificate from the AWS IoT registry. Separately, I must confirm that certificate output from the command below is the same certificate I am using in my Amazon FreeRTOS device code:

```
C:\>aws iot describe-certificate --certificate-id 003a4c70a976add0ebbeb4957b98c6b6436906bcb8e6ecc6ba5445f995e753d7
{
    "certificateDescription": {
        "certificateArn": "arn:aws:iot:us-west-2:785484208847:cert/003a4c70a976add0ebbeb4957b98c6b6436906bcb8e6ecc6ba5445f995e753d7",
        "status": "ACTIVE",
        "certificateId": "003a4c70a976add0ebbeb4957b98c6b6436906bcb8e6ecc6ba5445f995e753d7",
        "lastModifiedDate": 1511135938.521,
        "certificatePem": "-----BEGIN CERTIFICATE-----\n...\n-----END CERTIFICATE-----\n",
        "transferData": {},
        "ownedBy": "785484208847",
        "creationDate": 1511135938.521
    }
}
```

Figure out what policy is attached to my client device certificate:

```
C:\>aws iot list-attached-policies --target arn:aws:iot:us-west-2:785484208847:cert/003a4c70a976add0ebbeb4957b98c6b6436906bcb8e6ecc6ba5445f995e753d7
{
    "policies": [
        {
            "policyName": "ShadowMk5",
            "policyArn": "arn:aws:iot:us-west-2:785484208847:policy/ShadowMk5"
        }
    ]
}
```

Dump out the device policy and confirm that it is what I expect.

```
C:\>aws iot get-policy --policy-name ShadowMk5
{
    "policyName": "ShadowMk5",
    "policyArn": "arn:aws:iot:us-west-2:785484208847:policy/ShadowMk5",
    "policyDocument": "{\n  \"Version\": \"2012-10-17\",\n  \"Statement\": [\n    {\n      \"Effect\": \"Allow\",\n      \"Action\": \"iot:Subscribe\",\n      \"Resource\": \"arn:aws:iot:us-west-2:785484208847:topicfilter/$aws/things/dcgecc/shadow/*\"\n    },\n    {\n      \"Effect\": \"Allow\",\n      \"Action\": \"iot:Receive\",\n      \"Resource\": \"arn:aws:iot:us-west-2:785484208847:topic/$aws/things/dcgecc/shadow/*\"\n    },\n    {\n      \"Effect\": \"Allow\",\n      \"Action\": \"iot:Publish\",\n      \"Resource\": \"arn:aws:iot:us-west-2:785484208847:topic/$aws/things/dcgecc/shadow/update\"\n    },\n    {\n      \"Effect\": \"Allow\",\n      \"Action\": \"iot:Publish\",\n      \"Resource\": \"arn:aws:iot:us-west-2:785484208847:topic/$aws/things/dcgecc/shadow/get\"\n    },\n    {\n      \"Effect\": \"Allow\",\n      \"Action\": \"iot:Publish\",\n      \"Resource\": \"arn:aws:iot:us-west-2:785484208847:topic/$aws/things/dcgecc/shadow/delete\"\n    },\n    {\n      \"Effect\": \"Allow\",\n      \"Action\": \"iot:Connect\",\n      \"Resource\": \"arn:aws:iot:us-west-2:785484208847:client/dcgecc\"\n    }\n  ]\n}",
    "defaultVersionId": "6"
}
```

Finally, if the AWS console shows that the Greengrass group configuration is as expected, but the Core itself doesn't seem to be behaving as expected, it can be helpful to trigger a redeployment of the latest settings. Use the following CLI command for that purpose:

`aws greengrass create-deployment --deployment-type Redeployment`

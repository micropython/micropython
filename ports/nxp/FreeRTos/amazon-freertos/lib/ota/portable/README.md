## Porting
In order to support Amazon FreeRTOS Over-the-Air Updates (OTA) on your microcontroller, it is necessary to implement the Portable Application Layer (PAL). This subdirectory consists of silicon vendor names that have successfully completed and tested at least one PAL. The PAL interface is defined in lib\include\private\aws_ota_pal.h (relative to the top of the repo).

PAL interface documentation is in https://docs.aws.amazon.com/freertos/latest/userguide/porting-ota-pal.html. However, please note that OTA is still in beta, so the definitive PAL interface reference is the header file. 

## Unit Testing for an OTA PAL

The first step is to complete the port of your PAL (portable layer; i.e., the interface defined by lib\include\private\aws_ota_pal.h) and confirm that it passes the OTA unit tests in AFQP (i.e., those under tests\common\ota).

## AWS IoT Device Setup

As a prerequisite to integration testing, you must configure your device as an AWS IoT "Thing". 

1. Follow these instructions, https://docs.aws.amazon.com/freertos/latest/userguide/freertos-prereqs.html. For an alternative description of getting started with AWS IoT, you can also use these steps https://docs.aws.amazon.com/iot/latest/developerguide/iot-console-signin.html, up to and including the web page entitled _Attach a Certificate to a Thing_.
2. Check your configuration using https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_windows.html and our _MQTT Hello World_ demo app.
Transfer your client configuration to your port and run “MQTT Hello World” from your board. 

## Integration Testing for an OTA PAL

1. Once you’ve confirmed that the previous step is working, open aws_demo_runner.c, comment out the two references to vStartMQTTEchoDemo(), and uncomment the two references to vStartOTAUpdateDemoTask(). 
Build and run vStartOTAUpdateDemoTask on your IoT Module board. The expected behavior of that app is to start the OTA Agent and then wait forever for a job notification from the cloud.
2. To create an OTA job, following these steps:
    1. https://docs.aws.amazon.com/freertos/latest/userguide/freertos-ota-dev.html
    2. https://docs.aws.amazon.com/freertos/latest/userguide/ota-prereqs.html
    3. https://docs.aws.amazon.com/freertos/latest/userguide/dg-ota-create-update.html
    4. https://docs.aws.amazon.com/freertos/latest/userguide/ota-console-workflow.html
3. After you publish the job created in the last step above, the OTA agent on the aboard should receive the JSON job document and immediately start downloading the new firmware block-by-block via MQTT. Errors encountered during that procedure are logged by the agent to the serial output from the device.


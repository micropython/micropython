# Amazon FreeRTOS Reference Bootloader ##
The reference bootloader demonstrates features for supporting OTA and crypto signature verification on boot.

**Note that currently the reference bootloader works on Microchip only.**

## Building the bootloader

* Install python 3 or higher  
  [Python Download](https://www.python.org/downloads/)  
  Check if you can run command: `python --version`, it should output 3.x.x
    
* Install pip if not already available on system  
   follow instructions at  https://pip.pypa.io/en/stable/installing/  

* Install pyopenssl by executing  
`pip install pyopenssl`

* Put the ECDSA code signer certificate in pem format in the path - 
    `\demos\common\ota\bootloader\utility\codesigner_cert_utility`  
    rename the cert to `aws_ota_codesigner_certificate.pem`  
    
* The codesigner_cert_utility.py runs as the pre build step for the bootloader 
project and extracts the public key in `aws_boot_codesigner_public_key.h` which
is then linked in the bootloader. 

## Building the OTA application image

* Update the OTA sequence number in file `\demos\common\ota\bootloader\utility\user-config\ota-descriptor.config`  
    The OTA sequence number should be incremented for every new OTA image generated as the bootloader will determine 
    the application to boot from this sequence number.
    
*  Update the application version number in `\demos\common\include\aws_application_version.h`  

*  The ota_image_generator.py utility runs as the post-build step and generates the final OTA binary file 
    that should be uploaded when creating OTA job. The name of the binary has the extension of `ota.bin`.
    ex - mplab.production.ota.bin

*  Build the aws_demos project. That project must build the bootloader first (as a loadable project), then the application, and finally generate 
   required OTA binaries.
   
*  For debug builds, running the factory image generation script is not required. For production builds, running the factory image generation script is necessary. In production, the factory image should be signed with the bootloader verifying its crypto signature. Please follow the instructions below to generate a unified hex file containing the crypto signature.
    
##  Building the factory image

* Install the SREC tool: 
  http://srecord.sourceforge.net/  
  Add the tools `srec_cat` and `srec_info` to the system PATH.

* Update the desired OTA sequence number and Application Version, as described in above steps.

* Build the aws_demos project and run the utility script `factory_image_generator.py ` under the **utility** folder to generate a factory image. 

```
python factory_image_generator.py -b mplab.production.bin -p MCHP-Curiosity-PIC32MZEF -k test_private_key.pem -x aws_bootloader.X.production.hex
```
    -mplab.production.bin is the application binary (this is not OTA binary)  
    -MCHP-Curiosity-PIC32MZEF is the platform name  
    -test_private_key.pem is the private key 
    -aws_bootloader.X.production.hex is the bootloader hex  

The unified hex file generated has the extension `factory.hex` for ex - mplab.production.factory.unified.hex

## Flashing the factory image

* Open MPLAB IPE and connect to the target curiosity board using an ICD4.

* Erase the device.

* Load the generated factory hex file i.e. mplab.production.factory.unified.hex.

* Click program and wait for programming to complete.

* Remove the ICD4 header; if everything is setup correctly you should see console logs for booting the image on first bank. 

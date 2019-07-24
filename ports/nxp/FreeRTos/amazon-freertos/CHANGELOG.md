# Change Log for Amazon FreeRTOS

## V1.4.6 12/27/2018
### New Features

#### New Board: Renesas Starter Kit+ for RX65N-2MB
The Renesas Starter Kit+ for RX65N-2MB is now qualified for Amazon FreeRTOS. This port updates the PKCS #11 portable layer, demo projects, and tests.

### Updates

#### FreeRTOS+POSIX V1.0.2
- Improvement to reduce the size of a pthread object and make the object user allocatable from stack.

#### FreeRTOS+TCP V2.0.10
- Add FreeRTOS+TCP support for the Renesas Starter Kit+ for RX65N-2MB.

#### FreeRTOS Kernel V10.0.1
- Add FreeRTOS Kernel support for the Renesas Starter Kit+ for RX65N-2MB.

#### PKCS #11 PAL for MT7697Hx-Dev-Kit V1.0.1
- Update license information.

#### PKCS #11 PAL for Renesas Starter Kit+ for RX65N-2MB V1.0.0
- Add PKCS #11 support for the Renesas Starter Kit+ for RX65N-2MB. 
- Note that the random number generation solution in this port is provided for demonstration purposes only. See the comment in lib/third_party/mcu_vendor/renesas/amazon_freertos_common/entropy_hardware_poll.c for more information.

#### Wi-Fi for MT7697Hx-Dev-Kit V1.0.1
- Update license information.

## V1.4.5 12/13/2018
### New Features

#### New Board: MediaTek MT7697Hx-Dev-Kit
The MediaTek MT7697 System on Chip (SoC) is now qualified for Amazon FreeRTOS. You can take advantage of Amazon FreeRTOS features and benefits using the MediaTek MT7697Hx Development Kit available from MediaTek Labs.  This development board contains the MT7697 SoC, which includes an Arm Cortex-M4 MCU, low power 1T1R 802.11 b/g/n Wi-Fi, Bluetooth 4.2 subsystem and power management unit.

#### lwIP Support
Amazon FreeRTOS support for the MediaTek MT7697Hx-Dev-Kit includes for the first time support for the Lightweight TCP / IP network stack (lwIP). This flexibility will support customer choice in identifying the best TCP stack solution for IoT devices.

### Updates

#### FreeRTOS+TCP V2.0.9
- Update to flush ARP cache when then network is down.

#### mbedTLS-based PKCS#11 V1.0.6
- Delete extra include headers.

#### PKCS #11 PAL for MT7697Hx-Dev-Kit V1.0.0
- Add PKCS #11 support for the MediaTek MT7697Hx-Dev-Kit.

#### Secure Sockets for FreeRTOS+TCP V1.1.5
- Update documentation.

#### Secure Sockets for lwIP V1.0.0
- Add Secure Sockets support for lwIP.

#### Wi-Fi for Infineon XMC4800 IoT Connectivity Kit V1.0.1
- Update documentation.

#### Wi-Fi for MT7697Hx-Dev-Kit V1.0.0
- Add Wi-Fi support for the MediaTek MT7697Hx-Dev-Kit.

## V1.4.4 11/19/2018
### Updates

#### Device Defender Demo V1.4.4
- Remove warnings in Device Defender Demo build.

#### Microchip "OTA Demo" Factory Image Script
- Fix post-build command and python script for generating OTA factory image for Mac users.

#### Device Defender Agent V1.0.2
- Update formatting and build warnings.

#### OTA PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.2
- Fix bug in retrieving code signature verification certificate.

#### OTA PAL for Curiosity PIC32MZEF V1.0.2
- Fix bug in retrieving code signature verification certificate.

## V1.4.3 11/07/2018
### New Features

#### New Board: Xilinx Zynq-7000 based MicroZed Industrial IoT Bundle
- Update Amazon FreeRTOS with port files, demo projects, and tests for the Xilinx Zynq-7000 based MicroZed Industrial IoT Bundle

### Updates

#### mbedTLS Library
- Upgrade to mbedTLS version 2.13.1.

#### FreeRTOS+POSIX V1.0.1
- Minor bug fixes.

#### FreeRTOS+TCP V2.0.8
- Update the Zynq-7000 portable layer for receive descriptor alignment.

#### PKCS #11 Updates
Update mbedTLS-based PKCS #11, and PKCS #11 PAL. These changes have been made to more closely align with the PKCS #11 standard, respond to feedback from users and partners, and make it easy for users to go to production from a prototype.
Applications calling into PKCS #11 functions directly (rather than indirectly via an Amazon provided secure sockets or TLS layer) may experience breaking changes. 

##### mbedTLS-based PKCS #11
- C_Initialize handles initialization of randomness in an effort to minimize entropy generation (or seed access) every time sessions are created and destroyed.  To protect random values, thread safety has been enabled in mbedTLS.
- C_SignInit and C_VerifyInit utilize the key handle that is passed in, rather than the first key found in order to comply with the PKCS #11 standard
- C_FindObject APIs no longer instantiate keys from the aws_clientcredential_keys.h header file if keys are not found. This removes the dependency of PKCS #11 on values that will be unique per-device (a transition step for enabling production-scale provisioning). Note that calling vDevModeKeyProvisioning() is now necessary to provision the device.
- C_FindObject PKCS #11 objects can be looked up by CKA_LABEL, in order to provide a standard-compliant object lookup.  Note that pkcs11configFILE_NAME_* configurations have been removed from aws_pkcs11_config.h, see aws_pkcs11.h for pkcs11configLABEL_* defines to access labels, and aws_pkcs11_pal.c for pkcs11palFILE_NAME_* defines.
- C_FindObject and C_GetAttributeValue accept different attribute arguments.
- C_CreateObject requires DER encoded certificates and keys instead of PEM formatted and object attributes required for creating objects have changed.  Note that vDevModeKeyProvisioning() has been updated to supply required attributes and convert inputs from DER to PEM if necessary.
- C_GenerateKeyPair now stores keys in non-volatile memory.
- C_Finalize is no longer invoked by helper functions to prevent threads from interfering with each other's PKCS #11 instances.
- Some error codes have been changes to better match the PKCS #11 standard.
- aws_tls.c and PKCS #11 AFQP tests have updates to reflect these changes.
    - mbedTLS-based PKCS #11 V1.0.5
    - TLS V1.1.3

##### PKCS #11 PAL for mbedTLS-based PKCS #11
- Breaking changes were made to PAL PKCS #11 functions in order to transition from file-centric API to object handle and object label based API.
    - PKCS #11 PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.1
    - PKCS #11 PAL for XMC4800 IoT Kit V1.0.1
    - PKCS #11 PAL for Curiosity PIC32MZEF V1.0.4
    - PKCS #11 PAL for LPC54018 IoT Module V1.0.3
    - PKCS #11 PAL for Windows Simulator V1.0.3
    - PKCS #11 PAL for STM32L4 Discovery kit IoT node V1.0.3
    - PKCS #11 PAL for Xilinx Zynq MicroZed V1.0.0 (new)

##### PKCS #11 for CC3220SF-LAUNCHXL
- Updates to match behavior of mbedTLS-based PKCS #11. 
- mbedTLS added to support conversion between DER and PEM objects.  Note that after provisioning the device, mbedTLS and provisiong PKCS #11 functions may be removed to reduce code size.
    - PKCS #11 PAL for CC3220SF-LAUNCHXL V1.0.3

##### OTA PAL Updates
- The OTA PALs for the Curiosity PIC32MZEF and ESP32-DevKitC ESP-WROVER-KIT boards have been modified to utilize PKCS #11 API to retrieve code signing keys, rather than calling into PKCS #11 PAL functions. 
    - OTA PAL for Curiosity PIC32MZEF V1.0.1
    - OTA PAL for ESP32-DevKitC ESP-WROVER-KIT V1.0.1

#### Secure Socket for FreeRTOS+TCP V1.1.4
- Minor update to handle PKCS #11 error codes.
- Update formatting.

#### Secure Sockets for Infineon XMC4800 IoT Connectivity Kit V1.0.1
- Fix the license header from Secure Socket to Secure Sockets.

#### Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 4
- Bug fix to support Amazon Trust Services endpoints. For more information, please see https://aws.amazon.com/blogs/iot/aws-iot-core-ats-endpoints/.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.5
- Remove duplicate file name definitions.  See aws_secure_sockets_config.h for file name defines.

#### Shadow V1.0.5
- Minor bug fixes.


## V1.4.2 10/17/2018
### New Features

#### New Board: Infineon XMC4800 IoT Connectivity Kit
Update Amazon FreeRTOS with port files, demo projects, and tests for the Infineon XMC4800 IoT Connectivity Kit.

### Updates

#### Update pthread Implementation in ESP-IDF
Incorporate an update to Espressif's ESP-IDF which improves the implementation of pthread.

#### Build Improvement: MPLAB PIC32 Projects
Resolve several warnings in the MPLAB project builds, and update the projects to no longer assume that the XC32 compiler is in the host computer's path.

#### Move Helper Utilities to the 'tools/' Directory
Move a few utilities to the root-level 'tools/' directory, from the 'tests/common/tools/' and 'demos/common/tools/' directories.

#### Styling Updates
Improve consistency of Hungarian Notation usage, update a number of methods to use 'void' instead of an empty argument list, and update the style guide.

#### New POSIX Functions
Add POSIX functions including `time`, `localtime_r`, and `strftime`.

#### Refactor Device Defender Direcory Structure
Update Device Defender code to use the same 'lib/' and 'include/' directory structures as the other libraries.

#### AFQP Documentation Updates
Update the Amazon FreeRTOS Qualification Program's documentation to reflect updated directory structures.

## V1.4.1 08/29/2018
### New Features
None

### Updates

#### OTA PAL for Windows Simulator V1.0.1
- Update Amazon FreeRTOS Windows Simulator to use ECDSA/SHA-256 for OTA image verification.


## V1.4.0 08/27/2018
### New Features

#### Demo Bootloader for Microchip Curiosity PIC32MZEF V1.0.0
The demo bootloader supports Amazon FreeRTOS over-the-air (OTA) update by implementing firmware version checking, cryptographic signature verification, and application self-test. The firmware verification includes verifying the authenticity and integrity of the new firmware received over the air. The bootloader verifies the cryptographic signature of the application before boot. The elliptic-curve digital signature algorithm (ECDSA) with SHA256 is used. The utilities provided can be used to generate a signed application that can be flashed on the device. This enables signature verification of the initial image.

### Updates

#### Over-the-air (OTA) updates
The over-the-air (OTA) updates feature of Amazon FreeRTOS is now generally available. The release includes enhancements to the OTA Agent and changes to the OTA Portable Abstraction Layer (PAL) interface.

#### OTA PAL for Curiosity PIC32MZEF V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for Windows Simulator V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for CC3220SF-LAUNCHXL V1.0.0
Update for API changes for OTA general availability release.

#### OTA PAL for Espressif ESP32 V1.0.0
Update for API changes for OTA general availability release.

#### OTA Agent V1.0.0
Enhancements and API changes for OTA general availability release.


## V1.3.2 08/21/2018
### New Features
None

### Updates

#### FreeRTOS+TCP 
- Multiple security improvements and fixes in packet parsing routines, DNS caching, and TCP sequence number and ID generation.
- Disable NBNS and LLMNR by default.
- Add TCP hang protection by default.

#### Secure Sockets for FreeRTOS+TCP 
- Improve security in Amazon FreeRTOS Secure Sockets usage of mbedTLS and ALPN.

We thank Ori Karliner of Zimperium zLabs Team for reporting these issues.


## V1.3.1 08/10/2018
### New Features
None

### Updates

#### OTA Beta Update
- Updates to OTA Beta to incorporate a new API for the OTA service. This API is not compatible with the  API used in the original OTA Beta released on Dec. 20th. 
- Add a "reference bootloader" for use in the OTA process. This bootloader is for use with the Microchip MCU.

#### Amazon FreeRTOS Qualification Program (AFQP) Update
- Update AFQP documentation. For more info on the changes to AFQP, reference the Revision History of the "Amazon FreeRTOS Qualification Program Developer Guide" in the "tests" directory.

#### Device Defender Update
- Add a demo to illustrate the operation of Device Defender for the Windows Simulator and Microchip PIC32MZEF MCU.

#### TI "Hello World" Build Failure Resolution
- Address an issue where the "Hello World" demo did not build with TI CCS 7.3 in AFR 1.3.0 when downloaded from OCW.


## V1.3.0 07/31/2018

### New Features

#### AFQP 1.0 Support
This release of AFR has support for vendors who wish to have their ports qualified for Amazon FreeRTOS through the Amazon FreeRTOS Qualification Program (AFQP). This is the first public release of AFQP tests and documentation. A new top level "tests" directory is added to support this functionality. AFQP documents are available in "tests" directory. [Learn more.](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-qualification-program.html)

#### Device Defender 1.0 Support
AWS IoT Device Defender is an AWS IoT security service that allows users to audit the configuration of their devices, monitor connected devices to detect abnormal behavior, and to mitigate security risks. It gives you the ability to enforce consistent security policies across your AWS IoT device fleet and respond quickly when devices are compromised. Device side support of this feature is part of this release. Devices supported are WinSim and Microchip PIC32MZEF. [Learn more.](https://docs.aws.amazon.com/freertos/latest/userguide/afr-device-defender-library.html)

#### FreeRTOS+POSIX 1.0 Interface
This release includes version 1.0.0 of FreeRTOS+POSIX.  FreeRTOS+POSIX is a POSIX compatibility layer that allows existing POSIX applications to run without modifications of FreeRTOS.  This release supports POSIX threads, mutexes, barriers, condition variables, semaphores, message queues, clocks, timers, and error numbers.  While most of the POSIX functions are implemented and up to specification, limitations in the FreeRTOS kernel precluded the standard implementations of certain functions.  The functions which differ from the POSIX specification are identified in their header files. Currently, FreeRTOS+POSIX is only used by drivers of the TI CC3220SF.

### Updates

#### FreeRTOS Kernel V10.0.1
- Add Idle tick counter interface
- Rename posix/ to FreeRTOS_POSIX/

#### FreeRTOS+TCP V2.0.4
- Fix issues raised by the Coverity scan
    - Fix a typo ulRxWindowLength -> ulTxWindowLength in FreeRTOS_Sockets.c
    - Fix strncmp length in FreeRTOS_DNS.c
    - Fix styling in FreeRTOS_ARP.c
- Fix a spelling typo ";east" -> "least" (response from a pull request)
- Add auto check of network interfaces for WinSim

#### MQTT Agent V1.1.2
- Move MQTT metrics to agent

#### mbedTLS-based PKCS #11 V1.0.3
- Reduce the number of warnings generated

#### PKCS #11 for LPC54018 IoT Module V1.0.1
- Change project baudrate setting to resolve AFQP test failures

#### Secure Sockets for NXP54018_IoT_Module V1.0.0 Beta 3
- Update to latest NXP driver to address AFQP 1.0 test failures

#### Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 2
- Update to new Inventek driver to resolve AFQP 1.0 test failures

#### Wi-Fi for Curiosity PIC32MZEF V1.0.3
- Change Microchip network param to use a direct address instead of a section to reduce the size of the binary image to allow OTA to continue working.
- Reduce number of warnings generated.

#### Wi-Fi for LPC54018 IoT Module V1.0.1
- Add fixes for Demo and DHCP.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.2
- Update for release of AFQP 1.0

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.2
- Update for release of AFQP 1.0

---------------------------------------------------------------------------------------

## V1.2.7 05/31/2018

- Update the Texas Instruments SimpleLink CC3220SF SDK from version 1.40.01.00 to version 2.10.00.04.
- Fix the MQTT Echo Demo (Hello World Demo) to avoid truncating received strings.
- Modify the Getting Started scripts to check if the AWS CLI is configured.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.4
- Update comments for SimpleLink CC3220SF SDK version 2.10.00.04.

--------------

## V1.2.6 05/18/2018

- Fix NXP MCUXpresso project build failure on Linux.

----------------

## V1.2.5 05/14/2018

- Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

#### FreeRTOS+TCP V2.0.4
 - Add Espressif ESP32 network interface support.

#### mbedTLS-based PKCS #11 V1.0.3
 - Implement C_DigestInit, C_DigestUpdate, and C_DigestFinal for SHA-256.
 - Implement C_GenerateKeyPair for non-persistent ECDSA P256.

#### PKCS #11 for ESP32-DevKitC ESP-WROVER-KIT V1.0.0
 - Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.2
 - Bug fix to ensure that WIFI_ConnectAP() switches to the network parameters input,
      even when already connected to a different set.

#### Wi-Fi for ESP32-DevKitC ESP-WROVER-KIT V1.0.0
 - Add support for Espressif's ESP32-DevKitC and ESP-WROVER-KIT.

---------------------------------------------------------------------------------------

## V1.2.4 05/01/2018

 - Upgrade to mbedTLS version 2.8.
 - Add MCUXpresso IDE demo project for the NXP LPC54018 IoT Module.

#### Crypto V1.0.2
 - Minor updates due to mbedTLS crypto interface changes.

#### FreeRTOS+TCP V2.0.3
 - Fix a bug where re-transmission and duplicated TCP packets would create a
      computation underflow as well as a memory leak.
 - Add new public function FreeRTOS_UpdateMACAddress() to allow changing the MAC address
      after FreeRTOS_IPInit. Sometimes the device MAC address is not available at the
      time FreeRTOS_IPInit() is called, so it needs to be changed afterward.
 - Remove non-cryptographic rand() implementation.
 - Remove a static variable in functions prvGetHostByName() and prvCreateDNSSocket() to
      make them threadsafe.

#### Greengrass Discovery V1.0.3
 - Provide a helpful error message if the Greengrass Discovery JSON does not fit in
      the supplied buffer.

#### MQTT Agent V1.1.2
 - Bug fix to avoid socket leak if MQTT Connect fails after a successful TCP
      connection.
 - Add support for disabling subscription management feature by defining the macro
      mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT as 0.

#### OTA PAL for Curiosity PIC32MZEF V0.9.1
 - Update for PKCS #11 PAL layer API changes.

#### OTA PAL for Windows Simulator V0.9.2
- Minor restructuring of file locations.

#### OTA PAL for CC3220SF-LAUNCHXL V0.9.3
 - Minor changes to enable test integration.

#### OTA Agent V0.9.4
 - Minor restructuring of file locations.

#### mbedTLS-based PKCS #11 V1.0.2
 - Combine the mbedTLS based PKCS #11 implementation from Curiosity PIC32MZEF,
      LPC54018 IoT Module, Windows Simulator, and STM32L4 Discovery kit IoT node into a
      single file.
 - Add support for public key verification of signatures.
 - Fix to free context structures on session failure.
 - Update C_OpenSession to use CKF_SERIAL_SESSION flag.

#### PKCS #11 for Curiosity PIC32MZEF V1.0.2
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS #11 for LPC54018 IoT Module V1.0.1
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS #11 PAL for Windows Simulator V1.0.2
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS #11 for STM32L4 Discovery kit IoT node V1.0.1
 - Create port specific functions for certificate and key access:
      PKCS11_PAL_SaveFile(), PKCS11_PAL_ReadFile(), PKCS11_PAL_ReleaseFileData().

#### PKCS #11 for CC3220SF-LAUNCHXL V1.0.2
 - PKCS #11 implementation for TI based on mbedTLS moved into this file.

#### Secure Socket for FreeRTOS+TCP V1.1.2
 - Combine Secure Sockets implementation for Curiosity PIC32MZEF and Windows
      Simulator into a single file.
 - Fix return value of SOCKETS_Socket on error.
 - Attempting to create an unsupported UDP socket now triggers an assert.
 - Add cryptographic random number generator function for TCP sequence numbers.
 - Update the Socket structure to keep track of a connection attempt and added
      support of the ECONN error.

#### Secure Sockets for LPC54018 IoT Module V1.0.0 Beta 3
 - Fix minor bug in SOCKETS_Recv().

#### Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 2
 - Fix return value of SOCKETS_Close on error.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.3
 - Secure sockets printing is now controlled independently using the SOCKETS_PRINT
      macro. SOCKETS_PRINT prints TI driver error codes.

#### Shadow V1.0.3
 - Change names of configuration constants to be consistent with FreeRTOS style.

#### TLS V1.1.1
 - Support AWS IoT Just-in-Time Registration (JITR) by sending optional
      client-issuer certificate.
 - Use CKF_SERIAL_SESSION flag with PKCS #11.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.3
 - Update for setting the MAC Address in WIFI_On() by using new FreeRTOS+TCP
      function FreeRTOS_UpdateMACAddress().
 - Redefine printing and assert stubs used in the Wi-Fi driver code.
 - Add implementation of WIFI_GetMAC().
 - Add implementation of WIFI_IsConnected().
 - Minor bug fixes.

#### Wi-Fi for LPC54018 IoT Module V1.0.2
 - Add implementation of WIFI_IsConnected().
 - Fix max DNS name length redefinition.
 - Fix compiler errors in MCUXpresso IDE.
 - Minor bug fixes.

#### Wi-Fi STM32L4 Discovery kit IoT node V1.0.1
 - Add implementation of WIFI_IsConnected().
 - Add NULL pointer checks throughout.
 - Minor bug fixes.

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.2
 - Add implementation of WIFI_IsConnected().
 - Add NULL pointer checks throughout.
 - Minor bug fixes.

---------------------------------------------------------------------------------------

## V1.2.3 03/29/2018
 - Fix TI IAR project build failure.

---------------------------------------------------------------------------------------

## V1.2.2 02/27/2018

#### OTA Agent V0.9.3
 - Formatting update.

#### OTA PAL for Curiosity PIC32MZEF V0.9.0
 - Beta release of the OTA Update support for the Microchip Curiosity PIC32MZEF.

#### PKCS #11 for Curiosity_PIC32MZEF V1.0.1
 - Add support for the management of OTA update code signing keys.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.1
 - Update to conditionally compile the entire file.

---------------------------------------------------------------------------------------

## V1.2.1 02/23/2018

 - Add an IAR IDE demo project for the Texas Instruments CC3220SF-LAUNCHXL.
 - Add Wi-Fi support for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS+TCP V2.0.2
 - Improve NULL variable check in buffer management.

#### MQTT Agent V1.1.1
 - Minor bug fix checking for a NULL pointer.

#### OTA Agent V0.9.2
 - Update to support NULL OTA file handles.

#### Amazon FreeRTOS OTA PAL for CC3220SF-LAUNCHXL V0.9.2
 - Update to support NULL OTA file handles.

#### PKCS #11 for CC3220SF-LAUNCHXL V1.0.1
 - Add a dummy variable to a previously empty structure to fix IAR compiler errors.

#### Secure Socket for Windows Simulator V1.1.1
 - Formatting update.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.2
 - Update to print SimpleLink driver-specific error codes when debugging.
 - Add error handling for non-blocking sockets.
 - Update socket option to return an error if security options are specified after
      a connection.

#### Wi-Fi for Curiosity PIC32MZEF V1.0.1
 - Update such that Wi-Fi disconnection waits until the link is down before
      returning.

#### Wi-Fi for CC3220SF-LAUNCHXL V1.0.1
 - Fix error in attempting to overwrite a const memory.

---------------------------------------------------------------------------------------

## V1.2.0 02/06/2018

#### Greengrass Discovery V1.0.2
 - Update to send all data until an error is received.

#### MQTT Agent V1.1.0
 - Add support for ALPN. ALPN allows MQTT traffic to be sent to
      the AWS IoT broker over port 443 instead of 8883.

#### OTA Agent V0.9.1
 - Send a FAILED status from agent when a file too large for the platform
      is received.
 - Rename some files.

#### PKCS #11 for Windows Simulator
 - Add developer mode key provisioning support.

#### Secure Socket for Curiosity PIC32MZEF V1.0.1
 - Add support for ALPN.

#### Secure Socket for Windows Simulator V1.1.0
 - Add support for ALPN.

#### Secure Sockets for CC3220SF-LAUNCHXL V1.0.1
- Remove unnecessary server certificate storage on the client side.
- Remove unnecessary global synchronization semaphore.
- Update for other small bugs.

#### Shadow V1.0.2
- Fix error handling bugs.
- Require client tokens.
- Update for other small bugs.

#### TLS V1.1.0
- Add support for ALPN.

---------------------------------------

## V1.1.0 12/20/2017

#### Crypto V1.0.1
- Fix compiler warning for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS+TCP V2.0.1
- Add support for the Microchip Curiosity PIC32MZEF.

#### FreeRTOS Kernel V10.0.1
- Minor bug fixes to allow Linux and C++ builds.

#### Greengrass Discovery V1.0.1
- Reformat console display messages in order to better facilitate demos and debugging.

#### MQTT Agent V1.0.1
- The MQTT Agent now processes commands between successive socket reads to       enable faster command handling, especially when the connected socket is       receiving data continuously.

#### OTA Agent V0.9.0
- Beta release of OTA Update library for Amazon FreeRTOS. Includes support   for the Texas Instruments CC3220SF-LAUNCHXL and Windows Simulator.

#### PKCS #11 for Curiosity PIC32MZEF V1.0.0 Beta 1
- Add support for the Microchip Curiosity PIC32MZEF.

#### Secure Socket for Curiosity PIC32MZEF V1.0.0
- Add support for the Microchip Curiosity PIC32MZEF.

#### Secure Sockets for LPC54018 IoT Module V1.0.0 Beta 2
- Fix bugs in the Secure Sockets implementation for the NXP LPC54018 IoT       Module.

#### Shadow V1.0.1
- Fix compiler warning for the Microchip Curiosity PIC32MZEF.

#### Wi-Fi for LPC54018 IoT Module V1.0.1
- Change the Wi-Fi Connection timeout to 10 seconds.

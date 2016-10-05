1 - Introduction
----------------

The Atmel® Software Framework (ASF) is a collection of embedded software for Atmel flash MCU (www.atmel.com/asf).
» It simplifies the usage of microcontrollers, providing an abstraction to the hardware and high-value middlewares
» ASF is designed to be used for evaluation, prototyping, design and production phases
» ASF is integrated in the Atmel Studio IDE with a graphical user interface or available as standalone for GCC, IAR compilers
» ASF can be downloaded for free

ASF online documentation can be found here: http://asf.atmel.com

Supported devices:
» AVR UC3(Trade Mark) (http://www.atmel.com/products/microcontrollers/avr/32-bitavruc3.aspx)
» AVR XMEGA(Trade Mark) (http://www.atmel.com/products/microcontrollers/avr/avr_xmega.aspx) 
» megaAVR(Trade Mark) (http://www.atmel.com/products/microcontrollers/avr/megaavr.aspx) 
» SAM (http://www.atmel.com/products/microcontrollers/arm/default.aspx)

Each software module is provided with full source code, example of usage and
ready-to-use projects for the IAR EW and GNU GCC compilers.

This package is dedicated for IAR and GCC makefile users. AVR Studio 5 or Atmel Studio 6 users do not
need this package as the ASF is integrated in Atmel Studio.

The top folder organization is as follow:
- the avr32/ folder contains software modules(source code and projects) dedicated to AVR UC3 devices,
- the mega/ folder contains software modules(source code and projects) dedicated to megaAVR devices,
- the xmega/ folder contains software modules(source code and projects) dedicated to AVR XMEGA devices,
- the sam/ folder contains software modules(source code and projects) dedicated to SAM devices,
- the common/ folder contains software modules(source code and projects) shared by all AVR and SAM devices,
- the thirdparty/ folder contains software modules(source code and projects) from thirdparty providers for all Atmel devices.

The thirdparty/ folder is organized by thirdparty software module (i.e. one folder per thirdparty software module).

The avr32/, xmega/, mega/, sam/ and common/ folders are organized as follow:
- the drivers/ folder contains low-level software drivers for MCU on-chip resources (eg cpu, usart, adc drivers).
- the boards/ folder contains board-specific source code files.
- the utils/ folder contains files that are used by all other modules: it holds
several linker script files, IAR & GCC pre-compiled libraries of some ASF modules,
and C/C++ utility files with general usage defines, macros and functions.
- the services/ folder contains application-oriented pieces of software that are
not specific to boards nor chips (eg. FAT, TCP/IP stack, os, JPEG decoder).
For the common/ top folder, the services/ folder also contains chip-specific code.
- the components/ folder offers, for each supported hardware component, a software
interface to interact with the component (eg memories like SDRAM, SD card, or display)
- the applications/ contains hefty examples of applications using services and drivers.


2 - Documentation
--------------------------

ASF release notes, reference manual and getting started documents can be found on http://www.atmel.com/asf.

The ASF documentation can be found online on http://asf.atmel.com.


3 - Toolchain Header Files Update
---------------------------------

For 8-bit AVR users it is recommended to update the toolchain header files, a
description about how to do that is located in the readme.txt file under the
xmega/utils/header_files/ directory.

For AVR UC3 users using IAR, it is recommended to update the IAR header files.
To do that, unzip the avr32-headers.zip file (located under
the avr32/utils/header_files/ directory) to the IAR EWAVR32 installation folder
under "IAR installation folder"/Embedded Workbench x.y/avr32/inc/.


4 - IAR EW Users
----------------

Using an example of usage of the AVR UC3 GPIO driver module as an example, the
IAR projects are located under:
- avr32/drivers/gpio/peripheral_bus_example/at32uc3a0512_evk1000/iar/ for an EVK1100 board,
- avr32/drivers/gpio/peripheral_bus_example/at32uc3a0512_evk1105/iar/ for an EVK1105 board,
- avr32/drivers/gpio/peripheral_bus_example/at32uc3b0256_evk1101/iar/ for an EVK1101 board,
- avr32/drivers/gpio/peripheral_bus_example/at32uc3a3256_evk1104/iar/ for an EVK1104 board,
- avr32/drivers/gpio/peripheral_bus_example/at32uc3l064_stk600-rcuc3l0/iar/ for an STK600-RCUC3L0 setup,
- avr32/drivers/gpio/peripheral_bus_example/at32uc3c0512c_uc3c_ek/iar/ for an AT32UC3C-EK board,
- avr32/drivers/gpio/peripheral_bus_example/at32uc3l064_uc3l_ek/iar/ for an AT32UC3L-EK board.

Each iar folder contains a full IAR project with:
- an IAR EWAVR32 workspace file(avr32_drivers_gpio_peripheral_bus_example_uc3l_ek.eww: double-click on this file to open the whole project),
- an IAR EWAVR32 project file(avr32_drivers_gpio_peripheral_bus_example_uc3l_ek.ewp),
- an IAR EWAVR32 debug configuration file(avr32_drivers_gpio_peripheral_bus_example_uc3l_ek.ewd).


5 - AVR32 Studio Users
----------------------

It is possible to work with an unzipped ASF package from within AVR32 Studio: this
is described in the application note "AVR32769: How to Compile the standalone AVR
UC3 Software Framework in AVR32 Studio V2": http://www.atmel.com/dyn/resources/prod_documents/doc32115.pdf


6 - Offline documentation - Generating The HTML Documentation
-------------------------------------

All modules are fully documented using doxygen tags. Each project within the ASF
contains a doxyfile.doxygen (used to configure doxygen for a proper documentation
generation): to generate the html documentation, doxygen must be installed (see
http://www.doxygen.org/download.html) and the doxyfile.doxygen must be used as
the input configuration file for doxygen.

Using an example of usage of the AVR UC3 GPIO driver module as an example, for
an AT32UC3C-EK board, the associated doxyfile.doxygen file is under the
avr32/drivers/gpio/peripheral_bus_example/at32uc3c0512c_uc3c_ek/doxygen/ folder.
Run doxygen and use this doxyfile.doxygen as configuration file for doxygen.
Using the command line, this is done with the following command: doxygen doxyfile.doxygen


7 - Contact Information
-----------------------

For more info about Atmel MCU visit http://www.atmel.com/products/microcontrollers/default.aspx or contact support through the http://support.atmel.no/ site.
The support site also have a Frequently Asked Questions page with the most common questions.

ASF bugs or enhancement requests can be reported in the ASF Bug Tracker (http://asf.atmel.com/bugzilla/).


8 - Community Information
-------------------------

Users can find an ASF forum (http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewforum&f=21) on AVRfreaks for AVR users or on http://www.at91.com/ for SAM users.
This forum can be used to have an open discussion about usage, development, bugs, fixes, improvements, etc.
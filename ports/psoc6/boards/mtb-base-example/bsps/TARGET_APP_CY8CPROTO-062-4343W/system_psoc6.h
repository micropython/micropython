/***************************************************************************//**
* \file system_psoc6.h
* \version 2.95.1
*
* \brief Device system header file.
*
********************************************************************************
* \copyright
* Copyright 2016-2021 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/


#ifndef _SYSTEM_PSOC6_H_
#define _SYSTEM_PSOC6_H_

/**
* \addtogroup group_system_config_cm4
* \{
* Provides device startup, system configuration, and linker script files.
* The system startup provides the followings features:
* - See \ref group_system_config_device_initialization for the:
*   * \ref group_system_config_dual_core_device_initialization
*   * \ref group_system_config_single_core_device_initialization
* - \ref group_system_config_device_memory_definition
* - \ref group_system_config_heap_stack_config
* - \ref group_system_config_default_handlers
* - \ref group_system_config_device_vector_table
* - \ref group_system_config_cm4_functions
*
* \section group_system_config_configuration Configuration Considerations
*
* \subsection group_system_config_device_memory_definition Device Memory Definition
* The flash and RAM allocation for each CPU is defined by the linker scripts.
* For dual-core devices, the physical flash and RAM memory is shared between the CPU cores.
* 2 KB of RAM (allocated at the end of RAM) are reserved for system use.
* For Single-Core devices the system reserves additional 80 bytes of RAM.
* Using the reserved memory area for other purposes will lead to unexpected behavior.
*
* \note The linker files provided with the PDL are generic and handle all common
* use cases. Your project may not use every section defined in the linker files.
* In that case you may see warnings during the build process. To eliminate build
* warnings in your project, you can simply comment out or remove the relevant
* code in the linker file.
*
* \note For the PSoC 64 Secure MCUs devices, refer to the following page:
* https://www.cypress.com/documentation/software-and-drivers/psoc-64-secure-mcu-secure-boot-sdk-user-guide
*
*
* <b>ARM GCC</b>\n
* The flash and RAM sections for the CPU are defined in the linker files:
* 'xx_yy.ld', where 'xx' is the device group, and 'yy' is the target CPU; for example,
* 'cy8c6xx7_cm0plus.ld' and 'cy8c6xx7_cm4_dual.ld'.
* \note If the start of the Cortex-M4 application image is changed, the value
* of the \ref CY_CORTEX_M4_APPL_ADDR should also be changed. The
* \ref CY_CORTEX_M4_APPL_ADDR macro should be used as the parameter for the
* Cy_SysEnableCM4() function call.
* By default, the COMPONENT_CM0P_SLEEP prebuilt image is used for the CM0p core.
* More about CM0+ prebuilt images, see here:
* https://github.com/cypresssemiconductorco/psoc6cm0p
*
* Change the flash and RAM sizes by editing the macros value in the
* linker files for both CPUs:
* - 'xx_cm0plus.ld', where 'xx' is the device group:
* \code
* flash       (rx)  : ORIGIN = 0x10000000, LENGTH = 0x2000
* ram         (rwx) : ORIGIN = 0x08000000, LENGTH = 0x2000
* \endcode
* - 'xx_cm4_dual.ld', where 'xx' is the device group:
* \code
* flash       (rx)  : ORIGIN = 0x10000000, LENGTH = 0x100000
* ram         (rwx) : ORIGIN = 0x08002000, LENGTH = 0x45800
* \endcode
*
* Change the value of the \ref CY_CORTEX_M4_APPL_ADDR macro to the ROM ORIGIN's
* value (0x10000000) + FLASH_CM0P_SIZE value (0x2000, the size of a flash image
* of the Cortex-M0+ application should be the same value as the flash LENGTH in
* 'xx_cm0plus.ld') in the 'xx_cm4_dual.ld' file, where 'xx' is the device group.
* Do this by either:
* - Passing the following commands to the compiler:\n
* \code -D CY_CORTEX_M4_APPL_ADDR=0x10002000 \endcode
* or
* - Editing the \ref CY_CORTEX_M4_APPL_ADDR value in the 'system_xx.h', where
* 'xx' is the device family:\n
* \code #define CY_CORTEX_M4_APPL_ADDR (0x10002000u) \endcode
*
* <b>ARM Compiler</b>\n
* The flash and RAM sections for the CPU are defined in the linker files:
* 'xx_yy.sct', where 'xx' is the device group, and 'yy' is the target CPU; for
* example 'cy8c6xx7_cm0plus.sct' and 'cy8c6xx7_cm4_dual.sct'.
* \note If the start of the Cortex-M4 application image is changed, the value
* of the of the \ref CY_CORTEX_M4_APPL_ADDR should also be changed. The
* \ref CY_CORTEX_M4_APPL_ADDR macro should be used as the parameter for the \ref
* Cy_SysEnableCM4() function call.
* By default, the COMPONENT_CM0P_SLEEP prebuilt image is used for the CM0p core.
* More about CM0+ prebuilt images, see here:
* https://github.com/cypresssemiconductorco/psoc6cm0p
*
* \note The linker files provided with the PDL are generic and handle all common
* use cases. Your project may not use every section defined in the linker files.
* In that case you may see the warnings during the build process:
* L6314W (no section matches pattern) and/or L6329W
* (pattern only matches removed unused sections). In your project, you can
* suppress the warning by passing the "--diag_suppress=L6314W,L6329W" option to
* the linker. You can also comment out or remove the relevant code in the linker
* file.
*
* Change the flash and RAM sizes by editing the macros value in the
* linker files for both CPUs:
* - 'xx_cm0plus.sct', where 'xx' is the device group:
* \code
* #define FLASH_START 0x10000000
* #define FLASH_SIZE  0x00002000
* #define RAM_START   0x08000000
* #define RAM_SIZE    0x00002000
* \endcode
* - 'xx_cm4_dual.sct', where 'xx' is the device group:
* \code
* #define FLASH_START 0x10000000
* #define FLASH_SIZE  0x00100000
* #define RAM_START   0x08002000
* #define RAM_SIZE    0x00045800
* \endcode
*
* Change the value of the \ref CY_CORTEX_M4_APPL_ADDR macro to the FLASH_START
* value (0x10000000) + FLASH_CM0P_SIZE value (0x2000, the size of a flash image
* of the Cortex-M0+ application should be the same value as the FLASH_SIZE in the
* 'xx_cm0plus.sct') in the 'xx_cm4_dual.sct' file, where 'xx' is the device group.
* Do this by either:
* - Passing the following commands to the compiler:\n
* \code -D CY_CORTEX_M4_APPL_ADDR=0x10002000 \endcode
* or
* - Editing the \ref CY_CORTEX_M4_APPL_ADDR value in the 'system_xx.h', where
* 'xx' is the device family:\n
* \code #define CY_CORTEX_M4_APPL_ADDR (0x10002000u) \endcode
*
* <b>IAR</b>\n
* The flash and RAM sections for the CPU are defined in the linker files:
* 'xx_yy.icf', where 'xx' is the device group, and 'yy' is the target CPU; for example,
* 'cy8c6xx7_cm0plus.icf' and 'cy8c6xx7_cm4_dual.icf'.
* \note If the start of the Cortex-M4 application image is changed, the value
* of the of the \ref CY_CORTEX_M4_APPL_ADDR should also be changed. The
* \ref CY_CORTEX_M4_APPL_ADDR macro should be used as the parameter for the \ref
* Cy_SysEnableCM4() function call.
* By default, the COMPONENT_CM0P_SLEEP prebuilt image is used for the CM0p core.
* More about CM0+ prebuilt images, see here:
* https://github.com/cypresssemiconductorco/psoc6cm0p
*
* Change the flash and RAM sizes by editing the macros value in the
* linker files for both CPUs:
* - 'xx_cm0plus.icf', where 'xx' is the device group:
* \code
* define symbol __ICFEDIT_region_IROM1_start__ = 0x10000000;
* define symbol __ICFEDIT_region_IROM1_end__   = 0x10001FFF;
* define symbol __ICFEDIT_region_IRAM1_start__ = 0x08000000;
* define symbol __ICFEDIT_region_IRAM1_end__   = 0x08001FFF;
* \endcode
* - 'xx_cm4_dual.icf', where 'xx' is the device group:
* \code
* define symbol __ICFEDIT_region_IROM1_start__ = 0x10000000;
* define symbol __ICFEDIT_region_IROM1_end__   = 0x100FFFFF;
* define symbol __ICFEDIT_region_IRAM1_start__ = 0x08002000;
* define symbol __ICFEDIT_region_IRAM1_end__   = 0x080477FF;
* \endcode
*
* Change the value of the \ref CY_CORTEX_M4_APPL_ADDR macro to the
* __ICFEDIT_region_IROM1_start__ value (0x10000000) + FLASH_CM0P_SIZE value
* (0x2000, the size of a flash image of the Cortex-M0+ application) in the
* 'xx_cm4_dual.icf' file, where 'xx' is the device group. The sum result
* should be the same as (__ICFEDIT_region_IROM1_end__ + 1) value in the
* 'xx_cm0plus.icf'. Do this by either:
* - Passing the following commands to the compiler:\n
* \code -D CY_CORTEX_M4_APPL_ADDR=0x10002000 \endcode
* or
* - Editing the \ref CY_CORTEX_M4_APPL_ADDR value in the 'system_xx.h', where
* 'xx' is the device family:\n
* \code #define CY_CORTEX_M4_APPL_ADDR (0x10002000u) \endcode
*
* \subsection group_system_config_device_initialization Device Initialization
* After a power-on-reset (POR), the boot process is handled by the boot code
* from the on-chip ROM that is always executed by the Cortex-M0+ core. The boot
* code passes the control to the Cortex-M0+ startup code located in flash.
*
* \subsubsection group_system_config_dual_core_device_initialization Dual-Core Devices
* The Cortex-M0+ startup code performs the device initialization by a call to
* SystemInit() and then calls the main() function. The Cortex-M4 core is disabled
* by default. Enable the core using the \ref Cy_SysEnableCM4() function.
* See \ref group_system_config_cm4_functions for more details.
* \note Startup code executes SystemInit() function for the both Cortex-M0+ and Cortex-M4 cores.
* The function has a separate implementation on each core.
* Both function implementations unlock and disable the WDT.
* Therefore enable the WDT after both cores have been initialized.
*
* \subsubsection group_system_config_single_core_device_initialization Single-Core Devices
* The Cortex-M0+ core is not user-accessible on these devices. In this case the
* Flash Boot handles setup of the CM0+ core and starts the Cortex-M4 core.
*
* \subsection group_system_config_heap_stack_config Heap and Stack Configuration
* There are two ways to adjust heap and stack configurations:
* -# Editing source code files
* -# Specifying via command line
*
* By default, the stack size is set to 0x00001000 and the heap size is allocated
* dynamically to the whole available free memory up to stack memory and it
* is set to the 0x00000400 (for ARM GCC and IAR compilers) as minimal value.
*
* \subsubsection group_system_config_heap_stack_config_gcc ARM GCC
* - <b>Editing source code files</b>\n
* The heap and stack sizes are defined in the assembler startup files
* (e.g. startup_psoc6_01_cm0plus.S and startup_psoc6_01_cm4.S).
* Change the heap and stack sizes by modifying the following lines:\n
* \code .equ  Stack_Size, 0x00001000 \endcode
* \code .equ  Heap_Size,  0x00000400 \endcode
* Also, the stack size is defined in the linker script files: 'xx_yy.ld',
* where 'xx' is the device family, and 'yy' is the target CPU; for example,
* cy8c6xx7_cm0plus.ld and cy8c6xx7_cm4_dual.ld.
* Change the stack size by modifying the following line:\n
* \code STACK_SIZE = 0x1000; \endcode
*
* \note Correct operation of malloc and related functions depends on the working
* implementation of the 'sbrk' function. Newlib-nano (default C runtime library
* used by the GNU Arm Embedded toolchain) provides weak 'sbrk' implementation that
* doesn't check for heap and stack collisions during excessive memory allocations.
* To ensure the heap always remains within the range defined by __HeapBase and
* __HeapLimit linker symbols, provide a strong override for the 'sbrk' function:
* \snippet startup/snippet/main.c snippet_sbrk
* For FreeRTOS-enabled multi-threaded applications, it is sufficient to include
* clib-support library that provides newlib-compatible implementations of
* 'sbrk', '__malloc_lock' and '__malloc_unlock':
* <br>
* https://github.com/cypresssemiconductorco/clib-support.
*
* \subsubsection group_system_config_heap_stack_config_mdk ARM Compiler
* - <b>Editing source code files</b>\n
* The stack size is defined in the linker script files: 'xx_yy.sct',
* where 'xx' is the device family, and 'yy' is the target CPU; for example,
* cy8c6xx7_cm0plus.sct and cy8c6xx7_cm4_dual.sct.
* Change the stack size by modifying the following line:\n
* \code STACK_SIZE = 0x1000; \endcode
*
* \subsubsection group_system_config_heap_stack_config_iar IAR
* - <b>Editing source code files</b>\n
* The heap and stack sizes are defined in the linker script files: 'xx_yy.icf',
* where 'xx' is the device family, and 'yy' is the target CPU; for example,
* cy8c6xx7_cm0plus.icf and cy8c6xx7_cm4_dual.icf.
* Change the heap and stack sizes by modifying the following lines:\n
* \code Stack_Size      EQU     0x00001000 \endcode
* \code Heap_Size       EQU     0x00000400 \endcode
*
* - <b>Specifying via command line</b>\n
* Change the heap and stack sizes passing the following commands to the
* linker (including quotation marks):\n
* \code --define_symbol __STACK_SIZE=0x000000400 \endcode
* \code --define_symbol __HEAP_SIZE=0x000000100 \endcode
*
* \subsection group_system_config_default_handlers Default Interrupt Handlers Definition
* The default interrupt handler functions are defined as weak functions to a dummy
* handler in the startup file. The naming convention for the interrupt handler names
* is \<interrupt_name\>_IRQHandler. A default interrupt handler can be overwritten in
* user code by defining the handler function using the same name. For example:
* \code
* void scb_0_interrupt_IRQHandler(void)
*{
*    ...
*}
* \endcode
*
* \subsection group_system_config_device_vector_table Vectors Table Copy from Flash to RAM
* This process uses memory sections defined in the linker script. The startup
* code actually defines the contents of the vector table and performs the copy.
* \subsubsection group_system_config_device_vector_table_gcc ARM GCC
* The linker script file is 'xx_yy.ld', where 'xx' is the device family, and
* 'yy' is the target CPU; for example, cy8c6xx7_cm0plus.ld and cy8c6xx7_cm4_dual.ld.
* It defines sections and locations in memory.\n
*       Copy interrupt vectors from flash to RAM: \n
*       From: \code LONG (__Vectors) \endcode
*       To:   \code LONG (__ram_vectors_start__) \endcode
*       Size: \code LONG (__Vectors_End - __Vectors) \endcode
* The vector table address (and the vector table itself) are defined in the
* assembler startup files (e.g. startup_psoc6_01_cm0plus.S and startup_psoc6_01_cm4.S).
* The code in these files copies the vector table from Flash to RAM.
* \subsubsection group_system_config_device_vector_table_mdk ARM Compiler
* The linker script file is 'xx_yy.sct', where 'xx' is the device family,
* and 'yy' is the target CPU; for example, cy8c6xx7_cm0plus.sct and
* cy8c6xx7_cm4_dual.sct. The linker script specifies that the vector table
* (RESET_RAM) shall be first in the RAM section.\n
* RESET_RAM represents the vector table. It is defined in the assembler startup
* files (e.g. startup_psoc6_01_cm0plus.s and startup_psoc6_01_cm4.s).
* The code in these files copies the vector table from Flash to RAM.
*
* \subsubsection group_system_config_device_vector_table_iar IAR
* The linker script file is 'xx_yy.icf', where 'xx' is the device family, and
* 'yy' is the target CPU; for example, cy8c6xx7_cm0plus.icf and cy8c6xx7_cm4_dual.icf.
* This file defines the .intvec_ram section and its location.
* \code place at start of IRAM1_region  { readwrite section .intvec_ram}; \endcode
* The vector table address (and the vector table itself) are defined in the
* assembler startup files (e.g. startup_psoc6_01_cm0plus.s and startup_psoc6_01_cm4.s).
* The code in these files copies the vector table from Flash to RAM.
*
* \section group_system_config_MISRA MISRA Compliance
*
* <table class="doxtable">
*   <tr>
*     <th>MISRA Rule</th>
*     <th>Rule Class (Required/Advisory)</th>
*     <th>Rule Description</th>
*     <th>Description of Deviation(s)</th>
*   </tr>
*   <tr>
*     <td>2.3</td>
*     <td>R</td>
*     <td>The character sequence // shall not be used within a comment.</td>
*     <td>The comments provide a useful WEB link to the documentation.</td>
*   </tr>
* </table>
*
* \section group_system_config_changelog Changelog
*   <table class="doxtable">
*   <tr>
*       <th>Version</th>
*       <th>Changes</th>
*       <th>Reason for Change</th>
*   </tr>
*   <tr>
*       <td rowspan="1">2.95.1</td>
*       <td>Restructured documentation.</td>
*       <td>Documentation update.</td>
*   </tr>
*   <tr>
*       <td rowspan="1">2.95</td>
*       <td>Update FPU enable function with CMSIS macros to disable/enable interrupts</td>
*       <td>Move to stadnard inline CMSIS ARM macros</td>
*   </tr>
*   <tr>
*       <td rowspan="2">2.91</td>
*       <td>Updated memory configuration for PSoC 64 devices.</td>
*       <td>Flash and RAM memory allocation updated.</td>
*   </tr>
*   <tr>
*       <td>Added cys06xxa_cm4 linker scripts.</td>
*       <td>New device support.</td>
*   </tr>
*   <tr>
*       <td rowspan="4">2.90.1</td>
*       <td>Updated \ref group_system_config_heap_stack_config_gcc section with the note
*           on the dynamic memory allocation for ARM GCC.</td>
*       <td>Documentation update.</td>
*   </tr>
*   <tr>
*       <td>Updated system_psoc6.h to include custom CY_SYSTEM_PSOC6_CONFIG passed as compiler macro.</td>
*       <td>Improve configuration flexibility.</td>
*   </tr>
*   <tr>
*       <td>Updated attribute usage for the linker section placement in CM0+ startup code</td>
*       <td>Enhancement based on usability feedback.</td>
*   </tr>
*   <tr>
*       <td>Renamed the '.cy_xip' linker script region as 'cy_xip'</td>
*       <td>Enable access to the XIP region start/end addresses from the C code.</td>
*   </tr>
*   <tr>
*       <td>2.90</td>
*       <td>Updated linker scripts for PSoC 64 Secure MCU cyb06xx7 devices.</td>
*       <td>Flash allocation adjustment.</td>
*   </tr>
*   <tr>
*       <td rowspan="2">2.80</td>
*       <td>Updated linker scripts for PSoC 64 Secure MCU devices.</td>
*       <td>Updated FLASH and SRAM memory area definitions in cyb0xxx linker script templates
*           in accordance with the PSoC 64 Secure Boot SDK policies.</td>
*   </tr>
*   <tr>
*       <td>Added \ref Cy_PRA_Init() call to \ref SystemInit() Cortex-M0+ and Cortex-M4 functions for PSoC 64 Secure MCU.</td>
*       <td>Updated PSoC 64 Secure MCU startup sequence to initialize the Protected Register Access driver.</td>
*   </tr>
*   <tr>
*       <td>2.70.1</td>
*       <td>Updated documentation for the better description of the existing startup implementation.</td>
*       <td>User experience enhancement.</td>
*   </tr>
*   <tr>
*       <td rowspan="5">2.70</td>
*       <td>Updated \ref SystemCoreClockUpdate() implementation - The SysClk API is reused.</td>
*       <td>Code optimization.</td>
*   </tr>
*   <tr>
*       <td>Updated \ref SystemInit() implementation - The IPC7 structure is initialized for both cores.</td>
*       <td>Provided support for SysPM driver updates.</td>
*   </tr>
*   <tr>
*       <td>Updated the linker scripts.</td>
*       <td>Reserved FLASH area for the MCU boot headers.</td>
*   </tr>
*   <tr>
*       <td>Added System Pipe initialization for all devices. </td>
*       <td>Improved PDL usability according to user experience.</td>
*   </tr>
*   <tr>
*       <td>Removed redundant legacy macros: CY_CLK_EXT_FREQ_HZ, CY_CLK_ECO_FREQ_HZ and CY_CLK_ALTHF_FREQ_HZ.
*           Use \ref Cy_SysClk_ExtClkSetFrequency, \ref Cy_SysClk_EcoConfigure and \ref Cy_BLE_EcoConfigure functions instead them. </td>
*       <td>Defect fixing.</td>
*   </tr>
*   <tr>
*       <td>2.60</td>
*       <td>Updated linker scripts.</td>
*       <td>Provided support for new devices, updated usage of CM0p prebuilt image.</td>
*   </tr>
*   <tr>
*       <td>2.50</td>
*       <td>Updated assembler files, C files, linker scripts.</td>
*       <td>Dynamic allocated HEAP size for Arm Compiler 6, IAR 8.</td>
*   </tr>
*   <tr>
*       <td>2.40</td>
*       <td>Updated assembler files, C files, linker scripts.</td>
*       <td>Added Arm Compiler 6 support.</td>
*   </tr>
*   <tr>
*       <td rowspan="2">2.30</td>
*       <td>Added assembler files, linker scripts for Mbed OS.</td>
*       <td>Added Arm Mbed OS embedded operating system support.</td>
*   </tr>
*   <tr>
*       <td>Updated linker scripts to extend the Flash and Ram memories size available for the CM4 core.</td>
*       <td>Enhanced PDL usability.</td>
*   </tr>
*   <tr>
*       <td>2.20</td>
*       <td>Moved the Cy_IPC_SystemSemaInit(), Cy_IPC_SystemPipeInit() functions implementation from IPC to Startup.</td>
*       <td>Changed the IPC driver configuration method from compile time to run time.</td>
*   </tr>
*   <tr>
*     <td rowspan="2"> 2.10</td>
*     <td>Added constructor attribute to SystemInit() function declaration for ARM MDK compiler. \n
*         Removed $Sub$$main symbol for ARM MDK compiler.
*     </td>
*     <td>uVision Debugger support.</td>
*   </tr>
*   <tr>
*     <td>Updated description of the Startup behavior for Single-Core Devices. \n
*         Added note about WDT disabling by SystemInit() function.
*     </td>
*     <td>Documentation improvement.</td>
*   </tr>
*   <tr>
*     <td rowspan="4"> 2.0</td>
*     <td>Added restoring of FLL registers to the default state in SystemInit() API for single core devices.
*         Single core device support.
*     </td>
*     <td></td>
*   </tr>
*   <tr>
*     <td>Added Normal Access Restrictions, Public Key, TOC part2 and TOC part2 copy to Supervisory flash linker memory regions. \n
*         Renamed 'wflash' memory region to 'em_eeprom'.
*     </td>
*     <td>Linker scripts usability improvement.</td>
*   </tr>
*   <tr>
*     <td>Added Cy_IPC_SystemSemaInit(), Cy_IPC_SystemPipeInit(), Cy_Flash_Init() functions call to SystemInit() API.</td>
*     <td>Reserved system resources for internal operations.</td>
*   </tr>
*   <tr>
*     <td>Added clearing and releasing of IPC structure #7 (reserved for the Deep-Sleep operations) to SystemInit() API.</td>
*     <td>To avoid deadlocks in case of SW or WDT reset during Deep-Sleep entering.</td>
*   </tr>
*   <tr>
*       <td>1.0</td>
*       <td>Initial version</td>
*       <td></td>
*   </tr>
* </table>
*
*
* \defgroup group_system_config_macro Macros
* \{
*   \defgroup group_system_config_system_macro            System Macros
*   \defgroup group_system_config_cm4_status_macro        Cortex-M4 Status Macros
*   \defgroup group_system_config_user_settings_macro     User Settings Macros
* \}
* \defgroup group_system_config_functions Functions
* \{
*   \defgroup group_system_config_cm4_functions           Cortex-M4 Control Functions
* \}
* \defgroup group_system_config_globals Global Variables
*
* \}
*/

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
*******************************************************************************/
#include <stdint.h>


/*******************************************************************************
* Global preprocessor symbols/macros ('define')
*******************************************************************************/
#if ((defined(__GNUC__) && (__ARM_ARCH == 6) && (__ARM_ARCH_6M__ == 1)) || \
    (defined(__ICCARM__) && (__CORE__ == __ARM6M__)) || \
    (defined(__ARMCC_VERSION) && (__TARGET_ARCH_THUMB == 3)))
    #define CY_SYSTEM_CPU_CM0P          1UL
#else
    #define CY_SYSTEM_CPU_CM0P          0UL
#endif


/*******************************************************************************
*
*                      START OF USER SETTINGS HERE
*                      ===========================
*
*                 All lines with '<<<' can be set by user.
*
*******************************************************************************/

/**
* \addtogroup group_system_config_user_settings_macro
* \{
*/

/*
 * Include optional application-specific configuration header.
 *
 * For example, custom system_psoc6_config.h can be included here
 * by adding the below macro definition to the build system:
 * DEFINES+=CY_SYSTEM_PSOC6_CONFIG='"system_psoc6_config.h"'
 */
#if defined(CY_SYSTEM_PSOC6_CONFIG)
#include CY_SYSTEM_PSOC6_CONFIG
#endif


/***************************************************************************//**
* \brief Start address of the Cortex-M4 application ([address]UL)
*        <i>(USER SETTING)</i>
*******************************************************************************/
#if !defined(CY_CORTEX_M4_APPL_ADDR)
    #define CY_CORTEX_M4_APPL_ADDR          (CY_FLASH_BASE + 0x2000U)   /* <<< 8 kB of flash is reserved for the Cortex-M0+ application */
#endif /* (CY_CORTEX_M4_APPL_ADDR) */


/***************************************************************************//**
* \brief IPC Semaphores allocation ([value]UL).
*        <i>(USER SETTING)</i>
*******************************************************************************/
#define CY_IPC_SEMA_COUNT               (128UL)  /* <<< This will allow 128 (4*32) semaphores */


/***************************************************************************//**
* \brief IPC Pipe definitions ([value]UL).
*        <i>(USER SETTING)</i>
*******************************************************************************/
#define CY_IPC_MAX_ENDPOINTS            (8UL) /* <<< 8 endpoints */


/*******************************************************************************
*
*                         END OF USER SETTINGS HERE
*                         =========================
*
*******************************************************************************/

/** \} group_system_config_user_settings_macro */


/**
* \addtogroup group_system_config_system_macro
* \{
*/

#if (CY_SYSTEM_CPU_CM0P == 1UL) || defined(CY_DOXYGEN)
/** The Cortex-M0+ startup driver identifier */
    #define CY_STARTUP_M0P_ID               ((uint32_t)((uint32_t)((0x0EU) & 0x3FFFU) << 18U))
#endif /* (CY_SYSTEM_CPU_CM0P == 1UL) */

#if (CY_SYSTEM_CPU_CM0P != 1UL) || defined(CY_DOXYGEN)
/** The Cortex-M4 startup driver identifier */
    #define CY_STARTUP_M4_ID        ((uint32_t)((uint32_t)((0x0FU) & 0x3FFFU) << 18U))
#endif /* (CY_SYSTEM_CPU_CM0P != 1UL) */

/** \} group_system_config_system_macro */


/** \cond */
#if defined(__ARMCC_VERSION)
extern void SystemInit(void) __attribute__((constructor));
#else
extern void SystemInit(void);
#endif /* (__ARMCC_VERSION) */

extern void SystemCoreClockUpdate(void);
/** \endcond */


/**
* \addtogroup group_system_config_cm4_functions
* \{
*/
extern uint32_t Cy_SysGetCM4Status(void);
extern void     Cy_SysEnableCM4(uint32_t vectorTableOffset);
extern void     Cy_SysDisableCM4(void);
extern void     Cy_SysRetainCM4(void);
extern void     Cy_SysResetCM4(void);
/** \} group_system_config_cm4_functions */


/** \cond */
extern void     Default_Handler(void);

void Cy_SysIpcPipeIsrCm0(void);
void Cy_SysIpcPipeIsrCm4(void);

extern void     Cy_SystemInit(void);
extern void     Cy_SystemInitFpuEnable(void);

extern uint32_t cy_delayFreqKhz;
extern uint8_t cy_delayFreqMhz;
/** \endcond */


#if (CY_SYSTEM_CPU_CM0P == 1UL) || defined(CY_DOXYGEN)
/**
* \addtogroup group_system_config_cm4_status_macro
* \{
*/
#define CY_SYS_CM4_STATUS_ENABLED   (3U)    /**< The Cortex-M4 core is enabled: power on, clock on, no isolate, no reset and no retain. */
#define CY_SYS_CM4_STATUS_DISABLED  (0U)    /**< The Cortex-M4 core is disabled: power off, clock off, isolate, reset and no retain. */
#define CY_SYS_CM4_STATUS_RETAINED  (2U)    /**< The Cortex-M4 core is retained. power off, clock off, isolate, no reset and retain. */
#define CY_SYS_CM4_STATUS_RESET     (1U)    /**< The Cortex-M4 core is in the Reset mode: clock off, no isolated, no retain and reset. */
/** \} group_system_config_cm4_status_macro */

#endif /* (CY_SYSTEM_CPU_CM0P == 1UL) */


/*******************************************************************************
*                             IPC Configuration
*                         =========================
*******************************************************************************/
/* IPC CY_PIPE default configuration */
#define CY_SYS_CYPIPE_CLIENT_CNT        (8UL)

#define CY_SYS_INTR_CYPIPE_MUX_EP0      (1UL)   /* IPC CYPRESS PIPE */
#define CY_SYS_INTR_CYPIPE_PRIOR_EP0    (1UL)   /* Notifier Priority */
#define CY_SYS_INTR_CYPIPE_PRIOR_EP1    (1UL)   /* Notifier Priority */

#define CY_SYS_CYPIPE_CHAN_MASK_EP0     (0x0001UL << CY_IPC_CHAN_CYPIPE_EP0)
#define CY_SYS_CYPIPE_CHAN_MASK_EP1     (0x0001UL << CY_IPC_CHAN_CYPIPE_EP1)


/******************************************************************************/
/*
 * The System pipe configuration defines the IPC channel number, interrupt
 * number, and the pipe interrupt mask for the endpoint.
 *
 * The format of the endPoint configuration
 *    Bits[31:16] Interrupt Mask
 *    Bits[15:8 ] IPC interrupt
 *    Bits[ 7:0 ] IPC channel
 */

/* System Pipe addresses */
/* CyPipe defines */

#define CY_SYS_CYPIPE_INTR_MASK   (CY_SYS_CYPIPE_CHAN_MASK_EP0 | CY_SYS_CYPIPE_CHAN_MASK_EP1)

#define CY_SYS_CYPIPE_CONFIG_EP0  ((CY_SYS_CYPIPE_INTR_MASK << CY_IPC_PIPE_CFG_IMASK_Pos) \
    | (CY_IPC_INTR_CYPIPE_EP0 << CY_IPC_PIPE_CFG_INTR_Pos) \
    | CY_IPC_CHAN_CYPIPE_EP0)
#define CY_SYS_CYPIPE_CONFIG_EP1  ((CY_SYS_CYPIPE_INTR_MASK << CY_IPC_PIPE_CFG_IMASK_Pos) \
    | (CY_IPC_INTR_CYPIPE_EP1 << CY_IPC_PIPE_CFG_INTR_Pos) \
    | CY_IPC_CHAN_CYPIPE_EP1)

/******************************************************************************/



/** \addtogroup group_system_config_globals
* \{
*/
extern uint32_t cy_BleEcoClockFreqHz;
/** \} group_system_config_globals */

/** \cond INTERNAL */
extern uint32_t cy_Hfclk0FreqHz;
extern uint32_t cy_PeriClkFreqHz;
extern uint32_t SystemCoreClock;
extern uint32_t cy_AhbFreqHz;




/*******************************************************************************
* Backward compatibility macros. The following code is DEPRECATED and must
* not be used in new projects
*******************************************************************************/

/* BWC defines for functions related to enter/exit critical section */
#define Cy_SaveIRQ      Cy_SysLib_EnterCriticalSection
#define Cy_RestoreIRQ   Cy_SysLib_ExitCriticalSection
#define CY_SYS_INTR_CYPIPE_EP0          (CY_IPC_INTR_CYPIPE_EP0)
#define CY_SYS_INTR_CYPIPE_EP1          (CY_IPC_INTR_CYPIPE_EP1)
#define cy_delayFreqHz                  (SystemCoreClock)

/** \endcond */

#ifdef __cplusplus
}
#endif

#endif /* _SYSTEM_PSOC6_H_ */


/* [] END OF FILE */

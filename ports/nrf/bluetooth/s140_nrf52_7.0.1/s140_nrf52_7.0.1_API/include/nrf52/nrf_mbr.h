/*
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
  @defgroup nrf_mbr_api Master Boot Record API
  @{

  @brief APIs for updating SoftDevice and BootLoader

*/

#ifndef NRF_MBR_H__
#define NRF_MBR_H__

#include "nrf_svc.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup NRF_MBR_DEFINES Defines
 * @{ */

/**@brief MBR SVC Base number. */
#define MBR_SVC_BASE            (0x18)

/**@brief Page size in words. */
#define MBR_PAGE_SIZE_IN_WORDS  (1024)

/** @brief The size that must be reserved for the MBR when a SoftDevice is written to flash.
This is the offset where the first byte of the SoftDevice hex file is written. */
#define MBR_SIZE                (0x1000)

/** @brief Location (in the flash memory) of the bootloader address. */
#define MBR_BOOTLOADER_ADDR      (0xFF8)

/** @brief Location (in UICR) of the bootloader address. */
#define MBR_UICR_BOOTLOADER_ADDR (&(NRF_UICR->NRFFW[0]))

/** @brief Location (in the flash memory) of the address of the MBR parameter page. */
#define MBR_PARAM_PAGE_ADDR      (0xFFC)

/** @brief Location (in UICR) of the address of the MBR parameter page. */
#define MBR_UICR_PARAM_PAGE_ADDR (&(NRF_UICR->NRFFW[1]))


/** @} */

/** @addtogroup NRF_MBR_ENUMS Enumerations
 * @{ */

/**@brief nRF Master Boot Record API SVC numbers. */
enum NRF_MBR_SVCS
{
  SD_MBR_COMMAND = MBR_SVC_BASE, /**< ::sd_mbr_command */
};

/**@brief Possible values for ::sd_mbr_command_t.command */
enum NRF_MBR_COMMANDS
{
  SD_MBR_COMMAND_COPY_BL,                 /**< Copy a new BootLoader. @see ::sd_mbr_command_copy_bl_t*/
  SD_MBR_COMMAND_COPY_SD,                 /**< Copy a new SoftDevice. @see ::sd_mbr_command_copy_sd_t*/
  SD_MBR_COMMAND_INIT_SD,                 /**< Initialize forwarding interrupts to SD, and run reset function in SD. Does not require any parameters in ::sd_mbr_command_t params.*/
  SD_MBR_COMMAND_COMPARE,                 /**< This command works like memcmp. @see ::sd_mbr_command_compare_t*/
  SD_MBR_COMMAND_VECTOR_TABLE_BASE_SET,   /**< Change the address the MBR starts after a reset. @see ::sd_mbr_command_vector_table_base_set_t*/
  SD_MBR_COMMAND_RESERVED,
  SD_MBR_COMMAND_IRQ_FORWARD_ADDRESS_SET, /**< Start forwarding all interrupts to this address. @see ::sd_mbr_command_irq_forward_address_set_t*/
};

/** @} */

/** @addtogroup NRF_MBR_TYPES Types
 * @{ */

/**@brief This command copies part of a new SoftDevice
 *
 * The destination area is erased before copying.
 * If dst is in the middle of a flash page, that whole flash page will be erased.
 * If (dst+len) is in the middle of a flash page, that whole flash page will be erased.
 *
 * The user of this function is responsible for setting the BPROT registers.
 *
 * @retval ::NRF_SUCCESS indicates that the contents of the memory blocks where copied correctly.
 * @retval ::NRF_ERROR_INTERNAL indicates that the contents of the memory blocks where not verified correctly after copying.
 */
typedef struct
{
  uint32_t *src;  /**< Pointer to the source of data to be copied.*/
  uint32_t *dst;  /**< Pointer to the destination where the content is to be copied.*/
  uint32_t len;   /**< Number of 32 bit words to copy. Must be a multiple of @ref MBR_PAGE_SIZE_IN_WORDS words.*/
} sd_mbr_command_copy_sd_t;


/**@brief This command works like memcmp, but takes the length in words.
 *
 * @retval ::NRF_SUCCESS indicates that the contents of both memory blocks are equal.
 * @retval ::NRF_ERROR_NULL indicates that the contents of the memory blocks are not equal.
 */
typedef struct
{
  uint32_t *ptr1; /**< Pointer to block of memory. */
  uint32_t *ptr2; /**< Pointer to block of memory. */
  uint32_t len;   /**< Number of 32 bit words to compare.*/
} sd_mbr_command_compare_t;


/**@brief This command copies a new BootLoader.
 *
 * The MBR assumes that either @ref MBR_BOOTLOADER_ADDR or @ref MBR_UICR_BOOTLOADER_ADDR is set to
 * the address where the bootloader will be copied. If both addresses are set, the MBR will prioritize
 * @ref MBR_BOOTLOADER_ADDR.
 *
 * The bootloader destination is erased by this function.
 * If (destination+bl_len) is in the middle of a flash page, that whole flash page will be erased.
 *
 * This command requires that @ref MBR_PARAM_PAGE_ADDR or @ref MBR_UICR_PARAM_PAGE_ADDR is set,
 * see @ref sd_mbr_command.
 *
 * This command will use the flash protect peripheral (BPROT or ACL) to protect the flash that is
 * not intended to be written.
 *
 * On success, this function will not return. It will start the new bootloader from reset-vector as normal.
 *
 * @retval ::NRF_ERROR_INTERNAL indicates an internal error that should not happen.
 * @retval ::NRF_ERROR_FORBIDDEN if the bootloader address is not set.
 * @retval ::NRF_ERROR_INVALID_LENGTH if parameters attempts to read or write outside flash area.
 * @retval ::NRF_ERROR_NO_MEM No MBR parameter page is provided. See @ref sd_mbr_command.
 */
typedef struct
{
  uint32_t *bl_src;  /**< Pointer to the source of the bootloader to be be copied.*/
  uint32_t bl_len;   /**< Number of 32 bit words to copy for BootLoader. */
} sd_mbr_command_copy_bl_t;

/**@brief Change the address the MBR starts after a reset
 *
 * Once this function has been called, this address is where the MBR will start to forward
 * interrupts to after a reset.
 *
 * To restore default forwarding, this function should be called with @ref address set to 0. If a
 * bootloader is present, interrupts will be forwarded to the bootloader. If not, interrupts will
 * be forwarded to the SoftDevice.
 *
 * The location of a bootloader can be specified in @ref MBR_BOOTLOADER_ADDR or
 * @ref MBR_UICR_BOOTLOADER_ADDR. If both addresses are set, the MBR will prioritize
 * @ref MBR_BOOTLOADER_ADDR.
 *
 * This command requires that @ref MBR_PARAM_PAGE_ADDR or @ref MBR_UICR_PARAM_PAGE_ADDR is set,
 * see @ref sd_mbr_command.
 *
 * On success, this function will not return. It will reset the device.
 *
 * @retval ::NRF_ERROR_INTERNAL indicates an internal error that should not happen.
 * @retval ::NRF_ERROR_INVALID_ADDR if parameter address is outside of the flash size.
 * @retval ::NRF_ERROR_NO_MEM No MBR parameter page is provided. See @ref sd_mbr_command.
 */
typedef struct
{
  uint32_t address; /**< The base address of the interrupt vector table for forwarded interrupts.*/
} sd_mbr_command_vector_table_base_set_t;

/**@brief Sets the base address of the interrupt vector table for interrupts forwarded from the MBR
 *
 * Unlike sd_mbr_command_vector_table_base_set_t, this function does not reset, and it does not
 * change where the MBR starts after reset.
 *
 * @retval ::NRF_SUCCESS
 */
typedef struct
{
  uint32_t address; /**< The base address of the interrupt vector table for forwarded interrupts.*/
} sd_mbr_command_irq_forward_address_set_t;

/**@brief Input structure containing data used when calling ::sd_mbr_command
 *
 * Depending on what command value that is set, the corresponding params value type must also be
 * set. See @ref NRF_MBR_COMMANDS for command types and corresponding params value type. If command
 * @ref SD_MBR_COMMAND_INIT_SD is set, it is not necessary to set any values under params.
 */
typedef struct
{
  uint32_t command;  /**< Type of command to be issued. See @ref NRF_MBR_COMMANDS. */
  union
  {
    sd_mbr_command_copy_sd_t copy_sd;  /**< Parameters for copy SoftDevice.*/
    sd_mbr_command_compare_t compare;  /**< Parameters for verify.*/
    sd_mbr_command_copy_bl_t copy_bl;  /**< Parameters for copy BootLoader. Requires parameter page. */
    sd_mbr_command_vector_table_base_set_t base_set; /**< Parameters for vector table base set. Requires parameter page.*/
    sd_mbr_command_irq_forward_address_set_t irq_forward_address_set; /**< Parameters for irq forward address set*/
  } params; /**< Command parameters. */
} sd_mbr_command_t;

/** @} */

/** @addtogroup NRF_MBR_FUNCTIONS Functions
 * @{ */

/**@brief Issue Master Boot Record commands
 *
 * Commands used when updating a SoftDevice and bootloader.
 *
 * The @ref SD_MBR_COMMAND_COPY_BL and @ref SD_MBR_COMMAND_VECTOR_TABLE_BASE_SET requires
 * parameters to be retained by the MBR when resetting the IC. This is done in a separate flash
 * page. The location of the flash page should be provided by the application in either
 * @ref MBR_PARAM_PAGE_ADDR or @ref MBR_UICR_PARAM_PAGE_ADDR. If both addresses are set, the MBR
 * will prioritize @ref MBR_PARAM_PAGE_ADDR. This page will be cleared by the MBR and is used to
 * store the command before reset. When an address is specified, the page it refers to must not be
 * used by the application. If no address is provided by the application, i.e. both
 * @ref MBR_PARAM_PAGE_ADDR and @ref MBR_UICR_PARAM_PAGE_ADDR is 0xFFFFFFFF, MBR commands which use
 * flash will be unavailable and return @ref NRF_ERROR_NO_MEM.
 *
 * @param[in]  param Pointer to a struct describing the command.
 *
 * @note For a complete set of return values, see ::sd_mbr_command_copy_sd_t,
 *       ::sd_mbr_command_copy_bl_t, ::sd_mbr_command_compare_t,
 *       ::sd_mbr_command_vector_table_base_set_t, ::sd_mbr_command_irq_forward_address_set_t
 *
 * @retval ::NRF_ERROR_NO_MEM No MBR parameter page provided
 * @retval ::NRF_ERROR_INVALID_PARAM if an invalid command is given.
*/
SVCALL(SD_MBR_COMMAND, uint32_t, sd_mbr_command(sd_mbr_command_t* param));

/** @} */

#ifdef __cplusplus
}
#endif
#endif // NRF_MBR_H__

/**
  @}
*/

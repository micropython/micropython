/**
 * \file
 *
 * \brief Common SD/MMC stack header file
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef SD_MMC_H_INCLUDED
#define SD_MMC_H_INCLUDED

#include "compiler.h"
#include "conf_sd_mmc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup common_memory
 * \defgroup sd_mmc_stack_group SD/MMC/SDIO common stack
 *
 * SD/MMC/SDIO basic APIs used by SD/MMC/SDIO memory
 * APIs (\ref sd_mmc_stack_mem_group).
 * Also, it can be used by application which use the SDIO card
 * or specific application which does not need of File System.
 *
 * For usual application which use the SD/MMC card in
 * memory mode with a file system, please refer to
 * \ref sd_mmc_stack_mem_group.
 * @{
 */

typedef uint8_t sd_mmc_err_t; /**< Type of return error code */

/** \name Return error codes */
/** @{ */
#define SD_MMC_OK 0           /**< No error */
#define SD_MMC_INIT_ONGOING 1 /**< Card not initialized */
#define SD_MMC_ERR_NO_CARD 2  /**< No SD/MMC card inserted */
#define SD_MMC_ERR_UNUSABLE 3 /**< Unusable card */
#define SD_MMC_ERR_SLOT 4     /**< Slot unknow */
#define SD_MMC_ERR_COMM 5     /**< General communication error */
#define SD_MMC_ERR_PARAM 6    /**< Illeage input parameter */
#define SD_MMC_ERR_WP 7       /**< Card write protected */
/** @} */

typedef uint8_t card_type_t; /**< Type of card type */

/** \name Card Types */
/** @{ */
#define CARD_TYPE_UNKNOWN (0)   /**< Unknown type card */
#define CARD_TYPE_SD (1 << 0)   /**< SD card */
#define CARD_TYPE_MMC (1 << 1)  /**< MMC card */
#define CARD_TYPE_SDIO (1 << 2) /**< SDIO card */
#define CARD_TYPE_HC (1 << 3)   /**< High capacity card */
/** SD combo card (io + memory) */
#define CARD_TYPE_SD_COMBO (CARD_TYPE_SD | CARD_TYPE_SDIO)
/** @} */

typedef uint8_t card_version_t; /**< Type of card version */

/** \name Card Versions */
/** @{ */
#define CARD_VER_UNKNOWN (0)    /**< Unknown card version */
#define CARD_VER_SD_1_0 (0x10)  /**< SD version 1.0 and 1.01 */
#define CARD_VER_SD_1_10 (0x1A) /**< SD version 1.10 */
#define CARD_VER_SD_2_0 (0X20)  /**< SD version 2.00 */
#define CARD_VER_SD_3_0 (0X30)  /**< SD version 3.0X */
#define CARD_VER_MMC_1_2 (0x12) /**< MMC version 1.2 */
#define CARD_VER_MMC_1_4 (0x14) /**< MMC version 1.4 */
#define CARD_VER_MMC_2_2 (0x22) /**< MMC version 2.2 */
#define CARD_VER_MMC_3 (0x30)   /**< MMC version 3 */
#define CARD_VER_MMC_4 (0x40)   /**< MMC version 4 */
/** @} */

/** Card detect setting */
typedef struct sd_mmc_detect {
	int16_t  pin; /**< Detection pin, -1 if no such pin */
	uint16_t val; /**< Detection value */
} sd_mmc_detect_t;

/** This SD MMC stack uses the maximum block size autorized (512 bytes) */
#define SD_MMC_BLOCK_SIZE 512

/**
 * \brief Initialize the SD/MMC stack and low level driver required
 * \param[in] hal Pointer to HAL instance
 * \param[in] card_detects Pointer to list of card detect settings,
 *                         list size should be \ref CONF_SD_MMC_MEM_CNT
 * \param[in] wp_detects Pointer to list of write protect detect settings
 *                       list size should be \ref CONF_SD_MMC_MEM_CNT
 */
void sd_mmc_init(void *hal, sd_mmc_detect_t *card_detects, sd_mmc_detect_t *wp_detects);

/** \brief Return the number of slot available
 *
 * \return Number of card slot available
 */
uint8_t sd_mmc_nb_slot(void);

/** \brief Performs a card checks
 *
 * \param[in] slot     Card slot to use
 *
 * \retval SD_MMC_OK           Card ready
 * \retval SD_MMC_INIT_ONGOING Initialization on going
 * \retval SD_MMC_ERR_NO_CARD  Card not present in slot
 * \retval Other value for error cases, see \ref sd_mmc_err_t
 */
sd_mmc_err_t sd_mmc_check(uint8_t slot);

/** \brief Get the card type
 *
 * \param[in] slot     Card slot
 *
 * \return Card type (\ref card_type_t)
 */
card_type_t sd_mmc_get_type(uint8_t slot);

/** \brief Get the card version
 *
 * \param[in] slot     Card slot
 *
 * \return Card version (\ref card_version_t)
 */
card_version_t sd_mmc_get_version(uint8_t slot);

/** \brief Get the memory capacity
 *
 * \param[in] slot     Card slot
 *
 * \return Capacity (unit KB)
 */
uint32_t sd_mmc_get_capacity(uint8_t slot);

/** \brief Get the card write protection status
 *
 * \param[in] slot     Card slot
 *
 * \return true, if write portected
 */
bool sd_mmc_is_write_protected(uint8_t slot);

/**
 * \brief Initialize the read blocks of data from the card.
 *
 * \param[in] slot     Card slot to use
 * \param[in] start    Start block number to to read.
 * \param[in] nb_block Total number of blocks to be read.
 *
 * \return return SD_MMC_OK if success,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sd_mmc_init_read_blocks(uint8_t slot, uint32_t start, uint16_t nb_block);

/**
 * \brief Start the read blocks of data from the card.
 *
 * \param[out] dest     Pointer to read buffer.
 * \param[in]  nb_block Number of blocks to be read.
 *
 * \return return SD_MMC_OK if started,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sd_mmc_start_read_blocks(void *dest, uint16_t nb_block);

/**
 * \brief Wait the end of read blocks of data from the card.
 *
 * \param[in] abort Abort reading process initialized by
 *              \ref sd_mmc_init_read_blocks() after the reading issued by
 *              \ref sd_mmc_start_read_blocks() is done
 *
 * \return return SD_MMC_OK if success,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sd_mmc_wait_end_of_read_blocks(bool abort);

/**
 * \brief Initialize the write blocks of data
 *
 * \param[in] slot     Card slot to use
 * \param[in] start    Start block number to be written.
 * \param[in] nb_block Total number of blocks to be written.
 *
 * \return return SD_MMC_OK if success,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sd_mmc_init_write_blocks(uint8_t slot, uint32_t start, uint16_t nb_block);

/**
 * \brief Start the write blocks of data
 *
 * \param[in] src      Pointer to write buffer.
 * \param[in] nb_block Number of blocks to be written.
 *
 * \return return SD_MMC_OK if started,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sd_mmc_start_write_blocks(const void *src, uint16_t nb_block);

/**
 * \brief Wait the end of write blocks of data
 *
 * \param[in] abort Abort writing process initialized by
 *              \ref sd_mmc_init_write_blocks() after the writing issued by
 *              \ref sd_mmc_start_write_blocks() is done
 *
 * \return return SD_MMC_OK if success,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sd_mmc_wait_end_of_write_blocks(bool abort);

#if (CONF_SDIO_SUPPORT == 1)
/**
 * \brief Read one byte from SDIO using RW_DIRECT command.
 *
 * \param[in]  slot     Card slot to use
 * \param[in]  func_num  Function number.
 * \param[in]  addr      Register address to read from.
 * \param[out] dest      Pointer to read buffer.
 *
 * \return return SD_MMC_OK if success,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sdio_read_direct(uint8_t slot, uint8_t func_num, uint32_t addr, uint8_t *dest);
/**
 * \brief Write one byte to SDIO using RW_DIRECT command.
 *
 * \param[in] slot     Card slot to use
 * \param[in] func_num  Function number.
 * \param[in] addr      Register address to read from.
 * \param[in] data      Data to be written.
 *
 * \return return SD_MMC_OK if success,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sdio_write_direct(uint8_t slot, uint8_t func_num, uint32_t addr, uint8_t data);

/**
 * \brief Read bytes from SDIO using RW_EXTENDED command.
 *
 * \param[in] slot     Card slot to use
 * \param[in] func_num     Function number.
 * \param[in] addr         First register address to read from.
 * \param[in] inc_addr     0 - The data address is fixed.
 *                     1 - The data address increase automatically.
 * \param[out] dest         Pointer to read buffer.
 * \param[in] size         Number of bytes to read (1 ~ 512).
 *
 * \return return SD_MMC_OK if success,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sdio_read_extended(uint8_t slot, uint8_t func_num, uint32_t addr, uint8_t inc_addr, uint8_t *dest,
                                uint16_t size);

/**
 * \brief Write bytes to SDIO using RW_EXTENDED command.
 *
 * \param[in] slot     Card slot to use
 * \param[in] func_num     Function number.
 * \param[in] addr         First register address to write to.
 * \param[in] inc_addr     0 - The data address is fixed.
 *                     1 - The data address increase automatically.
 * \param[in] src          Pointer to write buffer.
 * \param[in] size         Number of bytes to read (1 ~ 512).
 *
 * \return return SD_MMC_OK if success,
 *         otherwise return an error code (\ref sd_mmc_err_t).
 */
sd_mmc_err_t sdio_write_extended(uint8_t slot, uint8_t func_num, uint32_t addr, uint8_t inc_addr, uint8_t *src,
                                 uint16_t size);
#endif /* SDIO_SUPPORT_ENABLE */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SD_MMC_H_INCLUDED */

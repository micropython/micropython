/* Auto-generated config file conf_sd_mmc.h */
#ifndef CONF_SD_MMC_H
#define CONF_SD_MMC_H

// <<< Use Configuration Wizard in Context Menu >>>

// <q> Enable the SDIO support
// <id> conf_sdio_support
#ifndef CONF_SDIO_SUPPORT
#define CONF_SDIO_SUPPORT 0
#endif

// <q> Enable the MMC card support
// <id> conf_mmc_support
#ifndef CONF_MMC_SUPPORT
#define CONF_MMC_SUPPORT 0
#endif

// <q> Enable the OS support
// <id> conf_sd_mmc_os_support
#ifndef CONF_OS_SUPPORT
#define CONF_OS_SUPPORT 0
#endif

// Detection (card/write protect) timeout (ms/ticks)
// conf_sd_mmc_debounce
#ifndef CONF_SD_MMC_DEBOUNCE
#define CONF_SD_MMC_DEBOUNCE 1000
#endif

#ifndef CONF_SD_MMC_MEM_CNT
#define CONF_SD_MMC_MEM_CNT 1
#endif

// <e> SD/MMC Slot 0
// <id> conf_sd_mmc_0_enable
#ifndef CONF_SD_MMC_0_ENABLE
#define CONF_SD_MMC_0_ENABLE 1
#endif

// <e> Card Detect (CD) 0 Enable
// <id> conf_sd_mmc_0_cd_detect_en
#ifndef CONF_SD_MMC_0_CD_DETECT_EN
#define CONF_SD_MMC_0_CD_DETECT_EN 0
#endif

// <o> Card Detect (CD) detection level
// <1=> High
// <0=> Low
// <id> conf_sd_mmc_0_cd_detect_value
#ifndef CONF_SD_MMC_0_CD_DETECT_VALUE
#define CONF_SD_MMC_0_CD_DETECT_VALUE 0
#endif
// </e>

// <e> Write Protect (WP) 0 Enable
// <id> conf_sd_mmc_0_wp_detect_en
#ifndef CONF_SD_MMC_0_WP_DETECT_EN
#define CONF_SD_MMC_0_WP_DETECT_EN 0
#endif

// <o> Write Protect (WP) detection level
// <1=> High
// <0=> Low
// <id> conf_sd_mmc_0_wp_detect_value
#ifndef CONF_SD_MMC_0_WP_DETECT_VALUE
#define CONF_SD_MMC_0_WP_DETECT_VALUE 1
#endif
// </e>

// </e>

#ifndef CONF_MCI_OS_SUPPORT
#define CONF_MCI_OS_SUPPORT 0
#endif

// <<< end of configuration section >>>

#endif // CONF_SD_MMC_H

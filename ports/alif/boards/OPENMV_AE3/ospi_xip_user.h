// This file is needed by ospi_xip/source/ospi/ospi_drv.c.
#define OSPI_XIP_ENABLE_AES_DECRYPTION (0)
#define OSPI_XIP_RX_SAMPLE_DELAY (4)
#define OSPI_XIP_DDR_DRIVE_EDGE (0)
// floor(1/4 OSPI clock cycle + 3.6ns) * 2
#define OSPI_XIP_RXDS_DELAY (12)

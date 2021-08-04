idf_component_register(SRCS "sdmmc_cmd.c"
                            "sdmmc_common.c"
                            "sdmmc_init.c"
                            "sdmmc_io.c"
                            "sdmmc_mmc.c"
                            "sdmmc_sd.c"
                    INCLUDE_DIRS include
                    REQUIRES driver
                    PRIV_REQUIRES soc)

if(CONFIG_DRIVER_BADGE_BSP_ENABLE)
    set(srcs
        "driver_badge_bsp.c"
    )
    set(includes
    "include"
    )
else()
    set(srcs "")
    set(includes
    ""
    )
endif()

idf_component_register(
	SRCS "${srcs}"
	INCLUDE_DIRS ${includes}
	REQUIRES micropython badge-bsp
)

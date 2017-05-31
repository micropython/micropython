#ifndef RASPBOOTIN2_PLATFORM
#define RASPBOOTIN2_PLATFORM

    // uncomment the line you want to build for
    //#define PLATFORM_RPI
    //#define PLATFORM_RPIBP
    //#define PLATFORM_RPI2

    #ifdef PLATFORM_RPI
        /// the base address of the periphals in memory
        #define PBASE 0x20000000
        /// name of the "board" for writing in human readable name
        #define PNAME "Raspberry Pi"
    #elif PLATFORM_RPIBP
        /// the base address of the periphals in memory
        #define PBASE 0x20000000
        /// name of the "board" for writing in human readable name
        #define PNAME "Raspberry Pi Model B+"
    #elif defined PLATFORM_RPI2
        /// the base address of the periphals in memory
        #define PBASE 0x3F000000
        /// name of the "board" for writing in human readable name
        #define PNAME "Raspberry Pi 2"
    #endif
#endif

.. _psoc6_feature_list:

Supported features
==================
The PSoC6 port is currently configured to configuration level ``MICROPY_CONFIG_ROM_LEVEL_FULL_FEATURES`` with a few additional settings applied, thus enabling most standard modules as listed in the following. 

Enabled modules
---------------
* Python standard modules and libraries
    * cmath
    * gc
    * math
    * uarray
    * uasyncio
    * ubinascii
    * ucollections
    * uerrno
    * uhashlib
    * uheapq
    * uio
    * ujson
    * uos
    * urandom
    * ure
    * uselect
    * usocket
    * ussl
    * ustruct
    * usys
    * utime
    * uzlib


* Micropython specific modules and libraries
    * framebuf
    * machine
        * Pin
        * I2C
        * RTC
        * SoftI2C
        * SPI
        * SoftSPI
        * PWM
        * Timer
        * ADC
        * ADCBlock

    * micropython
    * ucryptolib
    * uctypes
    * network


* Port specific modules and micro-libraries
    * psoc6 (flash support)


Not yet enabled
---------------
* Python standard modules and libraries
    * _thread

* Micropython specific modules and libraries
    * btree
    * ubluetooth


Table :ref:`configuration details <table_mpy_configuration>` below lists specific settings deviating from the configuration as per config level as well as functionality not yet implemented:

.. _table_mpy_configuration:

+-----------------+----------------------------------------------------------------------------------------------------------------------+
| Module          | Details                                                                                                              |
+=================+======================================================================================================================+
| gc              | Option ``MICROPY_ENABLE_GC`` enabled.                                                                                |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| uhashlib        | Options ``MICROPY_PY_UHASHLIB_MD5``, ``MICROPY_PY_UHASHLIB_SHA1``, ``MICROPY_PY_UHASHLIB_SHA256`` enabled.           |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| uos             | Support for LFS2 and FAT, LFS2 enabled by default. FS mounted on external flash at "/flash".                         |
|                 |                                                                                                                      |
|                 | Options ``MICROPY_PY_OS_DUPTERM``, ``MICROPY_PY_UOS_GETENV_PUTENV_UNSETENV``, ``MICROPY_PY_UOS_INCLUDEFILE``,        |
|                 | ``MICROPY_PY_UOS_SYSTEM``, ``MICROPY_PY_UOS_UNAME``, ``MICROPY_VFS_LFS2`` enabled.                                   |
|                 |                                                                                                                      |
|                 | Function *urandom()* not yet implemented. Requires implementing *mp_uos_urandom()* and setting option                |
|                 | ``MICROPY_PY_UOS_URANDOM``.                                                                                          |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| urandom         | Function *seed()* not yet implemented.                                                                               |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| ure             | Options ``MICROPY_PY_URE_DEBUG``, ``MICROPY_PY_URE_MATCH_GROUPS``, ``MICROPY_PY_URE_MATCH_SPAN_START_END`` enabled.  |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| usocket         | Options ``MICROPY_PY_USOCKET`` enabled.                                                                              |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| ussl            | Options ``MICROPY_PY_USSL`` enabled. Has 2 failing tests.                                                            |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| usys            | Options ``MICROPY_PY_SYS_EXC_INFO`` enabled.                                                                         |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| utime           | Enabled through HAL functions based on machine.RTC module. Option ``MICROPY_PY_UTIME_MP_HAL`` enabled.               |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| ucryptolib      | Options ``MICROPY_PY_UCRYPTOLIB``, ``MICROPY_PY_UCRYPTOLIB_CTR``, ``MICROPY_PY_UCRYPTOLIB_CONSTS`` enabled.          |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| machine         | Functions not yet implemented: *lightsleep()*, *deepsleep()*, *wake_reason()*, *time_pulse_us()*, *rng()*.           | 
|                 |                                                                                                                      |
|                 | Constants not yet implemented : *WLAN_WAKE*, *PIN_WAKE*, *RTC_WAKE*, *IDLE*, *SLEEP*, *DEEPSLEEP*.                   |
|                 |                                                                                                                      |
|                 | Submodules/classes not yet implemented: *ADC*, *bitstream*, *mem*, *Signal*, *SD*, *SDCard*, *SoftSPI*, *SPI*,       |
|                 | *Timer*, *UART*, *WDT*.                                                                                              |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| machine.Pin     | Functions not yet implemented: *drive()*, *irq()*, *mode()*, *pull()*.                                               |
|                 |                                                                                                                      |
|                 | Constants not yet implemented: *ALT*, *ALT_OPEN_DRAIN*, *PULL_UP*, *PULL_DOWN*, *PULL_HOLD*, *LOW_POWER*,            |
|                 | *MED_POWER*, *HIGH_POWER*, *IRQ_FALLING*, *IRQ_RISING*, *IRQ_LOW_LEVEL*, *IRQ_HIGH_LEVEL*.                           |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| machine.I2C     | Option ``MICROPY_PY_MACHINE_I2C`` enabled.                                                                           |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| machine.RTC     | Functions not yet implemented: *alarm()*, *alarm_left()*, *cancel()*, *irq()*.                                       |
|                 |                                                                                                                      |
|                 | Constants not yet implemented: *ALARM0*.                                                                             |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| machine.SoftI2C | Option ``MICROPY_PY_MACHINE_SOFTI2C`` enabled.                                                                       |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| machine.PWM     | option ``MICROPY_PY_MACHINE_PWM`` &  ``MICROPY_PY_MACHINE_PWM_INCLUDEFILE`` enabled                                  |
|                 |                                                                                                                      |
|                 | option ``MICROPY_PY_MACHINE_PWM_DUTY`` is not enabled.                                                               |
+-----------------+----------------------------------------------------------------------------------------------------------------------+                                                                                                                                             
| machine.SoftSPI | Option ``MICROPY_PY_MACHINE_SOFTSPI`` enabled.                                                                       |
+-----------------+----------------------------------------------------------------------------------------------------------------------+                                                                                                                                             
| machine.SPI     | Option ``MICROPY_PY_MACHINE_SPI``, ``MICROPY_PY_MACHINE_SPI_MSB`` , ``MICROPY_PY_MACHINE_SPI_MSB`` enabled.          |    
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| psoc6           | Option to enable the external instead of the internal flash: ``MICROPY_ENABLE_EXT_QSPI_FLASH``.                      |
|                 |                                                                                                                      |
|                 | Option to enable the port specific debug logger: ``MICROPY_LOGGER_DEBUG``.                                           |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| network         |  Option ``MICROPY_NETWORK`` enabled.                                                                                 |
|                 |                                                                                                                      |
|                 |  Functions not yet implemented: *phy_mode()*.                                                                        |
|                 |                                                                                                                      |                                                                             
|                 |  Classes not yet implemented: *LAN*.                                                                                 |                                  
+-----------------+----------------------------------------------------------------------------------------------------------------------+
| network.WLAN    |  Mode not yet implemented: *STA_AP*.                                                                                 |                                
|                 |                                                                                                                      |                                                                             
|                 |  Functions not yet implemented: *config*.                                                                            |               
|                 |                                                                                                                      |
+-----------------+----------------------------------------------------------------------------------------------------------------------+
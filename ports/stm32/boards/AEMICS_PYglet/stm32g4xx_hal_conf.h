#ifndef MICROPY_INCLUDED_STM32G4xx_HAL_CONF_H
#define MICROPY_INCLUDED_STM32G4xx_HAL_CONF_H

#include "boards/stm32g4xx_hal_conf_base.h"

#define HSE_VALUE              (8000000)
#define LSE_VALUE              (32768) /*!< Value of the External oscillator in Hz*/
#define EXTERNAL_CLOCK_VALUE   (0) /*!< Value of the External clock source in Hz*/

#define HSE_STARTUP_TIMEOUT    (100)   /*!< Time out for HSE start up, in ms */
#define LSE_STARTUP_TIMEOUT    (5000)  /*!< Time out for LSE start up, in ms */

#endif /* MICROPY_INCLUDED_STM32G4xx_HAL_CONF_H */

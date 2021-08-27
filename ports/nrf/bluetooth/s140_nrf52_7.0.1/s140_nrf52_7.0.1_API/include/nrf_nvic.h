/*
 * Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
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
 * @defgroup nrf_nvic_api SoftDevice NVIC API
 * @{
 *
 * @note In order to use this module, the following code has to be added to a .c file:
 *     \code
 *     nrf_nvic_state_t nrf_nvic_state = {0};
 *     \endcode
 *
 * @note Definitions and declarations starting with __ (double underscore) in this header file are
 * not intended for direct use by the application.
 *
 * @brief APIs for the accessing NVIC when using a SoftDevice.
 *
 */

#ifndef NRF_NVIC_H__
#define NRF_NVIC_H__

#include <stdint.h>
#include "nrf.h"
#include "nrf_svc.h"
#include "nrf_error.h"
#include "nrf_error_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@addtogroup NRF_NVIC_DEFINES Defines
 * @{ */

/**@defgroup NRF_NVIC_ISER_DEFINES SoftDevice NVIC internal definitions
 * @{ */

#define __NRF_NVIC_NVMC_IRQn (30) /**< The peripheral ID of the NVMC. IRQ numbers are used to identify peripherals, but the NVMC doesn't have an IRQ number in the MDK. */

#define __NRF_NVIC_ISER_COUNT (2) /**< The number of ISER/ICER registers in the NVIC that are used. */

/**@brief Interrupt priority levels used by the SoftDevice. */
#define __NRF_NVIC_SD_IRQ_PRIOS ((uint8_t)( \
    (1U << 0)    /**< Priority level high .*/   \
    | (1U << 1)  /**< Priority level medium. */ \
    | (1U << 4)  /**< Priority level low. */    \
    ))

/**@brief Interrupt priority levels available to the application. */
#define __NRF_NVIC_APP_IRQ_PRIOS ((uint8_t) ~__NRF_NVIC_SD_IRQ_PRIOS)

/**@brief Interrupts used by the SoftDevice, with IRQn in the range 0-31. */
#define __NRF_NVIC_SD_IRQS_0 ((uint32_t)( \
    (1U << POWER_CLOCK_IRQn) \
    | (1U << RADIO_IRQn) \
    | (1U << RTC0_IRQn) \
    | (1U << TIMER0_IRQn) \
    | (1U << RNG_IRQn) \
    | (1U << ECB_IRQn) \
    | (1U << CCM_AAR_IRQn) \
    | (1U << TEMP_IRQn) \
    | (1U << __NRF_NVIC_NVMC_IRQn) \
    | (1U << (uint32_t)SWI5_IRQn) \
    ))

/**@brief Interrupts used by the SoftDevice, with IRQn in the range 32-63. */
#define __NRF_NVIC_SD_IRQS_1 ((uint32_t)0)

/**@brief Interrupts available for to application, with IRQn in the range 0-31. */
#define __NRF_NVIC_APP_IRQS_0 (~__NRF_NVIC_SD_IRQS_0)

/**@brief Interrupts available for to application, with IRQn in the range 32-63. */
#define __NRF_NVIC_APP_IRQS_1 (~__NRF_NVIC_SD_IRQS_1)

/**@} */

/**@} */

/**@addtogroup NRF_NVIC_VARIABLES Variables
 * @{ */

/**@brief Type representing the state struct for the SoftDevice NVIC module. */
typedef struct
{
    uint32_t volatile __irq_masks[__NRF_NVIC_ISER_COUNT]; /**< IRQs enabled by the application in the NVIC. */
    uint32_t volatile __cr_flag;                        /**< Non-zero if already in a critical region */
} nrf_nvic_state_t;

/**@brief Variable keeping the state for the SoftDevice NVIC module. This must be declared in an
 * application source file. */
extern nrf_nvic_state_t nrf_nvic_state;

/**@} */

/**@addtogroup NRF_NVIC_INTERNAL_FUNCTIONS SoftDevice NVIC internal functions
 * @{ */

/**@brief Disables IRQ interrupts globally, including the SoftDevice's interrupts.
 *
 * @retval  The value of PRIMASK prior to disabling the interrupts.
 */
__STATIC_INLINE int __sd_nvic_irq_disable(void);

/**@brief Enables IRQ interrupts globally, including the SoftDevice's interrupts.
 */
__STATIC_INLINE void __sd_nvic_irq_enable(void);

/**@brief Checks if IRQn is available to application
 * @param[in]  IRQn  IRQ to check
 *
 * @retval  1 (true) if the IRQ to check is available to the application
 */
__STATIC_INLINE uint32_t __sd_nvic_app_accessible_irq(IRQn_Type IRQn);

/**@brief Checks if priority is available to application
 * @param[in]  priority  priority to check
 *
 * @retval  1 (true) if the priority to check is available to the application
 */
__STATIC_INLINE uint32_t __sd_nvic_is_app_accessible_priority(uint32_t priority);

/**@} */

/**@addtogroup NRF_NVIC_FUNCTIONS SoftDevice NVIC public functions
 * @{ */

/**@brief Enable External Interrupt.
 * @note Corresponds to NVIC_EnableIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in] IRQn See the NVIC_EnableIRQ documentation in CMSIS.
 *
 * @retval ::NRF_SUCCESS The interrupt was enabled.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE The interrupt is not available for the application.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED The interrupt has a priority not available for the application.
 */
__STATIC_INLINE uint32_t sd_nvic_EnableIRQ(IRQn_Type IRQn);

/**@brief  Disable External Interrupt.
 * @note Corresponds to NVIC_DisableIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in] IRQn See the NVIC_DisableIRQ documentation in CMSIS.
 *
 * @retval ::NRF_SUCCESS The interrupt was disabled.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE The interrupt is not available for the application.
 */
__STATIC_INLINE uint32_t sd_nvic_DisableIRQ(IRQn_Type IRQn);

/**@brief  Get Pending Interrupt.
 * @note Corresponds to NVIC_GetPendingIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in]   IRQn          See the NVIC_GetPendingIRQ documentation in CMSIS.
 * @param[out]  p_pending_irq Return value from NVIC_GetPendingIRQ.
 *
 * @retval ::NRF_SUCCESS The interrupt is available for the application.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE IRQn is not available for the application.
 */
__STATIC_INLINE uint32_t sd_nvic_GetPendingIRQ(IRQn_Type IRQn, uint32_t *p_pending_irq);

/**@brief  Set Pending Interrupt.
 * @note Corresponds to NVIC_SetPendingIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in] IRQn See the NVIC_SetPendingIRQ documentation in CMSIS.
 *
 * @retval ::NRF_SUCCESS The interrupt is set pending.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE IRQn is not available for the application.
 */
__STATIC_INLINE uint32_t sd_nvic_SetPendingIRQ(IRQn_Type IRQn);

/**@brief  Clear Pending Interrupt.
 * @note Corresponds to NVIC_ClearPendingIRQ in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in] IRQn See the NVIC_ClearPendingIRQ documentation in CMSIS.
 *
 * @retval ::NRF_SUCCESS The interrupt pending flag is cleared.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE IRQn is not available for the application.
 */
__STATIC_INLINE uint32_t sd_nvic_ClearPendingIRQ(IRQn_Type IRQn);

/**@brief Set Interrupt Priority.
 * @note Corresponds to NVIC_SetPriority in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 * @pre Priority is valid and not reserved by the stack.
 *
 * @param[in] IRQn      See the NVIC_SetPriority documentation in CMSIS.
 * @param[in] priority  A valid IRQ priority for use by the application.
 *
 * @retval ::NRF_SUCCESS The interrupt and priority level is available for the application.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE IRQn is not available for the application.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED The interrupt priority is not available for the application.
 */
__STATIC_INLINE uint32_t sd_nvic_SetPriority(IRQn_Type IRQn, uint32_t priority);

/**@brief Get Interrupt Priority.
 * @note Corresponds to NVIC_GetPriority in CMSIS.
 *
 * @pre IRQn is valid and not reserved by the stack.
 *
 * @param[in]  IRQn         See the NVIC_GetPriority documentation in CMSIS.
 * @param[out] p_priority   Return value from NVIC_GetPriority.
 *
 * @retval ::NRF_SUCCESS The interrupt priority is returned in p_priority.
 * @retval ::NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE - IRQn is not available for the application.
 */
__STATIC_INLINE uint32_t sd_nvic_GetPriority(IRQn_Type IRQn, uint32_t *p_priority);

/**@brief System Reset.
 * @note Corresponds to NVIC_SystemReset in CMSIS.
 *
 * @retval ::NRF_ERROR_SOC_NVIC_SHOULD_NOT_RETURN
 */
__STATIC_INLINE uint32_t sd_nvic_SystemReset(void);

/**@brief Enter critical region.
 *
 * @post Application interrupts will be disabled.
 * @note sd_nvic_critical_region_enter() and ::sd_nvic_critical_region_exit() must be called in matching pairs inside each
 * execution context
 * @sa sd_nvic_critical_region_exit
 *
 * @param[out] p_is_nested_critical_region If 1, the application is now in a nested critical region.
 *
 * @retval ::NRF_SUCCESS
 */
__STATIC_INLINE uint32_t sd_nvic_critical_region_enter(uint8_t *p_is_nested_critical_region);

/**@brief Exit critical region.
 *
 * @pre Application has entered a critical region using ::sd_nvic_critical_region_enter.
 * @post If not in a nested critical region, the application interrupts will restored to the state before ::sd_nvic_critical_region_enter was called.
 *
 * @param[in] is_nested_critical_region If this is set to 1, the critical region won't be exited. @sa sd_nvic_critical_region_enter.
 *
 * @retval ::NRF_SUCCESS
 */
__STATIC_INLINE uint32_t sd_nvic_critical_region_exit(uint8_t is_nested_critical_region);

/**@} */

#ifndef SUPPRESS_INLINE_IMPLEMENTATION

__STATIC_INLINE int __sd_nvic_irq_disable(void) {
    int pm = __get_PRIMASK();
    __disable_irq();
    return pm;
}

__STATIC_INLINE void __sd_nvic_irq_enable(void) {
    __enable_irq();
}

__STATIC_INLINE uint32_t __sd_nvic_app_accessible_irq(IRQn_Type IRQn) {
    if (IRQn < 32) {
        return ((1UL << IRQn) & __NRF_NVIC_APP_IRQS_0) != 0;
    } else if (IRQn < 64) {
        return ((1UL << (IRQn - 32)) & __NRF_NVIC_APP_IRQS_1) != 0;
    } else {
        return 1;
    }
}

__STATIC_INLINE uint32_t __sd_nvic_is_app_accessible_priority(uint32_t priority) {
    if ((priority >= (1 << __NVIC_PRIO_BITS))
        || (((1 << priority) & __NRF_NVIC_APP_IRQ_PRIOS) == 0)
        ) {
        return 0;
    }
    return 1;
}


__STATIC_INLINE uint32_t sd_nvic_EnableIRQ(IRQn_Type IRQn) {
    if (!__sd_nvic_app_accessible_irq(IRQn)) {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
    }
    if (!__sd_nvic_is_app_accessible_priority(NVIC_GetPriority(IRQn))) {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED;
    }

    if (nrf_nvic_state.__cr_flag) {
        nrf_nvic_state.__irq_masks[(uint32_t)((int32_t)IRQn) >> 5] |= (uint32_t)(1 << ((uint32_t)((int32_t)IRQn) & (uint32_t)0x1F));
    } else {
        NVIC_EnableIRQ(IRQn);
    }
    return NRF_SUCCESS;
}

__STATIC_INLINE uint32_t sd_nvic_DisableIRQ(IRQn_Type IRQn) {
    if (!__sd_nvic_app_accessible_irq(IRQn)) {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
    }

    if (nrf_nvic_state.__cr_flag) {
        nrf_nvic_state.__irq_masks[(uint32_t)((int32_t)IRQn) >> 5] &= ~(1UL << ((uint32_t)(IRQn) & 0x1F));
    } else {
        NVIC_DisableIRQ(IRQn);
    }

    return NRF_SUCCESS;
}

__STATIC_INLINE uint32_t sd_nvic_GetPendingIRQ(IRQn_Type IRQn, uint32_t *p_pending_irq) {
    if (__sd_nvic_app_accessible_irq(IRQn)) {
        *p_pending_irq = NVIC_GetPendingIRQ(IRQn);
        return NRF_SUCCESS;
    } else {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
    }
}

__STATIC_INLINE uint32_t sd_nvic_SetPendingIRQ(IRQn_Type IRQn) {
    if (__sd_nvic_app_accessible_irq(IRQn)) {
        NVIC_SetPendingIRQ(IRQn);
        return NRF_SUCCESS;
    } else {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
    }
}

__STATIC_INLINE uint32_t sd_nvic_ClearPendingIRQ(IRQn_Type IRQn) {
    if (__sd_nvic_app_accessible_irq(IRQn)) {
        NVIC_ClearPendingIRQ(IRQn);
        return NRF_SUCCESS;
    } else {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
    }
}

__STATIC_INLINE uint32_t sd_nvic_SetPriority(IRQn_Type IRQn, uint32_t priority) {
    if (!__sd_nvic_app_accessible_irq(IRQn)) {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
    }

    if (!__sd_nvic_is_app_accessible_priority(priority)) {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED;
    }

    NVIC_SetPriority(IRQn, (uint32_t)priority);
    return NRF_SUCCESS;
}

__STATIC_INLINE uint32_t sd_nvic_GetPriority(IRQn_Type IRQn, uint32_t *p_priority) {
    if (__sd_nvic_app_accessible_irq(IRQn)) {
        *p_priority = (NVIC_GetPriority(IRQn) & 0xFF);
        return NRF_SUCCESS;
    } else {
        return NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE;
    }
}

__STATIC_INLINE uint32_t sd_nvic_SystemReset(void) {
    NVIC_SystemReset();
    return NRF_ERROR_SOC_NVIC_SHOULD_NOT_RETURN;
}

__STATIC_INLINE uint32_t sd_nvic_critical_region_enter(uint8_t *p_is_nested_critical_region) {
    int was_masked = __sd_nvic_irq_disable();
    if (!nrf_nvic_state.__cr_flag) {
        nrf_nvic_state.__cr_flag = 1;
        nrf_nvic_state.__irq_masks[0] = (NVIC->ICER[0] & __NRF_NVIC_APP_IRQS_0);
        NVIC->ICER[0] = __NRF_NVIC_APP_IRQS_0;
        nrf_nvic_state.__irq_masks[1] = (NVIC->ICER[1] & __NRF_NVIC_APP_IRQS_1);
        NVIC->ICER[1] = __NRF_NVIC_APP_IRQS_1;
        *p_is_nested_critical_region = 0;
    } else {
        *p_is_nested_critical_region = 1;
    }
    if (!was_masked) {
        __sd_nvic_irq_enable();
    }
    return NRF_SUCCESS;
}

__STATIC_INLINE uint32_t sd_nvic_critical_region_exit(uint8_t is_nested_critical_region) {
    if (nrf_nvic_state.__cr_flag && (is_nested_critical_region == 0)) {
        int was_masked = __sd_nvic_irq_disable();
        NVIC->ISER[0] = nrf_nvic_state.__irq_masks[0];
        NVIC->ISER[1] = nrf_nvic_state.__irq_masks[1];
        nrf_nvic_state.__cr_flag = 0;
        if (!was_masked) {
            __sd_nvic_irq_enable();
        }
    }

    return NRF_SUCCESS;
}

#endif /* SUPPRESS_INLINE_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif // NRF_NVIC_H__

/**@} */

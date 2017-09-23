/**
 * \file
 *
 * \brief Delay related functionality declaration.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _HPL_DELAY_H_INCLUDED
#define _HPL_DELAY_H_INCLUDED

/**
 * \addtogroup HPL Delay
 *
 * \section hpl_delay_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#ifndef _UNIT_TEST_
#include <compiler.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name HPL functions
 */
//@{

/**
 * \brief Initialize delay functionality
 *
 * \param[in] hw The pointer to hardware instance
 */
void _delay_init(void *const hw);

/**
 * \brief Retrieve the amount of cycles to delay for the given amount of us
 *
 * \param[in] us The amount of us to delay for
 *
 * \return The amount of cycles
 */
uint32_t _get_cycles_for_us(const uint16_t us);

/**
 * \brief Retrieve the amount of cycles to delay for the given amount of ms
 *
 * \param[in] ms The amount of ms to delay for
 *
 * \return The amount of cycles
 */
uint32_t _get_cycles_for_ms(const uint16_t ms);

/**
 * \brief Delay loop to delay n number of cycles
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] cycles The amount of cycles to delay for
 */
void _delay_cycles(void *const hw, uint32_t cycles);
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_DELAY_H_INCLUDED */

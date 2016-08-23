/**
 * \file
 *
 * \brief Arch file for SAM0.
 *
 * This file defines common SAM0 series.
 *
 * Copyright (C) 2012-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _SAM_IO_
#define _SAM_IO_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* SAM D20 family */
#if (SAMD20)
#  include "samd20.h"
#endif

#if (SAMD21)
#  include "samd21.h"
#endif

#if (SAMR21)
#  include "samr21.h"
#endif

#if (SAMD09)
#  include "samd09.h"
#endif

#if (SAMD10)
#  include "samd10.h"
#endif

#if (SAMD11)
#  include "samd11.h"
#endif

#if (SAML21)
#  include "saml21.h"
#endif

#if (SAMR30)
#  include "samr30.h"
#endif

#if (SAML22)
#  include "saml22.h"
#endif

#if (SAMDA1)
#  include "samda1.h"
#endif

#if (SAMC20)
#  include "samc20.h"
#endif

#if (SAMC21)
#  include "samc21.h"
#endif

#if (SAMB11)
#  include "samb11.h"
#endif

#endif /* _SAM_IO_ */

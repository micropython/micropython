/*
**
** mcapi_config.c source file generated on January 25, 2019 at 09:17:28.	
**
** Copyright (C) 2000-2019 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected in 
** the MCAPI configuration editor. Changes to the MCAPI configuration should
** be made by changing the appropriate options rather than editing this file.
**
** For descriptions of each parameter, please refer to the CCES product Help.
**
*/

#include <services/mcapi/mcapi.h>
#include <adi_types.h>
#include <stddef.h>

#if defined (_MISRA_RULES)
#pragma diag(suppress:misra_rule_8_8:\
             "An external object or function shall be declared in one and only one file")
#pragma diag(suppress:misra_rule_8_10:\
             "All declarations and definitions of objects or functions at file scope\
             shall have internal linkage unless external linkage is required")
#pragma diag(suppress:misra_rule_19_4: \
             "This file needs macros that don't follow the MISRA format")
#pragma diag(suppress:misra_rule_19_10:\
             "This file needs macros that look like functions")
             
#endif /* _MISRA_RULES */

#define PRAGMA(x) _Pragma(#x)
#define ATTRIBUTE(x) __attribute__((x))
#define SECTION_PRAGMA(my_section) PRAGMA(section(#my_section))
#define SECTION_ATTRIBUTE(my_section) ATTRIBUTE(section(#my_section))

#if defined (__GNUC__)
#define ALIGNED_4_ATTRIBUTE ATTRIBUTE(aligned(4))
#define ALIGNED_4_PRAGMA
#define UNCACHED_DATA_ATTRIBUTE SECTION_ATTRIBUTE(.l2_uncached_data)
#define UNCACHED_DATA_PRAGMA
#else
#define ALIGNED_4_ATTRIBUTE
#define ALIGNED_4_PRAGMA PRAGMA(align(4))
#if defined (__ADSP215xx__)
#define UNCACHED_DATA_PRAGMA SECTION_PRAGMA(seg_l2_uncached)
#else
#define UNCACHED_DATA_PRAGMA SECTION_PRAGMA(L2_sram_uncached)
#endif
#define UNCACHED_DATA_ATTRIBUTE
#endif

int32_t adi_mcapi_Init(void);

const mcapi_uint32_t _adi_mcapi_numPorts = (24u + 8u);
const mcapi_uint32_t _adi_mcapi_numAnonPorts = 8u;

ALIGNED_4_PRAGMA
int8_t  _adi_mcapi_vLocalEndptSpace[(24u + 8u)*_ADI_MCAPI_ENDPOINT_SIZE] ALIGNED_4_ATTRIBUTE;
size_t _adi_mcapi_LocalEndptSpaceSize = sizeof(_adi_mcapi_vLocalEndptSpace);

const mcapi_uint32_t _adi_mcapi_num_api_requests = 31u;

ALIGNED_4_PRAGMA
int8_t _adi_mcapi_vAPIRequestSpace[31u*_ADI_MCAPI_REQUEST_SIZE] ALIGNED_4_ATTRIBUTE;
size_t _adi_mcapi_APIRequestSpaceSize = sizeof(_adi_mcapi_vAPIRequestSpace);

const mcapi_uint32_t _adi_mcapi_num_rx_buffers = 1u;
const mcapi_uint32_t _adi_mcapi_rx_buffer_size = 256u;

ALIGNED_4_PRAGMA
int8_t _adi_mcapi_vReceiveBufferSpace[1u * 256u] ALIGNED_4_ATTRIBUTE;

#if defined (__ADSP215xx__) || (defined(__NUM_CORES__) && (__NUM_CORES__ > 1)) 

const mcapi_uint32_t _adi_mcapi_num_isr_requests = 16u;

ALIGNED_4_PRAGMA
int8_t _adi_mcapi_vISRRequestSpace[16u*_ADI_MCAPI_REQUEST_SIZE] ALIGNED_4_ATTRIBUTE;
size_t _adi_mcapi_ISRRequestSpaceSize = sizeof(_adi_mcapi_vISRRequestSpace);

const mcapi_uint32_t _adi_mcapi_num_tx_buffers = 16u;
const mcapi_uint32_t _adi_mcapi_tx_buffer_size = 256u;

UNCACHED_DATA_PRAGMA
ALIGNED_4_PRAGMA
int8_t _adi_mcapi_vTransmitBufferSpace[16u * 256u] UNCACHED_DATA_ATTRIBUTE ALIGNED_4_ATTRIBUTE;

#endif /* __NUM_CORES__ */

/*
 * API called by adi_initComponents to initialize MCAPI. The parameters to mcapi_initialize 
 * are fixed and cannot be modified in any way. 
 */

mcapi_info_t  adi_mcapi_info;
mcapi_param_t adi_mcapi_init_parameters;

int32_t adi_mcapi_Init(void)
{
	mcapi_status_t mcapi_status;
	int32_t result = 0;

	/* Initialize MCAPI */

	mcapi_initialize(
		0u,                         /* the domain is always 0 */
		0u,                         /* Node ID */
		NULL,                       /* MCAPI node attributes */
		&adi_mcapi_init_parameters, /* MCAPI init parameters */
		&adi_mcapi_info,
		&mcapi_status );

	/* MCAPI status codes begin with 1 for MCAPI_SUCCESS, so it can't be 
	 * returned as a zero == success.
	 */
	if (MCAPI_SUCCESS != mcapi_status)
	{
		 result = mcapi_status; 
	} 

	return (result);
}


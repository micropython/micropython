/*****************************************************************************
* pppdebug.h - System debugging utilities.
*
* Copyright (c) 2003 by Marc Boucher, Services Informatiques (MBSI) inc.
* portions Copyright (c) 1998 Global Election Systems Inc.
* portions Copyright (c) 2001 by Cognizant Pty Ltd.
*
* The authors hereby grant permission to use, copy, modify, distribute,
* and license this software and its documentation for any purpose, provided
* that existing copyright notices are retained in all copies and that this
* notice and the following disclaimer are included verbatim in any 
* distributions. No written agreement, license, or royalty fee is required
* for any of the authorized uses.
*
* THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS *AS IS* AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
* IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
* REVISION HISTORY (please don't use tabs!)
*
* 03-01-01 Marc Boucher <marc@mbsi.ca>
*   Ported to lwIP.
* 98-07-29 Guy Lancaster <lancasterg@acm.org>, Global Election Systems Inc.
*   Original.
*
*****************************************************************************
*/
#ifndef PPPDEBUG_H
#define PPPDEBUG_H

/************************
*** PUBLIC DATA TYPES ***
************************/
/* Trace levels. */
typedef enum {
LOG_CRITICAL = 0,
LOG_ERR      = 1,
LOG_NOTICE   = 2,
LOG_WARNING  = 3,
LOG_INFO     = 5,
LOG_DETAIL   = 6,
LOG_DEBUG    = 7
} LogCodes;


/***********************
*** PUBLIC FUNCTIONS ***
***********************/
/*
 * ppp_trace - a form of printf to send tracing information to stderr
 */
void ppp_trace(int level, const char *format,...);

#define TRACELCP PPP_DEBUG

#if PPP_DEBUG

#define AUTHDEBUG(a) ppp_trace a
#define IPCPDEBUG(a) ppp_trace a
#define UPAPDEBUG(a) ppp_trace a
#define LCPDEBUG(a)  ppp_trace a
#define FSMDEBUG(a)  ppp_trace a
#define CHAPDEBUG(a) ppp_trace a
#define PPPDEBUG(a)  ppp_trace a

#else /* PPP_DEBUG */

#define AUTHDEBUG(a)
#define IPCPDEBUG(a)
#define UPAPDEBUG(a)
#define LCPDEBUG(a)
#define FSMDEBUG(a)
#define CHAPDEBUG(a)
#define PPPDEBUG(a)

#endif /* PPP_DEBUG */

#endif /* PPPDEBUG_H */

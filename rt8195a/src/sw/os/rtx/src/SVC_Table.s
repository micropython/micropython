;/*----------------------------------------------------------------------------
; *      RL-ARM - RTX
; *----------------------------------------------------------------------------
; *      Name:    SVC_TABLE.S
; *      Purpose: Pre-defined SVC Table for Cortex-M
; *      Rev.:    V4.60
; *----------------------------------------------------------------------------
; *
; * Copyright (c) 1999-2009 KEIL, 2009-2012 ARM Germany GmbH
; * All rights reserved.
; * Redistribution and use in source and binary forms, with or without
; * modification, are permitted provided that the following conditions are met:
; *  - Redistributions of source code must retain the above copyright
; *    notice, this list of conditions and the following disclaimer.
; *  - Redistributions in binary form must reproduce the above copyright
; *    notice, this list of conditions and the following disclaimer in the
; *    documentation and/or other materials provided with the distribution.
; *  - Neither the name of ARM  nor the names of its contributors may be used 
; *    to endorse or promote products derived from this software without 
; *    specific prior written permission.
; *
; * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
; * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
; * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
; * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
; * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
; * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
; * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
; * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; * POSSIBILITY OF SUCH DAMAGE.
; *---------------------------------------------------------------------------*/


        .file   "SVC_Table.S"


        .section ".svc_table"

        .global  SVC_Table
SVC_Table:
/* Insert user SVC functions here. SVC 0 used by RTL Kernel. */
#       .long   __SVC_1                 /* user SVC function */
SVC_End:

        .global  SVC_Count
SVC_Count:
        .long   (SVC_End-SVC_Table)/4


        .end

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

//* ----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//* ----------------------------------------------------------------------------
//* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
//* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
//* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
//* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
//* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//* ----------------------------------------------------------------------------
//* File Name           : lib_AT91SAM7X256.h
//* Object              : AT91SAM7X256 inlined functions
//* Generated           : AT91 SW Application Group  11/02/2005 (15:17:24)
//*
//* CVS Reference       : /lib_dbgu.h/1.1/Thu Aug 25 12:56:22 2005//
//* CVS Reference       : /lib_pmc_SAM7X.h/1.4/Tue Aug 30 13:00:36 2005//
//* CVS Reference       : /lib_VREG_6085B.h/1.1/Tue Feb  1 16:20:47 2005//
//* CVS Reference       : /lib_rstc_6098A.h/1.1/Wed Oct  6 10:39:20 2004//
//* CVS Reference       : /lib_ssc.h/1.4/Fri Jan 31 12:19:20 2003//
//* CVS Reference       : /lib_wdtc_6080A.h/1.1/Wed Oct  6 10:38:30 2004//
//* CVS Reference       : /lib_usart.h/1.5/Thu Nov 21 16:01:54 2002//
//* CVS Reference       : /lib_spi2.h/1.2/Tue Aug 23 15:37:28 2005//
//* CVS Reference       : /lib_pitc_6079A.h/1.2/Tue Nov  9 14:43:56 2004//
//* CVS Reference       : /lib_aic_6075b.h/1.2/Thu Jul  7 07:48:22 2005//
//* CVS Reference       : /lib_twi.h/1.3/Mon Jul 19 14:27:58 2004//
//* CVS Reference       : /lib_adc.h/1.6/Fri Oct 17 09:12:38 2003//
//* CVS Reference       : /lib_rttc_6081A.h/1.1/Wed Oct  6 10:39:38 2004//
//* CVS Reference       : /lib_udp.h/1.5/Tue Aug 30 12:13:47 2005//
//* CVS Reference       : /lib_tc_1753b.h/1.1/Fri Jan 31 12:20:02 2003//
//* CVS Reference       : /lib_MC_SAM7X.h/1.1/Thu Mar 25 15:19:14 2004//
//* CVS Reference       : /lib_pio.h/1.3/Fri Jan 31 12:18:56 2003//
//* CVS Reference       : /lib_can_AT91.h/1.5/Tue Aug 23 15:37:07 2005//
//* CVS Reference       : /lib_PWM_SAM.h/1.3/Thu Jan 22 10:10:50 2004//
//* CVS Reference       : /lib_pdc.h/1.2/Tue Jul  2 13:29:40 2002//
//* ----------------------------------------------------------------------------

#ifndef lib_AT91SAM7X256_H
#define lib_AT91SAM7X256_H

/* *****************************************************************************
                SOFTWARE API FOR AIC
   ***************************************************************************** */
#define AT91C_AIC_BRANCH_OPCODE ((void (*) ()) 0xE51FFF20) // ldr, pc, [pc, #-&F20]

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_ConfigureIt
//* \brief Interrupt Handler Initialization
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_AIC_ConfigureIt (
	AT91PS_AIC pAic,  // \arg pointer to the AIC registers
	unsigned int irq_id,     // \arg interrupt number to initialize
	unsigned int priority,   // \arg priority to give to the interrupt
	unsigned int src_type,   // \arg activation and sense of activation
	void (*newHandler) () ) // \arg address of the interrupt handler
{
	unsigned int oldHandler;
    unsigned int mask ;

    oldHandler = pAic->AIC_SVR[irq_id];

    mask = 0x1 << irq_id ;
    //* Disable the interrupt on the interrupt controller
    pAic->AIC_IDCR = mask ;
    //* Save the interrupt handler routine pointer and the interrupt priority
    pAic->AIC_SVR[irq_id] = (unsigned int) newHandler ;
    //* Store the Source Mode Register
    pAic->AIC_SMR[irq_id] = src_type | priority  ;
    //* Clear the interrupt on the interrupt controller
    pAic->AIC_ICCR = mask ;

	return oldHandler;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_EnableIt
//* \brief Enable corresponding IT number
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_EnableIt (
	AT91PS_AIC pAic,      // \arg pointer to the AIC registers
	unsigned int irq_id ) // \arg interrupt number to initialize
{
    //* Enable the interrupt on the interrupt controller
    pAic->AIC_IECR = 0x1 << irq_id ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_DisableIt
//* \brief Disable corresponding IT number
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_DisableIt (
	AT91PS_AIC pAic,      // \arg pointer to the AIC registers
	unsigned int irq_id ) // \arg interrupt number to initialize
{
    unsigned int mask = 0x1 << irq_id;
    //* Disable the interrupt on the interrupt controller
    pAic->AIC_IDCR = mask ;
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    pAic->AIC_ICCR = mask ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_ClearIt
//* \brief Clear corresponding IT number
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_ClearIt (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg interrupt number to initialize
{
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    pAic->AIC_ICCR = (0x1 << irq_id);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_AcknowledgeIt
//* \brief Acknowledge corresponding IT number
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_AcknowledgeIt (
	AT91PS_AIC pAic)     // \arg pointer to the AIC registers
{
    pAic->AIC_EOICR = pAic->AIC_EOICR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_SetExceptionVector
//* \brief Configure vector handler
//*----------------------------------------------------------------------------
__inline unsigned int  AT91F_AIC_SetExceptionVector (
	unsigned int *pVector, // \arg pointer to the AIC registers
	void (*Handler) () )   // \arg Interrupt Handler
{
	unsigned int oldVector = *pVector;

	if ((unsigned int) Handler == (unsigned int) AT91C_AIC_BRANCH_OPCODE)
		*pVector = (unsigned int) AT91C_AIC_BRANCH_OPCODE;
	else
		*pVector = (((((unsigned int) Handler) - ((unsigned int) pVector) - 0x8) >> 2) & 0x00FFFFFF) | 0xEA000000;

	return oldVector;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_Trig
//* \brief Trig an IT
//*----------------------------------------------------------------------------
__inline void  AT91F_AIC_Trig (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg interrupt number
{
	pAic->AIC_ISCR = (0x1 << irq_id) ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_IsActive
//* \brief Test if an IT is active
//*----------------------------------------------------------------------------
__inline unsigned int  AT91F_AIC_IsActive (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg Interrupt Number
{
	return (pAic->AIC_ISR & (0x1 << irq_id));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_IsPending
//* \brief Test if an IT is pending
//*----------------------------------------------------------------------------
__inline unsigned int  AT91F_AIC_IsPending (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg Interrupt Number
{
	return (pAic->AIC_IPR & (0x1 << irq_id));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_Open
//* \brief Set exception vectors and AIC registers to default values
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_Open(
	AT91PS_AIC pAic,        // \arg pointer to the AIC registers
	void (*IrqHandler) (),  // \arg Default IRQ vector exception
	void (*FiqHandler) (),  // \arg Default FIQ vector exception
	void (*DefaultHandler)  (), // \arg Default Handler set in ISR
	void (*SpuriousHandler) (), // \arg Default Spurious Handler
	unsigned int protectMode)   // \arg Debug Control Register
{
	int i;

	// Disable all interrupts and set IVR to the default handler
	for (i = 0; i < 32; ++i) {
		AT91F_AIC_DisableIt(pAic, i);
		AT91F_AIC_ConfigureIt(pAic, i, AT91C_AIC_PRIOR_LOWEST, AT91C_AIC_SRCTYPE_HIGH_LEVEL, DefaultHandler);
	}

	// Set the IRQ exception vector
	AT91F_AIC_SetExceptionVector((unsigned int *) 0x18, IrqHandler);
	// Set the Fast Interrupt exception vector
	AT91F_AIC_SetExceptionVector((unsigned int *) 0x1C, FiqHandler);

	pAic->AIC_SPU = (unsigned int) SpuriousHandler;
	pAic->AIC_DCR = protectMode;
}
/* *****************************************************************************
                SOFTWARE API FOR PDC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetNextRx
//* \brief Set the next receive transfer descriptor
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_SetNextRx (
	AT91PS_PDC pPDC,     // \arg pointer to a PDC controller
	char *address,       // \arg address to the next bloc to be received
	unsigned int bytes)  // \arg number of bytes to be received
{
	pPDC->PDC_RNPR = (unsigned int) address;
	pPDC->PDC_RNCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetNextTx
//* \brief Set the next transmit transfer descriptor
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_SetNextTx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be transmitted
	unsigned int bytes)    // \arg number of bytes to be transmitted
{
	pPDC->PDC_TNPR = (unsigned int) address;
	pPDC->PDC_TNCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetRx
//* \brief Set the receive transfer descriptor
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_SetRx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be received
	unsigned int bytes)    // \arg number of bytes to be received
{
	pPDC->PDC_RPR = (unsigned int) address;
	pPDC->PDC_RCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetTx
//* \brief Set the transmit transfer descriptor
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_SetTx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be transmitted
	unsigned int bytes)    // \arg number of bytes to be transmitted
{
	pPDC->PDC_TPR = (unsigned int) address;
	pPDC->PDC_TCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_EnableTx
//* \brief Enable transmit
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_EnableTx (
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	pPDC->PDC_PTCR = AT91C_PDC_TXTEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_EnableRx
//* \brief Enable receive
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_EnableRx (
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	pPDC->PDC_PTCR = AT91C_PDC_RXTEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_DisableTx
//* \brief Disable transmit
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_DisableTx (
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	pPDC->PDC_PTCR = AT91C_PDC_TXTDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_DisableRx
//* \brief Disable receive
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_DisableRx (
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	pPDC->PDC_PTCR = AT91C_PDC_RXTDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_IsTxEmpty
//* \brief Test if the current transfer descriptor has been sent
//*----------------------------------------------------------------------------
__inline int AT91F_PDC_IsTxEmpty ( // \return return 1 if transfer is complete
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	return !(pPDC->PDC_TCR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_IsNextTxEmpty
//* \brief Test if the next transfer descriptor has been moved to the current td
//*----------------------------------------------------------------------------
__inline int AT91F_PDC_IsNextTxEmpty ( // \return return 1 if transfer is complete
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	return !(pPDC->PDC_TNCR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_IsRxEmpty
//* \brief Test if the current transfer descriptor has been filled
//*----------------------------------------------------------------------------
__inline int AT91F_PDC_IsRxEmpty ( // \return return 1 if transfer is complete
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	return !(pPDC->PDC_RCR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_IsNextRxEmpty
//* \brief Test if the next transfer descriptor has been moved to the current td
//*----------------------------------------------------------------------------
__inline int AT91F_PDC_IsNextRxEmpty ( // \return return 1 if transfer is complete
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	return !(pPDC->PDC_RNCR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_Open
//* \brief Open PDC: disable TX and RX reset transfer descriptors, re-enable RX and TX
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_Open (
	AT91PS_PDC pPDC)       // \arg pointer to a PDC controller
{
    //* Disable the RX and TX PDC transfer requests
	AT91F_PDC_DisableRx(pPDC);
	AT91F_PDC_DisableTx(pPDC);

	//* Reset all Counter register Next buffer first
	AT91F_PDC_SetNextTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetNextRx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetRx(pPDC, (char *) 0, 0);

    //* Enable the RX and TX PDC transfer requests
	AT91F_PDC_EnableRx(pPDC);
	AT91F_PDC_EnableTx(pPDC);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_Close
//* \brief Close PDC: disable TX and RX reset transfer descriptors
//*----------------------------------------------------------------------------
__inline void AT91F_PDC_Close (
	AT91PS_PDC pPDC)       // \arg pointer to a PDC controller
{
    //* Disable the RX and TX PDC transfer requests
	AT91F_PDC_DisableRx(pPDC);
	AT91F_PDC_DisableTx(pPDC);

	//* Reset all Counter register Next buffer first
	AT91F_PDC_SetNextTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetNextRx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetRx(pPDC, (char *) 0, 0);

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SendFrame
//* \brief Close PDC: disable TX and RX reset transfer descriptors
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PDC_SendFrame(
	AT91PS_PDC pPDC,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	if (AT91F_PDC_IsTxEmpty(pPDC)) {
		//* Buffer and next buffer can be initialized
		AT91F_PDC_SetTx(pPDC, pBuffer, szBuffer);
		AT91F_PDC_SetNextTx(pPDC, pNextBuffer, szNextBuffer);
		return 2;
	}
	else if (AT91F_PDC_IsNextTxEmpty(pPDC)) {
		//* Only one buffer can be initialized
		AT91F_PDC_SetNextTx(pPDC, pBuffer, szBuffer);
		return 1;
	}
	else {
		//* All buffer are in use...
		return 0;
	}
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_ReceiveFrame
//* \brief Close PDC: disable TX and RX reset transfer descriptors
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PDC_ReceiveFrame (
	AT91PS_PDC pPDC,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	if (AT91F_PDC_IsRxEmpty(pPDC)) {
		//* Buffer and next buffer can be initialized
		AT91F_PDC_SetRx(pPDC, pBuffer, szBuffer);
		AT91F_PDC_SetNextRx(pPDC, pNextBuffer, szNextBuffer);
		return 2;
	}
	else if (AT91F_PDC_IsNextRxEmpty(pPDC)) {
		//* Only one buffer can be initialized
		AT91F_PDC_SetNextRx(pPDC, pBuffer, szBuffer);
		return 1;
	}
	else {
		//* All buffer are in use...
		return 0;
	}
}
/* *****************************************************************************
                SOFTWARE API FOR DBGU
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_InterruptEnable
//* \brief Enable DBGU Interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_DBGU_InterruptEnable(
        AT91PS_DBGU pDbgu,   // \arg  pointer to a DBGU controller
        unsigned int flag) // \arg  dbgu interrupt to be enabled
{
        pDbgu->DBGU_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_InterruptDisable
//* \brief Disable DBGU Interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_DBGU_InterruptDisable(
        AT91PS_DBGU pDbgu,   // \arg  pointer to a DBGU controller
        unsigned int flag) // \arg  dbgu interrupt to be disabled
{
        pDbgu->DBGU_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_GetInterruptMaskStatus
//* \brief Return DBGU Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_DBGU_GetInterruptMaskStatus( // \return DBGU Interrupt Mask Status
        AT91PS_DBGU pDbgu) // \arg  pointer to a DBGU controller
{
        return pDbgu->DBGU_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_IsInterruptMasked
//* \brief Test if DBGU Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline int AT91F_DBGU_IsInterruptMasked(
        AT91PS_DBGU pDbgu,   // \arg  pointer to a DBGU controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_DBGU_GetInterruptMaskStatus(pDbgu) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR PIO
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgPeriph
//* \brief Enable pins to be drived by peripheral
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgPeriph(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int periphAEnable,  // \arg PERIPH A to enable
	unsigned int periphBEnable)  // \arg PERIPH B to enable

{
	pPio->PIO_ASR = periphAEnable;
	pPio->PIO_BSR = periphBEnable;
	pPio->PIO_PDR = (periphAEnable | periphBEnable); // Set in Periph mode
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgOutput
//* \brief Enable PIO in output mode
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgOutput(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int pioEnable)      // \arg PIO to be enabled
{
	pPio->PIO_PER = pioEnable; // Set in PIO mode
	pPio->PIO_OER = pioEnable; // Configure in Output
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgInput
//* \brief Enable PIO in input mode
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgInput(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int inputEnable)      // \arg PIO to be enabled
{
	// Disable output
	pPio->PIO_ODR  = inputEnable;
	pPio->PIO_PER  = inputEnable;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgOpendrain
//* \brief Configure PIO in open drain
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgOpendrain(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int multiDrvEnable) // \arg pio to be configured in open drain
{
	// Configure the multi-drive option
	pPio->PIO_MDDR = ~multiDrvEnable;
	pPio->PIO_MDER = multiDrvEnable;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgPullup
//* \brief Enable pullup on PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgPullup(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int pullupEnable)   // \arg enable pullup on PIO
{
		// Connect or not Pullup
	pPio->PIO_PPUDR = ~pullupEnable;
	pPio->PIO_PPUER = pullupEnable;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgDirectDrive
//* \brief Enable direct drive on PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgDirectDrive(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int directDrive)    // \arg PIO to be configured with direct drive

{
	// Configure the Direct Drive
	pPio->PIO_OWDR  = ~directDrive;
	pPio->PIO_OWER  = directDrive;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgInputFilter
//* \brief Enable input filter on input PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgInputFilter(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int inputFilter)    // \arg PIO to be configured with input filter

{
	// Configure the Direct Drive
	pPio->PIO_IFDR  = ~inputFilter;
	pPio->PIO_IFER  = inputFilter;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetInput
//* \brief Return PIO input value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetInput( // \return PIO input
	AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
	return pPio->PIO_PDSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsInputSet
//* \brief Test if PIO is input flag is active
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsInputSet(
	AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
	unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_PIO_GetInput(pPio) & flag);
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_SetOutput
//* \brief Set to 1 output PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_SetOutput(
	AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
	unsigned int flag) // \arg  output to be set
{
	pPio->PIO_SODR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_ClearOutput
//* \brief Set to 0 output PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_ClearOutput(
	AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
	unsigned int flag) // \arg  output to be cleared
{
	pPio->PIO_CODR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_ForceOutput
//* \brief Force output when Direct drive option is enabled
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_ForceOutput(
	AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
	unsigned int flag) // \arg  output to be forced
{
	pPio->PIO_ODSR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_Enable
//* \brief Enable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_Enable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio to be enabled 
{
        pPio->PIO_PER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_Disable
//* \brief Disable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_Disable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio to be disabled 
{
        pPio->PIO_PDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetStatus
//* \brief Return PIO Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetStatus( // \return PIO Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_PSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsSet
//* \brief Test if PIO is Set
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_OutputEnable
//* \brief Output Enable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_OutputEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio output to be enabled
{
        pPio->PIO_OER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_OutputDisable
//* \brief Output Enable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_OutputDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio output to be disabled
{
        pPio->PIO_ODR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetOutputStatus
//* \brief Return PIO Output Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetOutputStatus( // \return PIO Output Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_OSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsOuputSet
//* \brief Test if PIO Output is Set
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsOutputSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetOutputStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_InputFilterEnable
//* \brief Input Filter Enable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_InputFilterEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio input filter to be enabled
{
        pPio->PIO_IFER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_InputFilterDisable
//* \brief Input Filter Disable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_InputFilterDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio input filter to be disabled
{
        pPio->PIO_IFDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetInputFilterStatus
//* \brief Return PIO Input Filter Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetInputFilterStatus( // \return PIO Input Filter Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_IFSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsInputFilterSet
//* \brief Test if PIO Input filter is Set
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsInputFilterSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetInputFilterStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetOutputDataStatus
//* \brief Return PIO Output Data Status 
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetOutputDataStatus( // \return PIO Output Data Status 
	AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_ODSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_InterruptEnable
//* \brief Enable PIO Interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_InterruptEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio interrupt to be enabled
{
        pPio->PIO_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_InterruptDisable
//* \brief Disable PIO Interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_InterruptDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio interrupt to be disabled
{
        pPio->PIO_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetInterruptMaskStatus
//* \brief Return PIO Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetInterruptMaskStatus( // \return PIO Interrupt Mask Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetInterruptStatus
//* \brief Return PIO Interrupt Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetInterruptStatus( // \return PIO Interrupt Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_ISR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsInterruptMasked
//* \brief Test if PIO Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsInterruptMasked(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetInterruptMaskStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsInterruptSet
//* \brief Test if PIO Interrupt is Set
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsInterruptSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetInterruptStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_MultiDriverEnable
//* \brief Multi Driver Enable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_MultiDriverEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio to be enabled
{
        pPio->PIO_MDER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_MultiDriverDisable
//* \brief Multi Driver Disable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_MultiDriverDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio to be disabled
{
        pPio->PIO_MDDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetMultiDriverStatus
//* \brief Return PIO Multi Driver Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetMultiDriverStatus( // \return PIO Multi Driver Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_MDSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsMultiDriverSet
//* \brief Test if PIO MultiDriver is Set
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsMultiDriverSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetMultiDriverStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_A_RegisterSelection
//* \brief PIO A Register Selection 
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_A_RegisterSelection(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio A register selection
{
        pPio->PIO_ASR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_B_RegisterSelection
//* \brief PIO B Register Selection 
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_B_RegisterSelection(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio B register selection 
{
        pPio->PIO_BSR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_Get_AB_RegisterStatus
//* \brief Return PIO Interrupt Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_Get_AB_RegisterStatus( // \return PIO AB Register Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_ABSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsAB_RegisterSet
//* \brief Test if PIO AB Register is Set
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsAB_RegisterSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_Get_AB_RegisterStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_OutputWriteEnable
//* \brief Output Write Enable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_OutputWriteEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio output write to be enabled
{
        pPio->PIO_OWER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_OutputWriteDisable
//* \brief Output Write Disable PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_OutputWriteDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio output write to be disabled
{
        pPio->PIO_OWDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetOutputWriteStatus
//* \brief Return PIO Output Write Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetOutputWriteStatus( // \return PIO Output Write Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_OWSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsOutputWriteSet
//* \brief Test if PIO OutputWrite is Set
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsOutputWriteSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetOutputWriteStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetCfgPullup
//* \brief Return PIO Configuration Pullup
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PIO_GetCfgPullup( // \return PIO Configuration Pullup 
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_PPUSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsOutputDataStatusSet
//* \brief Test if PIO Output Data Status is Set 
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsOutputDataStatusSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetOutputDataStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsCfgPullupStatusSet
//* \brief Test if PIO Configuration Pullup Status is Set
//*----------------------------------------------------------------------------
__inline int AT91F_PIO_IsCfgPullupStatusSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (~AT91F_PIO_GetCfgPullup(pPio) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR PMC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgSysClkEnableReg
//* \brief Configure the System Clock Enable Register of the PMC controller
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_CfgSysClkEnableReg (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int mode)
{
	//* Write to the SCER register
	pPMC->PMC_SCER = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgSysClkDisableReg
//* \brief Configure the System Clock Disable Register of the PMC controller
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_CfgSysClkDisableReg (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int mode)
{
	//* Write to the SCDR register
	pPMC->PMC_SCDR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetSysClkStatusReg
//* \brief Return the System Clock Status Register of the PMC controller
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PMC_GetSysClkStatusReg (
	AT91PS_PMC pPMC // pointer to a CAN controller
	)
{
	return pPMC->PMC_SCSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_EnablePeriphClock
//* \brief Enable peripheral clock
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_EnablePeriphClock (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int periphIds)  // \arg IDs of peripherals to enable
{
	pPMC->PMC_PCER = periphIds;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_DisablePeriphClock
//* \brief Disable peripheral clock
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_DisablePeriphClock (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int periphIds)  // \arg IDs of peripherals to enable
{
	pPMC->PMC_PCDR = periphIds;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetPeriphClock
//* \brief Get peripheral clock status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PMC_GetPeriphClock (
	AT91PS_PMC pPMC) // \arg pointer to PMC controller
{
	return pPMC->PMC_PCSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_CfgMainOscillatorReg
//* \brief Cfg the main oscillator
//*----------------------------------------------------------------------------
__inline void AT91F_CKGR_CfgMainOscillatorReg (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int mode)
{
	pCKGR->CKGR_MOR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_GetMainOscillatorReg
//* \brief Cfg the main oscillator
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CKGR_GetMainOscillatorReg (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	return pCKGR->CKGR_MOR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_EnableMainOscillator
//* \brief Enable the main oscillator
//*----------------------------------------------------------------------------
__inline void AT91F_CKGR_EnableMainOscillator(
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_MOR |= AT91C_CKGR_MOSCEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_DisableMainOscillator
//* \brief Disable the main oscillator
//*----------------------------------------------------------------------------
__inline void AT91F_CKGR_DisableMainOscillator (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_MOR &= ~AT91C_CKGR_MOSCEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_CfgMainOscStartUpTime
//* \brief Cfg MOR Register according to the main osc startup time
//*----------------------------------------------------------------------------
__inline void AT91F_CKGR_CfgMainOscStartUpTime (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int startup_time,  // \arg main osc startup time in microsecond (us)
	unsigned int slowClock)  // \arg slowClock in Hz
{
	pCKGR->CKGR_MOR &= ~AT91C_CKGR_OSCOUNT;
	pCKGR->CKGR_MOR |= ((slowClock * startup_time)/(8*1000000)) << 8;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_GetMainClockFreqReg
//* \brief Cfg the main oscillator
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CKGR_GetMainClockFreqReg (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	return pCKGR->CKGR_MCFR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_GetMainClock
//* \brief Return Main clock in Hz
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CKGR_GetMainClock (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int slowClock)  // \arg slowClock in Hz
{
	return ((pCKGR->CKGR_MCFR  & AT91C_CKGR_MAINF) * slowClock) >> 4;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgMCKReg
//* \brief Cfg Master Clock Register
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_CfgMCKReg (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int mode)
{
	pPMC->PMC_MCKR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetMCKReg
//* \brief Return Master Clock Register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PMC_GetMCKReg(
	AT91PS_PMC pPMC) // \arg pointer to PMC controller
{
	return pPMC->PMC_MCKR;
}

//*------------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetMasterClock
//* \brief Return master clock in Hz which correponds to processor clock for ARM7
//*------------------------------------------------------------------------------
__inline unsigned int AT91F_PMC_GetMasterClock (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int slowClock)  // \arg slowClock in Hz
{
	unsigned int reg = pPMC->PMC_MCKR;
	unsigned int prescaler = (1 << ((reg & AT91C_PMC_PRES) >> 2));
	unsigned int pllDivider, pllMultiplier;

	switch (reg & AT91C_PMC_CSS) {
		case AT91C_PMC_CSS_SLOW_CLK: // Slow clock selected
			return slowClock / prescaler;
		case AT91C_PMC_CSS_MAIN_CLK: // Main clock is selected
			return AT91F_CKGR_GetMainClock(pCKGR, slowClock) / prescaler;
		case AT91C_PMC_CSS_PLL_CLK: // PLLB clock is selected
			reg = pCKGR->CKGR_PLLR;
			pllDivider    = (reg  & AT91C_CKGR_DIV);
			pllMultiplier = ((reg  & AT91C_CKGR_MUL) >> 16) + 1;
			return AT91F_CKGR_GetMainClock(pCKGR, slowClock) / pllDivider * pllMultiplier / prescaler;
	}
	return 0;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_EnablePCK
//* \brief Enable peripheral clock
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_EnablePCK (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int pck,  // \arg Peripheral clock identifier 0 .. 7
	unsigned int mode)
{
	pPMC->PMC_PCKR[pck] = mode;
	pPMC->PMC_SCER = (1 << pck) << 8;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_DisablePCK
//* \brief Enable peripheral clock
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_DisablePCK (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int pck)  // \arg Peripheral clock identifier 0 .. 7
{
	pPMC->PMC_SCDR = (1 << pck) << 8;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_EnableIt
//* \brief Enable PMC interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_EnableIt (
	AT91PS_PMC pPMC,     // pointer to a PMC controller
	unsigned int flag)   // IT to be enabled
{
	//* Write to the IER register
	pPMC->PMC_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_DisableIt
//* \brief Disable PMC interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_DisableIt (
	AT91PS_PMC pPMC, // pointer to a PMC controller
	unsigned int flag) // IT to be disabled
{
	//* Write to the IDR register
	pPMC->PMC_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetStatus
//* \brief Return PMC Interrupt Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PMC_GetStatus( // \return PMC Interrupt Status
	AT91PS_PMC pPMC) // pointer to a PMC controller
{
	return pPMC->PMC_SR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetInterruptMaskStatus
//* \brief Return PMC Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PMC_GetInterruptMaskStatus( // \return PMC Interrupt Mask Status
	AT91PS_PMC pPMC) // pointer to a PMC controller
{
	return pPMC->PMC_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_IsInterruptMasked
//* \brief Test if PMC Interrupt is Masked
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PMC_IsInterruptMasked(
        AT91PS_PMC pPMC,   // \arg  pointer to a PMC controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_PMC_GetInterruptMaskStatus(pPMC) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_IsStatusSet
//* \brief Test if PMC Status is Set
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PMC_IsStatusSet(
        AT91PS_PMC pPMC,   // \arg  pointer to a PMC controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_PMC_GetStatus(pPMC) & flag);
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_CKGR_CfgPLLReg
//  \brief Cfg the PLL Register
// ----------------------------------------------------------------------------
__inline void AT91F_CKGR_CfgPLLReg (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int mode)
{
	pCKGR->CKGR_PLLR = mode;
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_CKGR_GetPLLReg
//  \brief Get the PLL Register
// ----------------------------------------------------------------------------
__inline unsigned int AT91F_CKGR_GetPLLReg (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	return pCKGR->CKGR_PLLR;
}


/* *****************************************************************************
                SOFTWARE API FOR RSTC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTSoftReset
//* \brief Start Software Reset
//*----------------------------------------------------------------------------
__inline void AT91F_RSTSoftReset(
        AT91PS_RSTC pRSTC,
        unsigned int reset)
{
	pRSTC->RSTC_RCR = (0xA5000000 | reset);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTSetMode
//* \brief Set Reset Mode
//*----------------------------------------------------------------------------
__inline void AT91F_RSTSetMode(
        AT91PS_RSTC pRSTC,
        unsigned int mode)
{
	pRSTC->RSTC_RMR = (0xA5000000 | mode);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTGetMode
//* \brief Get Reset Mode
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_RSTGetMode(
        AT91PS_RSTC pRSTC)
{
	return (pRSTC->RSTC_RMR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTGetStatus
//* \brief Get Reset Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_RSTGetStatus(
        AT91PS_RSTC pRSTC)
{
	return (pRSTC->RSTC_RSR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTIsSoftRstActive
//* \brief Return !=0 if software reset is still not completed
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_RSTIsSoftRstActive(
        AT91PS_RSTC pRSTC)
{
	return ((pRSTC->RSTC_RSR) & AT91C_RSTC_SRCMP);
}
/* *****************************************************************************
                SOFTWARE API FOR RTTC
   ***************************************************************************** */
//*--------------------------------------------------------------------------------------
//* \fn     AT91F_SetRTT_TimeBase()
//* \brief  Set the RTT prescaler according to the TimeBase in ms
//*--------------------------------------------------------------------------------------
__inline unsigned int AT91F_RTTSetTimeBase(
        AT91PS_RTTC pRTTC, 
        unsigned int ms)
{
	if (ms > 2000)
		return 1;   // AT91C_TIME_OUT_OF_RANGE
	pRTTC->RTTC_RTMR &= ~0xFFFF;	
	pRTTC->RTTC_RTMR |= (((ms << 15) /1000) & 0xFFFF);	
	return 0;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTTSetPrescaler()
//* \brief  Set the new prescaler value
//*--------------------------------------------------------------------------------------
__inline unsigned int AT91F_RTTSetPrescaler(
        AT91PS_RTTC pRTTC, 
        unsigned int rtpres)
{
	pRTTC->RTTC_RTMR &= ~0xFFFF;	
	pRTTC->RTTC_RTMR |= (rtpres & 0xFFFF);	
	return (pRTTC->RTTC_RTMR);
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTTRestart()
//* \brief  Restart the RTT prescaler
//*--------------------------------------------------------------------------------------
__inline void AT91F_RTTRestart(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR |= AT91C_RTTC_RTTRST;	
}


//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_SetAlarmINT()
//* \brief  Enable RTT Alarm Interrupt
//*--------------------------------------------------------------------------------------
__inline void AT91F_RTTSetAlarmINT(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR |= AT91C_RTTC_ALMIEN;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_ClearAlarmINT()
//* \brief  Disable RTT Alarm Interrupt
//*--------------------------------------------------------------------------------------
__inline void AT91F_RTTClearAlarmINT(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR &= ~AT91C_RTTC_ALMIEN;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_SetRttIncINT()
//* \brief  Enable RTT INC Interrupt
//*--------------------------------------------------------------------------------------
__inline void AT91F_RTTSetRttIncINT(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR |= AT91C_RTTC_RTTINCIEN;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_ClearRttIncINT()
//* \brief  Disable RTT INC Interrupt
//*--------------------------------------------------------------------------------------
__inline void AT91F_RTTClearRttIncINT(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR &= ~AT91C_RTTC_RTTINCIEN;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_SetAlarmValue()
//* \brief  Set RTT Alarm Value
//*--------------------------------------------------------------------------------------
__inline void AT91F_RTTSetAlarmValue(
        AT91PS_RTTC pRTTC, unsigned int alarm)
{
	pRTTC->RTTC_RTAR = alarm;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_GetAlarmValue()
//* \brief  Get RTT Alarm Value
//*--------------------------------------------------------------------------------------
__inline unsigned int AT91F_RTTGetAlarmValue(
        AT91PS_RTTC pRTTC)
{
	return(pRTTC->RTTC_RTAR);
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTTGetStatus()
//* \brief  Read the RTT status
//*--------------------------------------------------------------------------------------
__inline unsigned int AT91F_RTTGetStatus(
        AT91PS_RTTC pRTTC)
{
	return(pRTTC->RTTC_RTSR);
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_ReadValue()
//* \brief  Read the RTT value
//*--------------------------------------------------------------------------------------
__inline unsigned int AT91F_RTTReadValue(
        AT91PS_RTTC pRTTC)
{
        register volatile unsigned int val1,val2;
	do
	{
		val1 = pRTTC->RTTC_RTVR;
		val2 = pRTTC->RTTC_RTVR;
	}	
	while(val1 != val2);
	return(val1);
}
/* *****************************************************************************
                SOFTWARE API FOR PITC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PITInit
//* \brief System timer init : period in µsecond, system clock freq in MHz
//*----------------------------------------------------------------------------
__inline void AT91F_PITInit(
        AT91PS_PITC pPITC,
        unsigned int period,
        unsigned int pit_frequency)
{
	pPITC->PITC_PIMR = period? (period * pit_frequency + 8) >> 4 : 0; // +8 to avoid %10 and /10
	pPITC->PITC_PIMR |= AT91C_PITC_PITEN;	 
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITSetPIV
//* \brief Set the PIT Periodic Interval Value 
//*----------------------------------------------------------------------------
__inline void AT91F_PITSetPIV(
        AT91PS_PITC pPITC,
        unsigned int piv)
{
	pPITC->PITC_PIMR = piv | (pPITC->PITC_PIMR & (AT91C_PITC_PITEN | AT91C_PITC_PITIEN));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITEnableInt
//* \brief Enable PIT periodic interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_PITEnableInt(
        AT91PS_PITC pPITC)
{
	pPITC->PITC_PIMR |= AT91C_PITC_PITIEN;	 
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITDisableInt
//* \brief Disable PIT periodic interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_PITDisableInt(
        AT91PS_PITC pPITC)
{
	pPITC->PITC_PIMR &= ~AT91C_PITC_PITIEN;	 
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITGetMode
//* \brief Read PIT mode register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PITGetMode(
        AT91PS_PITC pPITC)
{
	return(pPITC->PITC_PIMR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITGetStatus
//* \brief Read PIT status register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PITGetStatus(
        AT91PS_PITC pPITC)
{
	return(pPITC->PITC_PISR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITGetPIIR
//* \brief Read PIT CPIV and PICNT without ressetting the counters
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PITGetPIIR(
        AT91PS_PITC pPITC)
{
	return(pPITC->PITC_PIIR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITGetPIVR
//* \brief Read System timer CPIV and PICNT without ressetting the counters
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PITGetPIVR(
        AT91PS_PITC pPITC)
{
	return(pPITC->PITC_PIVR);
}
/* *****************************************************************************
                SOFTWARE API FOR WDTC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTSetMode
//* \brief Set Watchdog Mode Register
//*----------------------------------------------------------------------------
__inline void AT91F_WDTSetMode(
        AT91PS_WDTC pWDTC,
        unsigned int Mode)
{
	pWDTC->WDTC_WDMR = Mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTRestart
//* \brief Restart Watchdog
//*----------------------------------------------------------------------------
__inline void AT91F_WDTRestart(
        AT91PS_WDTC pWDTC)
{
	pWDTC->WDTC_WDCR = 0xA5000001;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTSGettatus
//* \brief Get Watchdog Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_WDTSGettatus(
        AT91PS_WDTC pWDTC)
{
	return(pWDTC->WDTC_WDSR & 0x3);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTGetPeriod
//* \brief Translate ms into Watchdog Compatible value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_WDTGetPeriod(unsigned int ms)
{
	if ((ms < 4) || (ms > 16000))
		return 0;
	return((ms << 8) / 1000);
}
/* *****************************************************************************
                SOFTWARE API FOR VREG
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_VREG_Enable_LowPowerMode
//* \brief Enable VREG Low Power Mode
//*----------------------------------------------------------------------------
__inline void AT91F_VREG_Enable_LowPowerMode(
        AT91PS_VREG pVREG)
{
	pVREG->VREG_MR |= AT91C_VREG_PSTDBY;	 
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_VREG_Disable_LowPowerMode
//* \brief Disable VREG Low Power Mode
//*----------------------------------------------------------------------------
__inline void AT91F_VREG_Disable_LowPowerMode(
        AT91PS_VREG pVREG)
{
	pVREG->VREG_MR &= ~AT91C_VREG_PSTDBY;	 
}/* *****************************************************************************
                SOFTWARE API FOR MC
   ***************************************************************************** */

#define AT91C_MC_CORRECT_KEY  ((unsigned int) 0x5A << 24) // (MC) Correct Protect Key

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_Remap
//* \brief Make Remap
//*----------------------------------------------------------------------------
__inline void AT91F_MC_Remap (void)     //  
{
    AT91PS_MC pMC = (AT91PS_MC) AT91C_BASE_MC;
    
    pMC->MC_RCR = AT91C_MC_RCB;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_EFC_CfgModeReg
//* \brief Configure the EFC Mode Register of the MC controller
//*----------------------------------------------------------------------------
__inline void AT91F_MC_EFC_CfgModeReg (
	AT91PS_MC pMC, // pointer to a MC controller
	unsigned int mode)        // mode register 
{
	// Write to the FMR register
	pMC->MC_FMR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_EFC_GetModeReg
//* \brief Return MC EFC Mode Regsiter
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_MC_EFC_GetModeReg(
	AT91PS_MC pMC) // pointer to a MC controller
{
	return pMC->MC_FMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_EFC_ComputeFMCN
//* \brief Return MC EFC Mode Regsiter
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_MC_EFC_ComputeFMCN(
	int master_clock) // master clock in Hz
{
	return (master_clock/1000000 +2);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_EFC_PerformCmd
//* \brief Perform EFC Command
//*----------------------------------------------------------------------------
__inline void AT91F_MC_EFC_PerformCmd (
	AT91PS_MC pMC, // pointer to a MC controller
    unsigned int transfer_cmd)
{
	pMC->MC_FCR = transfer_cmd;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_EFC_GetStatus
//* \brief Return MC EFC Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_MC_EFC_GetStatus(
	AT91PS_MC pMC) // pointer to a MC controller
{
	return pMC->MC_FSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_EFC_IsInterruptMasked
//* \brief Test if EFC MC Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_MC_EFC_IsInterruptMasked(
        AT91PS_MC pMC,   // \arg  pointer to a MC controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_MC_EFC_GetModeReg(pMC) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_EFC_IsInterruptSet
//* \brief Test if EFC MC Interrupt is Set
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_MC_EFC_IsInterruptSet(
        AT91PS_MC pMC,   // \arg  pointer to a MC controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_MC_EFC_GetStatus(pMC) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR SPI
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgCs
//* \brief Configure SPI chip select register
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_CfgCs (
	AT91PS_SPI pSPI,     // pointer to a SPI controller
	int cs,     // SPI cs number (0 to 3)
 	int val)   //  chip select register
{
	//* Write to the CSR register
	*(pSPI->SPI_CSR + cs) = val;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_EnableIt
//* \brief Enable SPI interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_EnableIt (
	AT91PS_SPI pSPI,     // pointer to a SPI controller
	unsigned int flag)   // IT to be enabled
{
	//* Write to the IER register
	pSPI->SPI_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_DisableIt
//* \brief Disable SPI interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_DisableIt (
	AT91PS_SPI pSPI, // pointer to a SPI controller
	unsigned int flag) // IT to be disabled
{
	//* Write to the IDR register
	pSPI->SPI_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_Reset
//* \brief Reset the SPI controller
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_Reset (
	AT91PS_SPI pSPI // pointer to a SPI controller
	)
{
	//* Write to the CR register
	pSPI->SPI_CR = AT91C_SPI_SWRST;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_Enable
//* \brief Enable the SPI controller
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_Enable (
	AT91PS_SPI pSPI // pointer to a SPI controller
	)
{
	//* Write to the CR register
	pSPI->SPI_CR = AT91C_SPI_SPIEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_Disable
//* \brief Disable the SPI controller
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_Disable (
	AT91PS_SPI pSPI // pointer to a SPI controller
	)
{
	//* Write to the CR register
	pSPI->SPI_CR = AT91C_SPI_SPIDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgMode
//* \brief Enable the SPI controller
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_CfgMode (
	AT91PS_SPI pSPI, // pointer to a SPI controller
	int mode)        // mode register 
{
	//* Write to the MR register
	pSPI->SPI_MR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgPCS
//* \brief Switch to the correct PCS of SPI Mode Register : Fixed Peripheral Selected
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_CfgPCS (
	AT91PS_SPI pSPI, // pointer to a SPI controller
	char PCS_Device) // PCS of the Device
{	
 	//* Write to the MR register
	pSPI->SPI_MR &= 0xFFF0FFFF;
	pSPI->SPI_MR |= ( (PCS_Device<<16) & AT91C_SPI_PCS );
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_ReceiveFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initializaed with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_SPI_ReceiveFrame (
	AT91PS_SPI pSPI,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_ReceiveFrame(
		(AT91PS_PDC) &(pSPI->SPI_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_SendFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initializaed with Next Buffer, 0 if PDC is bSPIy
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_SPI_SendFrame(
	AT91PS_SPI pSPI,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_SendFrame(
		(AT91PS_PDC) &(pSPI->SPI_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_Close
//* \brief Close SPI: disable IT disable transfert, close PDC
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_Close (
	AT91PS_SPI pSPI)     // \arg pointer to a SPI controller
{
    //* Reset all the Chip Select register
    pSPI->SPI_CSR[0] = 0 ;
    pSPI->SPI_CSR[1] = 0 ;
    pSPI->SPI_CSR[2] = 0 ;
    pSPI->SPI_CSR[3] = 0 ;

    //* Reset the SPI mode
    pSPI->SPI_MR = 0  ;

    //* Disable all interrupts
    pSPI->SPI_IDR = 0xFFFFFFFF ;

    //* Abort the Peripheral Data Transfers
    AT91F_PDC_Close((AT91PS_PDC) &(pSPI->SPI_RPR));

    //* Disable receiver and transmitter and stop any activity immediately
    pSPI->SPI_CR = AT91C_SPI_SPIDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_PutChar
//* \brief Send a character,does not check if ready to send
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_PutChar (
	AT91PS_SPI pSPI,
	unsigned int character,
             unsigned int cs_number )
{
    unsigned int value_for_cs;
    value_for_cs = (~(1 << cs_number)) & 0xF;  //Place a zero among a 4 ONEs number
    pSPI->SPI_TDR = (character & 0xFFFF) | (value_for_cs << 16);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_GetChar
//* \brief Receive a character,does not check if a character is available
//*----------------------------------------------------------------------------
__inline int AT91F_SPI_GetChar (
	const AT91PS_SPI pSPI)
{
    return((pSPI->SPI_RDR) & 0xFFFF);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_GetInterruptMaskStatus
//* \brief Return SPI Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_SPI_GetInterruptMaskStatus( // \return SPI Interrupt Mask Status
        AT91PS_SPI pSpi) // \arg  pointer to a SPI controller
{
        return pSpi->SPI_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_IsInterruptMasked
//* \brief Test if SPI Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline int AT91F_SPI_IsInterruptMasked(
        AT91PS_SPI pSpi,   // \arg  pointer to a SPI controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_SPI_GetInterruptMaskStatus(pSpi) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR USART
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Baudrate
//* \brief Calculate the baudrate
//* Standard Asynchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_ASYNC_MODE ( AT91C_US_USMODE_NORMAL + \
                        AT91C_US_NBSTOP_1_BIT + \
                        AT91C_US_PAR_NONE + \
                        AT91C_US_CHRL_8_BITS + \
                        AT91C_US_CLKS_CLOCK )

//* Standard External Asynchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_ASYNC_SCK_MODE ( AT91C_US_USMODE_NORMAL + \
                            AT91C_US_NBSTOP_1_BIT + \
                            AT91C_US_PAR_NONE + \
                            AT91C_US_CHRL_8_BITS + \
                            AT91C_US_CLKS_EXT )

//* Standard Synchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_SYNC_MODE ( AT91C_US_SYNC + \
                       AT91C_US_USMODE_NORMAL + \
                       AT91C_US_NBSTOP_1_BIT + \
                       AT91C_US_PAR_NONE + \
                       AT91C_US_CHRL_8_BITS + \
                       AT91C_US_CLKS_CLOCK )

//* SCK used Label
#define AT91C_US_SCK_USED (AT91C_US_CKLO | AT91C_US_CLKS_EXT)

//* Standard ISO T=0 Mode : 8 bits , 1 stop , parity
#define AT91C_US_ISO_READER_MODE ( AT91C_US_USMODE_ISO7816_0 + \
					   		 AT91C_US_CLKS_CLOCK +\
                       		 AT91C_US_NBSTOP_1_BIT + \
                       		 AT91C_US_PAR_EVEN + \
                       		 AT91C_US_CHRL_8_BITS + \
                       		 AT91C_US_CKLO +\
                       		 AT91C_US_OVER)

//* Standard IRDA mode
#define AT91C_US_ASYNC_IRDA_MODE (  AT91C_US_USMODE_IRDA + \
                            AT91C_US_NBSTOP_1_BIT + \
                            AT91C_US_PAR_NONE + \
                            AT91C_US_CHRL_8_BITS + \
                            AT91C_US_CLKS_CLOCK )

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Baudrate
//* \brief Caluculate baud_value according to the main clock and the baud rate
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_US_Baudrate (
	const unsigned int main_clock, // \arg peripheral clock
	const unsigned int baud_rate)  // \arg UART baudrate
{
	unsigned int baud_value = ((main_clock*10)/(baud_rate * 16));
	if ((baud_value % 10) >= 5)
		baud_value = (baud_value / 10) + 1;
	else
		baud_value /= 10;
	return baud_value;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_SetBaudrate
//* \brief Set the baudrate according to the CPU clock
//*----------------------------------------------------------------------------
__inline void AT91F_US_SetBaudrate (
	AT91PS_USART pUSART,    // \arg pointer to a USART controller
	unsigned int mainClock, // \arg peripheral clock
	unsigned int speed)     // \arg UART baudrate
{
	//* Define the baud rate divisor register
	pUSART->US_BRGR = AT91F_US_Baudrate(mainClock, speed);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_SetTimeguard
//* \brief Set USART timeguard
//*----------------------------------------------------------------------------
__inline void AT91F_US_SetTimeguard (
	AT91PS_USART pUSART,    // \arg pointer to a USART controller
	unsigned int timeguard) // \arg timeguard value
{
	//* Write the Timeguard Register
	pUSART->US_TTGR = timeguard ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_EnableIt
//* \brief Enable USART IT
//*----------------------------------------------------------------------------
__inline void AT91F_US_EnableIt (
	AT91PS_USART pUSART, // \arg pointer to a USART controller
	unsigned int flag)   // \arg IT to be enabled
{
	//* Write to the IER register
	pUSART->US_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_DisableIt
//* \brief Disable USART IT
//*----------------------------------------------------------------------------
__inline void AT91F_US_DisableIt (
	AT91PS_USART pUSART, // \arg pointer to a USART controller
	unsigned int flag)   // \arg IT to be disabled
{
	//* Write to the IER register
	pUSART->US_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Configure
//* \brief Configure USART
//*----------------------------------------------------------------------------
__inline void AT91F_US_Configure (
	AT91PS_USART pUSART,     // \arg pointer to a USART controller
	unsigned int mainClock,  // \arg peripheral clock
	unsigned int mode ,      // \arg mode Register to be programmed
	unsigned int baudRate ,  // \arg baudrate to be programmed
	unsigned int timeguard ) // \arg timeguard to be programmed
{
    //* Disable interrupts
    pUSART->US_IDR = (unsigned int) -1;

    //* Reset receiver and transmitter
    pUSART->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS ;

	//* Define the baud rate divisor register
	AT91F_US_SetBaudrate(pUSART, mainClock, baudRate);

	//* Write the Timeguard Register
	AT91F_US_SetTimeguard(pUSART, timeguard);

    //* Clear Transmit and Receive Counters
    AT91F_PDC_Open((AT91PS_PDC) &(pUSART->US_RPR));

    //* Define the USART mode
    pUSART->US_MR = mode  ;

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_EnableRx
//* \brief Enable receiving characters
//*----------------------------------------------------------------------------
__inline void AT91F_US_EnableRx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Enable receiver
    pUSART->US_CR = AT91C_US_RXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_EnableTx
//* \brief Enable sending characters
//*----------------------------------------------------------------------------
__inline void AT91F_US_EnableTx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Enable  transmitter
    pUSART->US_CR = AT91C_US_TXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_ResetRx
//* \brief Reset Receiver and re-enable it
//*----------------------------------------------------------------------------
__inline void AT91F_US_ResetRx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
	//* Reset receiver
	pUSART->US_CR = AT91C_US_RSTRX;
    //* Re-Enable receiver
    pUSART->US_CR = AT91C_US_RXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_ResetTx
//* \brief Reset Transmitter and re-enable it
//*----------------------------------------------------------------------------
__inline void AT91F_US_ResetTx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
	//* Reset transmitter
	pUSART->US_CR = AT91C_US_RSTTX;
    //* Enable transmitter
    pUSART->US_CR = AT91C_US_TXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_DisableRx
//* \brief Disable Receiver
//*----------------------------------------------------------------------------
__inline void AT91F_US_DisableRx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Disable receiver
    pUSART->US_CR = AT91C_US_RXDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_DisableTx
//* \brief Disable Transmitter
//*----------------------------------------------------------------------------
__inline void AT91F_US_DisableTx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Disable transmitter
    pUSART->US_CR = AT91C_US_TXDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Close
//* \brief Close USART: disable IT disable receiver and transmitter, close PDC
//*----------------------------------------------------------------------------
__inline void AT91F_US_Close (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Reset the baud rate divisor register
    pUSART->US_BRGR = 0 ;

    //* Reset the USART mode
    pUSART->US_MR = 0  ;

    //* Reset the Timeguard Register
    pUSART->US_TTGR = 0;

    //* Disable all interrupts
    pUSART->US_IDR = 0xFFFFFFFF ;

    //* Abort the Peripheral Data Transfers
    AT91F_PDC_Close((AT91PS_PDC) &(pUSART->US_RPR));

    //* Disable receiver and transmitter and stop any activity immediately
    pUSART->US_CR = AT91C_US_TXDIS | AT91C_US_RXDIS | AT91C_US_RSTTX | AT91C_US_RSTRX ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_TxReady
//* \brief Return 1 if a character can be written in US_THR
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_US_TxReady (
	AT91PS_USART pUSART )     // \arg pointer to a USART controller
{
    return (pUSART->US_CSR & AT91C_US_TXRDY);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_RxReady
//* \brief Return 1 if a character can be read in US_RHR
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_US_RxReady (
	AT91PS_USART pUSART )     // \arg pointer to a USART controller
{
    return (pUSART->US_CSR & AT91C_US_RXRDY);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Error
//* \brief Return the error flag
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_US_Error (
	AT91PS_USART pUSART )     // \arg pointer to a USART controller
{
    return (pUSART->US_CSR &
    	(AT91C_US_OVRE |  // Overrun error
    	 AT91C_US_FRAME | // Framing error
    	 AT91C_US_PARE));  // Parity error
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_PutChar
//* \brief Send a character,does not check if ready to send
//*----------------------------------------------------------------------------
__inline void AT91F_US_PutChar (
	AT91PS_USART pUSART,
	int character )
{
    pUSART->US_THR = (character & 0x1FF);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_GetChar
//* \brief Receive a character,does not check if a character is available
//*----------------------------------------------------------------------------
__inline int AT91F_US_GetChar (
	const AT91PS_USART pUSART)
{
    return((pUSART->US_RHR) & 0x1FF);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_SendFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initializaed with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_US_SendFrame(
	AT91PS_USART pUSART,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_SendFrame(
		(AT91PS_PDC) &(pUSART->US_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_ReceiveFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initializaed with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_US_ReceiveFrame (
	AT91PS_USART pUSART,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_ReceiveFrame(
		(AT91PS_PDC) &(pUSART->US_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_SetIrdaFilter
//* \brief Set the value of IrDa filter tregister
//*----------------------------------------------------------------------------
__inline void AT91F_US_SetIrdaFilter (
	AT91PS_USART pUSART,
	unsigned char value
)
{
	pUSART->US_IF = value;
}

/* *****************************************************************************
                SOFTWARE API FOR SSC
   ***************************************************************************** */
//* Define the standard I2S mode configuration

//* Configuration to set in the SSC Transmit Clock Mode Register
//* Parameters :  nb_bit_by_slot : 8, 16 or 32 bits
//* 			  nb_slot_by_frame : number of channels
#define AT91C_I2S_ASY_MASTER_TX_SETTING(nb_bit_by_slot, nb_slot_by_frame)( +\
									   AT91C_SSC_CKS_DIV   +\
                            		   AT91C_SSC_CKO_CONTINOUS      +\
                            		   AT91C_SSC_CKG_NONE    +\
                                       AT91C_SSC_START_FALL_RF +\
                           			   AT91C_SSC_STTOUT  +\
                            		   ((1<<16) & AT91C_SSC_STTDLY) +\
                            		   ((((nb_bit_by_slot*nb_slot_by_frame)/2)-1) <<24))


//* Configuration to set in the SSC Transmit Frame Mode Register
//* Parameters : nb_bit_by_slot : 8, 16 or 32 bits
//* 			 nb_slot_by_frame : number of channels
#define AT91C_I2S_ASY_TX_FRAME_SETTING(nb_bit_by_slot, nb_slot_by_frame)( +\
									(nb_bit_by_slot-1)  +\
                            		AT91C_SSC_MSBF   +\
                            		(((nb_slot_by_frame-1)<<8) & AT91C_SSC_DATNB)  +\
                            		(((nb_bit_by_slot-1)<<16) & AT91C_SSC_FSLEN) +\
                            		AT91C_SSC_FSOS_NEGATIVE)


//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_SetBaudrate
//* \brief Set the baudrate according to the CPU clock
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_SetBaudrate (
        AT91PS_SSC pSSC,        // \arg pointer to a SSC controller
        unsigned int mainClock, // \arg peripheral clock
        unsigned int speed)     // \arg SSC baudrate
{
        unsigned int baud_value;
        //* Define the baud rate divisor register
        if (speed == 0)
           baud_value = 0;
        else
        {
           baud_value = (unsigned int) (mainClock * 10)/(2*speed);
           if ((baud_value % 10) >= 5)
                  baud_value = (baud_value / 10) + 1;
           else
                  baud_value /= 10;
        }

        pSSC->SSC_CMR = baud_value;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_Configure
//* \brief Configure SSC
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_Configure (
             AT91PS_SSC pSSC,          // \arg pointer to a SSC controller
             unsigned int syst_clock,  // \arg System Clock Frequency
             unsigned int baud_rate,   // \arg Expected Baud Rate Frequency
             unsigned int clock_rx,    // \arg Receiver Clock Parameters
             unsigned int mode_rx,     // \arg mode Register to be programmed
             unsigned int clock_tx,    // \arg Transmitter Clock Parameters
             unsigned int mode_tx)     // \arg mode Register to be programmed
{
    //* Disable interrupts
	pSSC->SSC_IDR = (unsigned int) -1;

    //* Reset receiver and transmitter
	pSSC->SSC_CR = AT91C_SSC_SWRST | AT91C_SSC_RXDIS | AT91C_SSC_TXDIS ;

    //* Define the Clock Mode Register
	AT91F_SSC_SetBaudrate(pSSC, syst_clock, baud_rate);

     //* Write the Receive Clock Mode Register
	pSSC->SSC_RCMR =  clock_rx;

     //* Write the Transmit Clock Mode Register
	pSSC->SSC_TCMR =  clock_tx;

     //* Write the Receive Frame Mode Register
	pSSC->SSC_RFMR =  mode_rx;

     //* Write the Transmit Frame Mode Register
	pSSC->SSC_TFMR =  mode_tx;

    //* Clear Transmit and Receive Counters
	AT91F_PDC_Open((AT91PS_PDC) &(pSSC->SSC_RPR));


}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_EnableRx
//* \brief Enable receiving datas
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_EnableRx (
	AT91PS_SSC pSSC)     // \arg pointer to a SSC controller
{
    //* Enable receiver
    pSSC->SSC_CR = AT91C_SSC_RXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_DisableRx
//* \brief Disable receiving datas
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_DisableRx (
	AT91PS_SSC pSSC)     // \arg pointer to a SSC controller
{
    //* Disable receiver
    pSSC->SSC_CR = AT91C_SSC_RXDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_EnableTx
//* \brief Enable sending datas
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_EnableTx (
	AT91PS_SSC pSSC)     // \arg pointer to a SSC controller
{
    //* Enable  transmitter
    pSSC->SSC_CR = AT91C_SSC_TXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_DisableTx
//* \brief Disable sending datas
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_DisableTx (
	AT91PS_SSC pSSC)     // \arg pointer to a SSC controller
{
    //* Disable  transmitter
    pSSC->SSC_CR = AT91C_SSC_TXDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_EnableIt
//* \brief Enable SSC IT
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_EnableIt (
	AT91PS_SSC pSSC, // \arg pointer to a SSC controller
	unsigned int flag)   // \arg IT to be enabled
{
	//* Write to the IER register
	pSSC->SSC_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_DisableIt
//* \brief Disable SSC IT
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_DisableIt (
	AT91PS_SSC pSSC, // \arg pointer to a SSC controller
	unsigned int flag)   // \arg IT to be disabled
{
	//* Write to the IDR register
	pSSC->SSC_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_ReceiveFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initialized with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_SSC_ReceiveFrame (
	AT91PS_SSC pSSC,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_ReceiveFrame(
		(AT91PS_PDC) &(pSSC->SSC_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_SendFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initialized with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_SSC_SendFrame(
	AT91PS_SSC pSSC,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_SendFrame(
		(AT91PS_PDC) &(pSSC->SSC_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_GetInterruptMaskStatus
//* \brief Return SSC Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_SSC_GetInterruptMaskStatus( // \return SSC Interrupt Mask Status
        AT91PS_SSC pSsc) // \arg  pointer to a SSC controller
{
        return pSsc->SSC_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_IsInterruptMasked
//* \brief Test if SSC Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline int AT91F_SSC_IsInterruptMasked(
        AT91PS_SSC pSsc,   // \arg  pointer to a SSC controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_SSC_GetInterruptMaskStatus(pSsc) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR TWI
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_EnableIt
//* \brief Enable TWI IT
//*----------------------------------------------------------------------------
__inline void AT91F_TWI_EnableIt (
	AT91PS_TWI pTWI, // \arg pointer to a TWI controller
	unsigned int flag)   // \arg IT to be enabled
{
	//* Write to the IER register
	pTWI->TWI_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_DisableIt
//* \brief Disable TWI IT
//*----------------------------------------------------------------------------
__inline void AT91F_TWI_DisableIt (
	AT91PS_TWI pTWI, // \arg pointer to a TWI controller
	unsigned int flag)   // \arg IT to be disabled
{
	//* Write to the IDR register
	pTWI->TWI_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_Configure
//* \brief Configure TWI in master mode
//*----------------------------------------------------------------------------
__inline void AT91F_TWI_Configure ( AT91PS_TWI pTWI )          // \arg pointer to a TWI controller
{
    //* Disable interrupts
	pTWI->TWI_IDR = (unsigned int) -1;

    //* Reset peripheral
	pTWI->TWI_CR = AT91C_TWI_SWRST;

	//* Set Master mode
	pTWI->TWI_CR = AT91C_TWI_MSEN;

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_GetInterruptMaskStatus
//* \brief Return TWI Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_TWI_GetInterruptMaskStatus( // \return TWI Interrupt Mask Status
        AT91PS_TWI pTwi) // \arg  pointer to a TWI controller
{
        return pTwi->TWI_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_IsInterruptMasked
//* \brief Test if TWI Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline int AT91F_TWI_IsInterruptMasked(
        AT91PS_TWI pTwi,   // \arg  pointer to a TWI controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_TWI_GetInterruptMaskStatus(pTwi) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR PWMC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_GetStatus
//* \brief Return PWM Interrupt Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PWMC_GetStatus( // \return PWM Interrupt Status
	AT91PS_PWMC pPWM) // pointer to a PWM controller
{
	return pPWM->PWMC_SR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_InterruptEnable
//* \brief Enable PWM Interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_InterruptEnable(
        AT91PS_PWMC pPwm,   // \arg  pointer to a PWM controller
        unsigned int flag) // \arg  PWM interrupt to be enabled
{
        pPwm->PWMC_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_InterruptDisable
//* \brief Disable PWM Interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_InterruptDisable(
        AT91PS_PWMC pPwm,   // \arg  pointer to a PWM controller
        unsigned int flag) // \arg  PWM interrupt to be disabled
{
        pPwm->PWMC_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_GetInterruptMaskStatus
//* \brief Return PWM Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PWMC_GetInterruptMaskStatus( // \return PWM Interrupt Mask Status
        AT91PS_PWMC pPwm) // \arg  pointer to a PWM controller
{
        return pPwm->PWMC_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_IsInterruptMasked
//* \brief Test if PWM Interrupt is Masked
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PWMC_IsInterruptMasked(
        AT91PS_PWMC pPWM,   // \arg  pointer to a PWM controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_PWMC_GetInterruptMaskStatus(pPWM) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_IsStatusSet
//* \brief Test if PWM Interrupt is Set
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_PWMC_IsStatusSet(
        AT91PS_PWMC pPWM,   // \arg  pointer to a PWM controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_PWMC_GetStatus(pPWM) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_CfgChannel
//* \brief Test if PWM Interrupt is Set
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_CfgChannel(
        AT91PS_PWMC pPWM,   // \arg  pointer to a PWM controller
        unsigned int channelId, // \arg PWM channel ID
        unsigned int mode, // \arg  PWM mode
        unsigned int period, // \arg PWM period
        unsigned int duty) // \arg PWM duty cycle
{
	pPWM->PWMC_CH[channelId].PWMC_CMR = mode;
	pPWM->PWMC_CH[channelId].PWMC_CDTYR = duty;
	pPWM->PWMC_CH[channelId].PWMC_CPRDR = period;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_StartChannel
//* \brief Enable channel
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_StartChannel(
        AT91PS_PWMC pPWM,   // \arg  pointer to a PWM controller
        unsigned int flag) // \arg  Channels IDs to be enabled
{
	pPWM->PWMC_ENA = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_StopChannel
//* \brief Disable channel
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_StopChannel(
        AT91PS_PWMC pPWM,   // \arg  pointer to a PWM controller
        unsigned int flag) // \arg  Channels IDs to be enabled
{
	pPWM->PWMC_DIS = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWM_UpdateChannel
//* \brief Update Period or Duty Cycle
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_UpdateChannel(
        AT91PS_PWMC pPWM,   // \arg  pointer to a PWM controller
        unsigned int channelId, // \arg PWM channel ID
        unsigned int update) // \arg  Channels IDs to be enabled
{
	pPWM->PWMC_CH[channelId].PWMC_CUPDR = update;
}

/* *****************************************************************************
                SOFTWARE API FOR UDP
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EnableIt
//* \brief Enable UDP IT
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_EnableIt (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned int flag)   // \arg IT to be enabled
{
	//* Write to the IER register
	pUDP->UDP_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_DisableIt
//* \brief Disable UDP IT
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_DisableIt (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned int flag)   // \arg IT to be disabled
{
	//* Write to the IDR register
	pUDP->UDP_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_SetAddress
//* \brief Set UDP functional address
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_SetAddress (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned char address)   // \arg new UDP address
{
	pUDP->UDP_FADDR = (AT91C_UDP_FEN | address);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EnableEp
//* \brief Enable Endpoint
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_EnableEp (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned char endpoint)   // \arg endpoint number
{
	pUDP->UDP_CSR[endpoint] |= AT91C_UDP_EPEDS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_DisableEp
//* \brief Enable Endpoint
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_DisableEp (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned char endpoint)   // \arg endpoint number
{
	pUDP->UDP_CSR[endpoint] &= ~AT91C_UDP_EPEDS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_SetState
//* \brief Set UDP Device state
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_SetState (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned int flag)   // \arg new UDP address
{
	pUDP->UDP_GLBSTATE  &= ~(AT91C_UDP_FADDEN | AT91C_UDP_CONFG);
	pUDP->UDP_GLBSTATE  |= flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_GetState
//* \brief return UDP Device state
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_UDP_GetState ( // \return the UDP device state
	AT91PS_UDP pUDP)     // \arg pointer to a UDP controller
{
	return (pUDP->UDP_GLBSTATE  & (AT91C_UDP_FADDEN | AT91C_UDP_CONFG));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_ResetEp
//* \brief Reset UDP endpoint
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_ResetEp ( // \return the UDP device state
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned int flag)   // \arg Endpoints to be reset
{
	pUDP->UDP_RSTEP = flag;
	pUDP->UDP_RSTEP = 0;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpStall
//* \brief Endpoint will STALL requests
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_EpStall(
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned char endpoint)   // \arg endpoint number
{
	pUDP->UDP_CSR[endpoint] |= AT91C_UDP_FORCESTALL;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpWrite
//* \brief Write value in the DPR
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_EpWrite(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint,  // \arg endpoint number
	unsigned char value)     // \arg value to be written in the DPR
{
	pUDP->UDP_FDR[endpoint] = value;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpRead
//* \brief Return value from the DPR
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_UDP_EpRead(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint)  // \arg endpoint number
{
	return pUDP->UDP_FDR[endpoint];
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpEndOfWr
//* \brief Notify the UDP that values in DPR are ready to be sent
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_EpEndOfWr(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint)  // \arg endpoint number
{
	pUDP->UDP_CSR[endpoint] |= AT91C_UDP_TXPKTRDY;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpClear
//* \brief Clear flag in the endpoint CSR register
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_EpClear(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint,  // \arg endpoint number
	unsigned int flag)       // \arg flag to be cleared
{
	pUDP->UDP_CSR[endpoint] &= ~(flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpSet
//* \brief Set flag in the endpoint CSR register
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_EpSet(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint,  // \arg endpoint number
	unsigned int flag)       // \arg flag to be cleared
{
	pUDP->UDP_CSR[endpoint] |= flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpStatus
//* \brief Return the endpoint CSR register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_UDP_EpStatus(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint)  // \arg endpoint number
{
	return pUDP->UDP_CSR[endpoint];
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_GetInterruptMaskStatus
//* \brief Return UDP Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_UDP_GetInterruptMaskStatus(
  AT91PS_UDP pUdp)        // \arg  pointer to a UDP controller
{
  return pUdp->UDP_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_IsInterruptMasked
//* \brief Test if UDP Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline int AT91F_UDP_IsInterruptMasked(
  AT91PS_UDP pUdp,       // \arg  pointer to a UDP controller
  unsigned int flag)     // \arg  flag to be tested
{
  return (AT91F_UDP_GetInterruptMaskStatus(pUdp) & flag);
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_UDP_InterruptStatusRegister
//  \brief Return the Interrupt Status Register
// ----------------------------------------------------------------------------
__inline unsigned int AT91F_UDP_InterruptStatusRegister( 
  AT91PS_UDP pUDP )      // \arg  pointer to a UDP controller
{
  return pUDP->UDP_ISR;
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_UDP_InterruptClearRegister
//  \brief Clear Interrupt Register
// ----------------------------------------------------------------------------
__inline void AT91F_UDP_InterruptClearRegister (
  AT91PS_UDP pUDP,       // \arg pointer to UDP controller
  unsigned int flag)     // \arg IT to be cleat
{
  pUDP->UDP_ICR = flag; 
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_UDP_EnableTransceiver
//  \brief Enable transceiver
// ----------------------------------------------------------------------------
__inline void AT91F_UDP_EnableTransceiver( 
  AT91PS_UDP pUDP )      // \arg  pointer to a UDP controller
{
    pUDP->UDP_TXVC &= ~AT91C_UDP_TXVDIS; 
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_UDP_DisableTransceiver
//  \brief Disable transceiver
// ----------------------------------------------------------------------------
__inline void AT91F_UDP_DisableTransceiver( 
  AT91PS_UDP pUDP )      // \arg  pointer to a UDP controller
{
    pUDP->UDP_TXVC = AT91C_UDP_TXVDIS; 
}

/* *****************************************************************************
                SOFTWARE API FOR TC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_TC_InterruptEnable
//* \brief Enable TC Interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_TC_InterruptEnable(
        AT91PS_TC pTc,   // \arg  pointer to a TC controller
        unsigned int flag) // \arg  TC interrupt to be enabled
{
        pTc->TC_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC_InterruptDisable
//* \brief Disable TC Interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_TC_InterruptDisable(
        AT91PS_TC pTc,   // \arg  pointer to a TC controller
        unsigned int flag) // \arg  TC interrupt to be disabled
{
        pTc->TC_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC_GetInterruptMaskStatus
//* \brief Return TC Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_TC_GetInterruptMaskStatus( // \return TC Interrupt Mask Status
        AT91PS_TC pTc) // \arg  pointer to a TC controller
{
        return pTc->TC_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC_IsInterruptMasked
//* \brief Test if TC Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline int AT91F_TC_IsInterruptMasked(
        AT91PS_TC pTc,   // \arg  pointer to a TC controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_TC_GetInterruptMaskStatus(pTc) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR CAN
   ***************************************************************************** */
#define	STANDARD_FORMAT 0
#define	EXTENDED_FORMAT 1

//*----------------------------------------------------------------------------
//* \fn    AT91F_InitMailboxRegisters()
//* \brief Configure the corresponding mailbox
//*----------------------------------------------------------------------------
__inline void AT91F_InitMailboxRegisters(AT91PS_CAN_MB	CAN_Mailbox,
								int  			mode_reg,
								int 			acceptance_mask_reg,
								int  			id_reg,
								int  			data_low_reg,
								int  			data_high_reg,
								int  			control_reg)
{
	CAN_Mailbox->CAN_MB_MCR 	= 0x0;
	CAN_Mailbox->CAN_MB_MMR 	= mode_reg;
	CAN_Mailbox->CAN_MB_MAM 	= acceptance_mask_reg;
	CAN_Mailbox->CAN_MB_MID 	= id_reg;
	CAN_Mailbox->CAN_MB_MDL 	= data_low_reg; 		
	CAN_Mailbox->CAN_MB_MDH 	= data_high_reg;
	CAN_Mailbox->CAN_MB_MCR 	= control_reg;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_EnableCAN()
//* \brief 
//*----------------------------------------------------------------------------
__inline void AT91F_EnableCAN(
	AT91PS_CAN pCAN)     // pointer to a CAN controller
{
	pCAN->CAN_MR |= AT91C_CAN_CANEN;

	// Wait for WAKEUP flag raising <=> 11-recessive-bit were scanned by the transceiver
	while( (pCAN->CAN_SR & AT91C_CAN_WAKEUP) != AT91C_CAN_WAKEUP );
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DisableCAN()
//* \brief 
//*----------------------------------------------------------------------------
__inline void AT91F_DisableCAN(
	AT91PS_CAN pCAN)     // pointer to a CAN controller
{
	pCAN->CAN_MR &= ~AT91C_CAN_CANEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_EnableIt
//* \brief Enable CAN interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_EnableIt (
	AT91PS_CAN pCAN,     // pointer to a CAN controller
	unsigned int flag)   // IT to be enabled
{
	//* Write to the IER register
	pCAN->CAN_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_DisableIt
//* \brief Disable CAN interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_DisableIt (
	AT91PS_CAN pCAN, // pointer to a CAN controller
	unsigned int flag) // IT to be disabled
{
	//* Write to the IDR register
	pCAN->CAN_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetStatus
//* \brief Return CAN Interrupt Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetStatus( // \return CAN Interrupt Status
	AT91PS_CAN pCAN) // pointer to a CAN controller
{
	return pCAN->CAN_SR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetInterruptMaskStatus
//* \brief Return CAN Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetInterruptMaskStatus( // \return CAN Interrupt Mask Status
	AT91PS_CAN pCAN) // pointer to a CAN controller
{
	return pCAN->CAN_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_IsInterruptMasked
//* \brief Test if CAN Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_IsInterruptMasked(
        AT91PS_CAN pCAN,   // \arg  pointer to a CAN controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_CAN_GetInterruptMaskStatus(pCAN) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_IsStatusSet
//* \brief Test if CAN Interrupt is Set
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_IsStatusSet(
        AT91PS_CAN pCAN,   // \arg  pointer to a CAN controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_CAN_GetStatus(pCAN) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgModeReg
//* \brief Configure the Mode Register of the CAN controller
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgModeReg (
	AT91PS_CAN pCAN, // pointer to a CAN controller
	unsigned int mode)        // mode register 
{
	//* Write to the MR register
	pCAN->CAN_MR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetModeReg
//* \brief Return the Mode Register of the CAN controller value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetModeReg (
	AT91PS_CAN pCAN // pointer to a CAN controller
	)
{
	return pCAN->CAN_MR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgBaudrateReg
//* \brief Configure the Baudrate of the CAN controller for the network
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgBaudrateReg (
	AT91PS_CAN pCAN, // pointer to a CAN controller
	unsigned int baudrate_cfg)
{
	//* Write to the BR register
	pCAN->CAN_BR = baudrate_cfg;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetBaudrate
//* \brief Return the Baudrate of the CAN controller for the network value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetBaudrate (
	AT91PS_CAN pCAN // pointer to a CAN controller
	)
{
	return pCAN->CAN_BR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetInternalCounter
//* \brief Return CAN Timer Regsiter Value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetInternalCounter (
	AT91PS_CAN pCAN // pointer to a CAN controller
	)
{
	return pCAN->CAN_TIM;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetTimestamp
//* \brief Return CAN Timestamp Register Value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetTimestamp (
	AT91PS_CAN pCAN // pointer to a CAN controller
	)
{
	return pCAN->CAN_TIMESTP;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetErrorCounter
//* \brief Return CAN Error Counter Register Value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetErrorCounter (
	AT91PS_CAN pCAN // pointer to a CAN controller
	)
{
	return pCAN->CAN_ECR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_InitTransferRequest
//* \brief Request for a transfer on the corresponding mailboxes
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_InitTransferRequest (
	AT91PS_CAN pCAN, // pointer to a CAN controller
    unsigned int transfer_cmd)
{
	pCAN->CAN_TCR = transfer_cmd;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_InitAbortRequest
//* \brief Abort the corresponding mailboxes
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_InitAbortRequest (
	AT91PS_CAN pCAN, // pointer to a CAN controller
    unsigned int abort_cmd)
{
	pCAN->CAN_ACR = abort_cmd;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgMessageModeReg
//* \brief Program the Message Mode Register
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgMessageModeReg (
	AT91PS_CAN_MB	CAN_Mailbox, // pointer to a CAN Mailbox
    unsigned int mode)
{
	CAN_Mailbox->CAN_MB_MMR = mode;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetMessageModeReg
//* \brief Return the Message Mode Register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetMessageModeReg (
	AT91PS_CAN_MB	CAN_Mailbox) // pointer to a CAN Mailbox
{
	return CAN_Mailbox->CAN_MB_MMR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgMessageIDReg
//* \brief Program the Message ID Register
//* \brief Version == 0 for Standard messsage, Version == 1 for Extended  
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgMessageIDReg (
	AT91PS_CAN_MB	CAN_Mailbox, // pointer to a CAN Mailbox
    unsigned int id,
    unsigned char version)
{
	if(version==0)	// IDvA Standard Format
		CAN_Mailbox->CAN_MB_MID = id<<18;
	else	// IDvB Extended Format
		CAN_Mailbox->CAN_MB_MID = id | (1<<29);	// set MIDE bit
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetMessageIDReg
//* \brief Return the Message ID Register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetMessageIDReg (
	AT91PS_CAN_MB	CAN_Mailbox) // pointer to a CAN Mailbox
{
	return CAN_Mailbox->CAN_MB_MID;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgMessageAcceptanceMaskReg
//* \brief Program the Message Acceptance Mask Register
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgMessageAcceptanceMaskReg (
	AT91PS_CAN_MB	CAN_Mailbox, // pointer to a CAN Mailbox
    unsigned int mask)
{
	CAN_Mailbox->CAN_MB_MAM = mask;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetMessageAcceptanceMaskReg
//* \brief Return the Message Acceptance Mask Register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetMessageAcceptanceMaskReg (
	AT91PS_CAN_MB	CAN_Mailbox) // pointer to a CAN Mailbox
{
	return CAN_Mailbox->CAN_MB_MAM;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetFamilyID
//* \brief Return the Message ID Register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetFamilyID (
	AT91PS_CAN_MB	CAN_Mailbox) // pointer to a CAN Mailbox
{
	return CAN_Mailbox->CAN_MB_MFID;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgMessageCtrl
//* \brief Request and config for a transfer on the corresponding mailbox
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgMessageCtrlReg (
	AT91PS_CAN_MB	CAN_Mailbox, // pointer to a CAN Mailbox
    unsigned int message_ctrl_cmd)
{
	CAN_Mailbox->CAN_MB_MCR = message_ctrl_cmd;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetMessageStatus
//* \brief Return CAN Mailbox Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetMessageStatus (
	AT91PS_CAN_MB	CAN_Mailbox) // pointer to a CAN Mailbox
{
	return CAN_Mailbox->CAN_MB_MSR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgMessageDataLow
//* \brief Program data low value
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgMessageDataLow (
	AT91PS_CAN_MB	CAN_Mailbox, // pointer to a CAN Mailbox
    unsigned int data)
{
	CAN_Mailbox->CAN_MB_MDL = data;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetMessageDataLow
//* \brief Return data low value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetMessageDataLow (
	AT91PS_CAN_MB	CAN_Mailbox) // pointer to a CAN Mailbox
{
	return CAN_Mailbox->CAN_MB_MDL;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgMessageDataHigh
//* \brief Program data high value
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgMessageDataHigh (
	AT91PS_CAN_MB	CAN_Mailbox, // pointer to a CAN Mailbox
    unsigned int data)
{
	CAN_Mailbox->CAN_MB_MDH = data;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_GetMessageDataHigh
//* \brief Return data high value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_CAN_GetMessageDataHigh (
	AT91PS_CAN_MB	CAN_Mailbox) // pointer to a CAN Mailbox
{
	return CAN_Mailbox->CAN_MB_MDH;	
}

/* *****************************************************************************
                SOFTWARE API FOR ADC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_EnableIt
//* \brief Enable ADC interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_EnableIt (
	AT91PS_ADC pADC,     // pointer to a ADC controller
	unsigned int flag)   // IT to be enabled
{
	//* Write to the IER register
	pADC->ADC_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_DisableIt
//* \brief Disable ADC interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_DisableIt (
	AT91PS_ADC pADC, // pointer to a ADC controller
	unsigned int flag) // IT to be disabled
{
	//* Write to the IDR register
	pADC->ADC_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetStatus
//* \brief Return ADC Interrupt Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetStatus( // \return ADC Interrupt Status
	AT91PS_ADC pADC) // pointer to a ADC controller
{
	return pADC->ADC_SR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetInterruptMaskStatus
//* \brief Return ADC Interrupt Mask Status
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetInterruptMaskStatus( // \return ADC Interrupt Mask Status
	AT91PS_ADC pADC) // pointer to a ADC controller
{
	return pADC->ADC_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_IsInterruptMasked
//* \brief Test if ADC Interrupt is Masked 
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_IsInterruptMasked(
        AT91PS_ADC pADC,   // \arg  pointer to a ADC controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_ADC_GetInterruptMaskStatus(pADC) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_IsStatusSet
//* \brief Test if ADC Status is Set
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_IsStatusSet(
        AT91PS_ADC pADC,   // \arg  pointer to a ADC controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_ADC_GetStatus(pADC) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_CfgModeReg
//* \brief Configure the Mode Register of the ADC controller
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_CfgModeReg (
	AT91PS_ADC pADC, // pointer to a ADC controller
	unsigned int mode)        // mode register 
{
	//* Write to the MR register
	pADC->ADC_MR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetModeReg
//* \brief Return the Mode Register of the ADC controller value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetModeReg (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_MR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_CfgTimings
//* \brief Configure the different necessary timings of the ADC controller
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_CfgTimings (
	AT91PS_ADC pADC, // pointer to a ADC controller
	unsigned int mck_clock, // in MHz 
	unsigned int adc_clock, // in MHz 
	unsigned int startup_time, // in us 
	unsigned int sample_and_hold_time)	// in ns  
{
	unsigned int prescal,startup,shtim;
	
	prescal = mck_clock/(2*adc_clock) - 1;
	startup = adc_clock*startup_time/8 - 1;
	shtim = adc_clock*sample_and_hold_time/1000 - 1;
	
	//* Write to the MR register
	pADC->ADC_MR = ( (prescal<<8) & AT91C_ADC_PRESCAL) | ( (startup<<16) & AT91C_ADC_STARTUP) | ( (shtim<<24) & AT91C_ADC_SHTIM);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_EnableChannel
//* \brief Return ADC Timer Register Value
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_EnableChannel (
	AT91PS_ADC pADC, // pointer to a ADC controller
	unsigned int channel)        // mode register 
{
	//* Write to the CHER register
	pADC->ADC_CHER = channel;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_DisableChannel
//* \brief Return ADC Timer Register Value
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_DisableChannel (
	AT91PS_ADC pADC, // pointer to a ADC controller
	unsigned int channel)        // mode register 
{
	//* Write to the CHDR register
	pADC->ADC_CHDR = channel;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetChannelStatus
//* \brief Return ADC Timer Register Value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetChannelStatus (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CHSR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_StartConversion
//* \brief Software request for a analog to digital conversion 
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_StartConversion (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	pADC->ADC_CR = AT91C_ADC_START;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_SoftReset
//* \brief Software reset
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_SoftReset (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	pADC->ADC_CR = AT91C_ADC_SWRST;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetLastConvertedData
//* \brief Return the Last Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetLastConvertedData (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_LCDR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetConvertedDataCH0
//* \brief Return the Channel 0 Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetConvertedDataCH0 (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CDR0;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetConvertedDataCH1
//* \brief Return the Channel 1 Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetConvertedDataCH1 (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CDR1;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetConvertedDataCH2
//* \brief Return the Channel 2 Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetConvertedDataCH2 (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CDR2;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetConvertedDataCH3
//* \brief Return the Channel 3 Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetConvertedDataCH3 (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CDR3;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetConvertedDataCH4
//* \brief Return the Channel 4 Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetConvertedDataCH4 (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CDR4;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetConvertedDataCH5
//* \brief Return the Channel 5 Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetConvertedDataCH5 (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CDR5;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetConvertedDataCH6
//* \brief Return the Channel 6 Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetConvertedDataCH6 (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CDR6;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_GetConvertedDataCH7
//* \brief Return the Channel 7 Converted Data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ADC_GetConvertedDataCH7 (
	AT91PS_ADC pADC // pointer to a ADC controller
	)
{
	return pADC->ADC_CDR7;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_CfgPMC
//* \brief Enable Peripheral clock in PMC for  DBGU
//*----------------------------------------------------------------------------
__inline void AT91F_DBGU_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_CfgPIO
//* \brief Configure PIO controllers to drive DBGU signals
//*----------------------------------------------------------------------------
__inline void AT91F_DBGU_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA27_DRXD    ) |
		((unsigned int) AT91C_PA28_DTXD    ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PMC
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgPIO
//* \brief Configure PIO controllers to drive PMC signals
//*----------------------------------------------------------------------------
__inline void AT91F_PMC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB30_PCK2    ) |
		((unsigned int) AT91C_PB29_PCK1    ), // Peripheral A
		((unsigned int) AT91C_PB20_PCK0    ) |
		((unsigned int) AT91C_PB0_PCK0    ) |
		((unsigned int) AT91C_PB22_PCK2    ) |
		((unsigned int) AT91C_PB21_PCK1    )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA30_PCK2    ) |
		((unsigned int) AT91C_PA13_PCK1    ) |
		((unsigned int) AT91C_PA27_PCK3    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_VREG_CfgPMC
//* \brief Enable Peripheral clock in PMC for  VREG
//*----------------------------------------------------------------------------
__inline void AT91F_VREG_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  RSTC
//*----------------------------------------------------------------------------
__inline void AT91F_RSTC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SSC
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SSC));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_CfgPIO
//* \brief Configure PIO controllers to drive SSC signals
//*----------------------------------------------------------------------------
__inline void AT91F_SSC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA25_RK      ) |
		((unsigned int) AT91C_PA22_TK      ) |
		((unsigned int) AT91C_PA21_TF      ) |
		((unsigned int) AT91C_PA24_RD      ) |
		((unsigned int) AT91C_PA26_RF      ) |
		((unsigned int) AT91C_PA23_TD      ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  WDTC
//*----------------------------------------------------------------------------
__inline void AT91F_WDTC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US1_CfgPMC
//* \brief Enable Peripheral clock in PMC for  US1
//*----------------------------------------------------------------------------
__inline void AT91F_US1_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_US1));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US1_CfgPIO
//* \brief Configure PIO controllers to drive US1 signals
//*----------------------------------------------------------------------------
__inline void AT91F_US1_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PB26_RI1     ) |
		((unsigned int) AT91C_PB24_DSR1    ) |
		((unsigned int) AT91C_PB23_DCD1    ) |
		((unsigned int) AT91C_PB25_DTR1    )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA7_SCK1    ) |
		((unsigned int) AT91C_PA8_RTS1    ) |
		((unsigned int) AT91C_PA6_TXD1    ) |
		((unsigned int) AT91C_PA5_RXD1    ) |
		((unsigned int) AT91C_PA9_CTS1    ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US0_CfgPMC
//* \brief Enable Peripheral clock in PMC for  US0
//*----------------------------------------------------------------------------
__inline void AT91F_US0_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_US0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US0_CfgPIO
//* \brief Configure PIO controllers to drive US0 signals
//*----------------------------------------------------------------------------
__inline void AT91F_US0_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA0_RXD0    ) |
		((unsigned int) AT91C_PA4_CTS0    ) |
		((unsigned int) AT91C_PA3_RTS0    ) |
		((unsigned int) AT91C_PA2_SCK0    ) |
		((unsigned int) AT91C_PA1_TXD0    ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI1_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SPI1
//*----------------------------------------------------------------------------
__inline void AT91F_SPI1_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SPI1));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI1_CfgPIO
//* \brief Configure PIO controllers to drive SPI1 signals
//*----------------------------------------------------------------------------
__inline void AT91F_SPI1_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PB11_SPI1_NPCS2) |
		((unsigned int) AT91C_PB10_SPI1_NPCS1) |
		((unsigned int) AT91C_PB16_SPI1_NPCS3)); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA22_SPI1_SPCK) |
		((unsigned int) AT91C_PA3_SPI1_NPCS2) |
		((unsigned int) AT91C_PA26_SPI1_NPCS2) |
		((unsigned int) AT91C_PA25_SPI1_NPCS1) |
		((unsigned int) AT91C_PA2_SPI1_NPCS1) |
		((unsigned int) AT91C_PA24_SPI1_MISO) |
		((unsigned int) AT91C_PA4_SPI1_NPCS3) |
		((unsigned int) AT91C_PA29_SPI1_NPCS3) |
		((unsigned int) AT91C_PA21_SPI1_NPCS0) |
		((unsigned int) AT91C_PA23_SPI1_MOSI)); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI0_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SPI0
//*----------------------------------------------------------------------------
__inline void AT91F_SPI0_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SPI0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI0_CfgPIO
//* \brief Configure PIO controllers to drive SPI0 signals
//*----------------------------------------------------------------------------
__inline void AT91F_SPI0_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PB13_SPI0_NPCS1) |
		((unsigned int) AT91C_PB14_SPI0_NPCS2) |
		((unsigned int) AT91C_PB17_SPI0_NPCS3)); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA16_SPI0_MISO) |
		((unsigned int) AT91C_PA13_SPI0_NPCS1) |
		((unsigned int) AT91C_PA14_SPI0_NPCS2) |
		((unsigned int) AT91C_PA12_SPI0_NPCS0) |
		((unsigned int) AT91C_PA17_SPI0_MOSI) |
		((unsigned int) AT91C_PA15_SPI0_NPCS3) |
		((unsigned int) AT91C_PA18_SPI0_SPCK), // Peripheral A
		((unsigned int) AT91C_PA7_SPI0_NPCS1) |
		((unsigned int) AT91C_PA8_SPI0_NPCS2) |
		((unsigned int) AT91C_PA9_SPI0_NPCS3)); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PITC
//*----------------------------------------------------------------------------
__inline void AT91F_PITC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  AIC
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_FIQ) |
		((unsigned int) 1 << AT91C_ID_IRQ0) |
		((unsigned int) 1 << AT91C_ID_IRQ1));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_CfgPIO
//* \brief Configure PIO controllers to drive AIC signals
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA30_IRQ0    ) |
		((unsigned int) AT91C_PA29_FIQ     ), // Peripheral A
		((unsigned int) AT91C_PA14_IRQ1    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_CfgPMC
//* \brief Enable Peripheral clock in PMC for  TWI
//*----------------------------------------------------------------------------
__inline void AT91F_TWI_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TWI));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_CfgPIO
//* \brief Configure PIO controllers to drive TWI signals
//*----------------------------------------------------------------------------
__inline void AT91F_TWI_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA11_TWCK    ) |
		((unsigned int) AT91C_PA10_TWD     ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  ADC
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_ADC));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ADC_CfgPIO
//* \brief Configure PIO controllers to drive ADC signals
//*----------------------------------------------------------------------------
__inline void AT91F_ADC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PB18_ADTRG   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWMC_CH3_CfgPIO
//* \brief Configure PIO controllers to drive PWMC_CH3 signals
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_CH3_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB22_PWM3    ), // Peripheral A
		((unsigned int) AT91C_PB30_PWM3    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWMC_CH2_CfgPIO
//* \brief Configure PIO controllers to drive PWMC_CH2 signals
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_CH2_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB21_PWM2    ), // Peripheral A
		((unsigned int) AT91C_PB29_PWM2    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWMC_CH1_CfgPIO
//* \brief Configure PIO controllers to drive PWMC_CH1 signals
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_CH1_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB20_PWM1    ), // Peripheral A
		((unsigned int) AT91C_PB28_PWM1    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWMC_CH0_CfgPIO
//* \brief Configure PIO controllers to drive PWMC_CH0 signals
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_CH0_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB19_PWM0    ), // Peripheral A
		((unsigned int) AT91C_PB27_PWM0    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RTTC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  RTTC
//*----------------------------------------------------------------------------
__inline void AT91F_RTTC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_CfgPMC
//* \brief Enable Peripheral clock in PMC for  UDP
//*----------------------------------------------------------------------------
__inline void AT91F_UDP_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_UDP));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_EMAC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  EMAC
//*----------------------------------------------------------------------------
__inline void AT91F_EMAC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_EMAC));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_EMAC_CfgPIO
//* \brief Configure PIO controllers to drive EMAC signals
//*----------------------------------------------------------------------------
__inline void AT91F_EMAC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB2_ETX0    ) |
		((unsigned int) AT91C_PB12_ETXER   ) |
		((unsigned int) AT91C_PB16_ECOL    ) |
		((unsigned int) AT91C_PB15_ERXDV_ECRSDV) |
		((unsigned int) AT91C_PB11_ETX3    ) |
		((unsigned int) AT91C_PB6_ERX1    ) |
		((unsigned int) AT91C_PB13_ERX2    ) |
		((unsigned int) AT91C_PB3_ETX1    ) |
		((unsigned int) AT91C_PB4_ECRS    ) |
		((unsigned int) AT91C_PB8_EMDC    ) |
		((unsigned int) AT91C_PB5_ERX0    ) |
		((unsigned int) AT91C_PB18_EF100   ) |
		((unsigned int) AT91C_PB14_ERX3    ) |
		((unsigned int) AT91C_PB1_ETXEN   ) |
		((unsigned int) AT91C_PB10_ETX2    ) |
		((unsigned int) AT91C_PB0_ETXCK_EREFCK) |
		((unsigned int) AT91C_PB9_EMDIO   ) |
		((unsigned int) AT91C_PB7_ERXER   ) |
		((unsigned int) AT91C_PB17_ERXCK   ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC0_CfgPMC
//* \brief Enable Peripheral clock in PMC for  TC0
//*----------------------------------------------------------------------------
__inline void AT91F_TC0_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TC0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC0_CfgPIO
//* \brief Configure PIO controllers to drive TC0 signals
//*----------------------------------------------------------------------------
__inline void AT91F_TC0_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB23_TIOA0   ) |
		((unsigned int) AT91C_PB24_TIOB0   ), // Peripheral A
		((unsigned int) AT91C_PB12_TCLK0   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC1_CfgPMC
//* \brief Enable Peripheral clock in PMC for  TC1
//*----------------------------------------------------------------------------
__inline void AT91F_TC1_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TC1));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC1_CfgPIO
//* \brief Configure PIO controllers to drive TC1 signals
//*----------------------------------------------------------------------------
__inline void AT91F_TC1_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB25_TIOA1   ) |
		((unsigned int) AT91C_PB26_TIOB1   ), // Peripheral A
		((unsigned int) AT91C_PB19_TCLK1   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC2_CfgPMC
//* \brief Enable Peripheral clock in PMC for  TC2
//*----------------------------------------------------------------------------
__inline void AT91F_TC2_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TC2));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC2_CfgPIO
//* \brief Configure PIO controllers to drive TC2 signals
//*----------------------------------------------------------------------------
__inline void AT91F_TC2_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB28_TIOB2   ) |
		((unsigned int) AT91C_PB27_TIOA2   ), // Peripheral A
		0); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA15_TCLK2   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  MC
//*----------------------------------------------------------------------------
__inline void AT91F_MC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIOA_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PIOA
//*----------------------------------------------------------------------------
__inline void AT91F_PIOA_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_PIOA));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIOB_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PIOB
//*----------------------------------------------------------------------------
__inline void AT91F_PIOB_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_PIOB));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgPMC
//* \brief Enable Peripheral clock in PMC for  CAN
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_CAN));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CAN_CfgPIO
//* \brief Configure PIO controllers to drive CAN signals
//*----------------------------------------------------------------------------
__inline void AT91F_CAN_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA20_CANTX   ) |
		((unsigned int) AT91C_PA19_CANRX   ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PWMC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PWMC
//*----------------------------------------------------------------------------
__inline void AT91F_PWMC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_PWMC));
}

#endif // lib_AT91SAM7X256_H

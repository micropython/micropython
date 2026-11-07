/*!
    \file    gd32f4xx_fmc.c
    \brief   FMC driver

    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/


#include "gd32f4xx_fmc.h"

/*!
    \brief    set the FMC wait state counter
    \param[in]  wscnt: wait state counter value
                only one parameter can be selected which is shown as below:
      \arg        WS_WSCNT_0: FMC 0 wait
      \arg        WS_WSCNT_1: FMC 1 wait
      \arg        WS_WSCNT_2: FMC 2 wait
      \arg        WS_WSCNT_3: FMC 3 wait
      \arg        WS_WSCNT_4: FMC 4 wait
      \arg        WS_WSCNT_5: FMC 5 wait
      \arg        WS_WSCNT_6: FMC 6 wait
      \arg        WS_WSCNT_7: FMC 7 wait
      \arg        WS_WSCNT_8: FMC 8 wait
      \arg        WS_WSCNT_9: FMC 9 wait
      \arg        WS_WSCNT_10: FMC 10 wait
      \arg        WS_WSCNT_11: FMC 11 wait
      \arg        WS_WSCNT_12: FMC 12 wait
      \arg        WS_WSCNT_13: FMC 13 wait
      \arg        WS_WSCNT_14: FMC 14 wait
      \arg        WS_WSCNT_15: FMC 15 wait
    \param[out] none
    \retval     none
*/
void fmc_wscnt_set(uint32_t wscnt)
{
    uint32_t reg;

    reg = FMC_WS;
    /* set the wait state counter value */
    reg &= ~FMC_WC_WSCNT;
    FMC_WS = (reg | wscnt);
}

/*!
    \brief    unlock the main FMC operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_unlock(void)
{
    if((RESET != (FMC_CTL & FMC_CTL_LK))) {
        /* write the FMC key */
        FMC_KEY = UNLOCK_KEY0;
        FMC_KEY = UNLOCK_KEY1;
    }
}

/*!
    \brief    lock the main FMC operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_lock(void)
{
    /* set the LK bit*/
    FMC_CTL |= FMC_CTL_LK;
}

#if defined (GD32F425) || defined (GD32F427) || defined (GD32F470)

/*!
    \brief      FMC erase page
    \param[in]  page_addr: the page address to be erased.
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_page_erase(uint32_t page_addr)
{
    fmc_state_enum fmc_state = FMC_READY;

    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        /* unlock page erase operation */
        FMC_PEKEY = UNLOCK_PE_KEY;

        /* start page erase */
        FMC_PECFG = FMC_PE_EN | page_addr;
        FMC_CTL &= ~FMC_CTL_SN;
        FMC_CTL |= FMC_CTL_SER;
        FMC_CTL |= FMC_CTL_START;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        FMC_PECFG &= ~FMC_PE_EN;
        FMC_CTL &= ~FMC_CTL_SER;
    }

    /* return the FMC state */
    return fmc_state;
}

#endif

/*!
    \brief    FMC erase sector
    \param[in]  fmc_sector: select the sector to erase
                only one parameter can be selected which is shown as below:
      \arg        CTL_SECTOR_NUMBER_0: sector 0
      \arg        CTL_SECTOR_NUMBER_1: sector 1
      \arg        CTL_SECTOR_NUMBER_2: sector 2
      \arg        CTL_SECTOR_NUMBER_3: sector 3
      \arg        CTL_SECTOR_NUMBER_4: sector 4
      \arg        CTL_SECTOR_NUMBER_5: sector 5
      \arg        CTL_SECTOR_NUMBER_6: sector 6
      \arg        CTL_SECTOR_NUMBER_7: sector 7
      \arg        CTL_SECTOR_NUMBER_8: sector 8
      \arg        CTL_SECTOR_NUMBER_9: sector 9
      \arg        CTL_SECTOR_NUMBER_10: sector 10
      \arg        CTL_SECTOR_NUMBER_11: sector 11
      \arg        CTL_SECTOR_NUMBER_12: sector 12
      \arg        CTL_SECTOR_NUMBER_13: sector 13
      \arg        CTL_SECTOR_NUMBER_14: sector 14
      \arg        CTL_SECTOR_NUMBER_15: sector 15
      \arg        CTL_SECTOR_NUMBER_16: sector 16
      \arg        CTL_SECTOR_NUMBER_17: sector 17
      \arg        CTL_SECTOR_NUMBER_18: sector 18
      \arg        CTL_SECTOR_NUMBER_19: sector 19
      \arg        CTL_SECTOR_NUMBER_20: sector 20
      \arg        CTL_SECTOR_NUMBER_21: sector 21
      \arg        CTL_SECTOR_NUMBER_22: sector 22
      \arg        CTL_SECTOR_NUMBER_23: sector 23
      \arg        CTL_SECTOR_NUMBER_24: sector 24
      \arg        CTL_SECTOR_NUMBER_25: sector 25
      \arg        CTL_SECTOR_NUMBER_26: sector 26
      \arg        CTL_SECTOR_NUMBER_27: sector 27
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_sector_erase(uint32_t fmc_sector)
{
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        /* start sector erase */
        FMC_CTL &= ~FMC_CTL_SN;
        FMC_CTL |= (FMC_CTL_SER | fmc_sector);
        FMC_CTL |= FMC_CTL_START;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        /* reset the SER bit */
        FMC_CTL &= (~FMC_CTL_SER);
        FMC_CTL &= ~FMC_CTL_SN;
    }

    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief    FMC erase whole chip
    \param[in]  none
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_mass_erase(void)
{
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        /* start whole chip erase */
        FMC_CTL |= (FMC_CTL_MER0 | FMC_CTL_MER1);
        FMC_CTL |= FMC_CTL_START;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        /* reset the MER bits */
        FMC_CTL &= ~(FMC_CTL_MER0 | FMC_CTL_MER1);
    }

    /* return the fmc state */
    return fmc_state;
}

/*!
    \brief    FMC erase whole bank0
    \param[in]  none
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_bank0_erase(void)
{
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        /* start FMC bank0 erase */
        FMC_CTL |= FMC_CTL_MER0;
        FMC_CTL |= FMC_CTL_START;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        /* reset the MER0 bit */
        FMC_CTL &= (~FMC_CTL_MER0);
    }

    /* return the fmc state */
    return fmc_state;
}

/*!
    \brief    FMC erase whole bank1
    \param[in]  none
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_bank1_erase(void)
{
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        /* start FMC bank1 erase */
        FMC_CTL |= FMC_CTL_MER1;
        FMC_CTL |= FMC_CTL_START;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        /* reset the MER1 bit */
        FMC_CTL &= (~FMC_CTL_MER1);
    }

    /* return the fmc state */
    return fmc_state;
}

/*!
    \brief    program a word at the corresponding address
    \param[in]  address: address to program
    \param[in]  data: word to program(0x00000000 - 0xFFFFFFFF)
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_word_program(uint32_t address, uint32_t data)
{
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        /* set the PG bit to start program */
        FMC_CTL &= ~FMC_CTL_PSZ;
        FMC_CTL |= CTL_PSZ_WORD;
        FMC_CTL |= FMC_CTL_PG;

        REG32(address) = data;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }

    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief    program a half word at the corresponding address
    \param[in]  address: address to program
    \param[in]  data: halfword to program(0x0000 - 0xFFFF)
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_halfword_program(uint32_t address, uint16_t data)
{
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        /* set the PG bit to start program */
        FMC_CTL &= ~FMC_CTL_PSZ;
        FMC_CTL |= CTL_PSZ_HALF_WORD;
        FMC_CTL |= FMC_CTL_PG;

        REG16(address) = data;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }

    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief    program a byte at the corresponding address
    \param[in]  address: address to program
    \param[in]  data: byte to program(0x00 - 0xFF)
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_byte_program(uint32_t address, uint8_t data)
{
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        /* set the PG bit to start program */
        FMC_CTL &= ~FMC_CTL_PSZ;
        FMC_CTL |= CTL_PSZ_BYTE;
        FMC_CTL |= FMC_CTL_PG;

        REG8(address) = data;

        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }

    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief    unlock the option byte operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ob_unlock(void)
{
    if(RESET != (FMC_OBCTL0 & FMC_OBCTL0_OB_LK)) {
        /* write the FMC key */
        FMC_OBKEY = OB_UNLOCK_KEY0;
        FMC_OBKEY = OB_UNLOCK_KEY1;
    }
}

/*!
    \brief    lock the option byte operation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ob_lock(void)
{
    /* reset the OB_LK bit */
    FMC_OBCTL0 |= FMC_OBCTL0_OB_LK;
}

/*!
    \brief    send option byte change command
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ob_start(void)
{
    /* set the OB_START bit in OBCTL0 register */
    FMC_OBCTL0 |= FMC_OBCTL0_OB_START;
}

/*!
    \brief    erase option byte
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ob_erase(void)
{
    uint32_t reg, reg1;
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
    reg = FMC_OBCTL0;
    reg1 = FMC_OBCTL1;

    if(FMC_READY == fmc_state) {

        /* reset the OB_FWDGT, OB_DEEPSLEEP and OB_STDBY, set according to ob_fwdgt ,ob_deepsleep and ob_stdby */
        reg |= (FMC_OBCTL0_NWDG_HW | FMC_OBCTL0_NRST_DPSLP | FMC_OBCTL0_NRST_STDBY);
        /* reset the BOR level */
        reg |= FMC_OBCTL0_BOR_TH;
        /* reset option byte boot bank value */
        reg &= ~FMC_OBCTL0_BB;
        /* reset option byte dbs value */
        reg &= ~FMC_OBCTL0_DBS;

        /* reset drp and wp value */
        reg |= FMC_OBCTL0_WP0;
        reg &= (~FMC_OBCTL0_DRP);
        FMC_OBCTL0 = reg;

        reg1 |= FMC_OBCTL1_WP1;
        FMC_OBCTL1 = reg1;

        FMC_OBCTL0 = reg;
    }
}

/*!
    \brief    enable write protection
    \param[in]  ob_wp: specify sector to be write protected
                one or more parameters can be selected which are shown as below:
      \arg        OB_WP_x(x=0..22):sector x(x = 0,1,2...22)
      \arg        OB_WP_23_27: sector23~27
      \arg        OB_WP_ALL: all sector
    \param[out] none
    \retval     SUCCESS or ERROR
*/
ErrStatus ob_write_protection_enable(uint32_t ob_wp)
{
    ErrStatus retval = SUCCESS;
    uint32_t reg0 = FMC_OBCTL0;
    uint32_t reg1 = FMC_OBCTL1;
    fmc_state_enum fmc_state = FMC_READY;
    if(RESET != (FMC_OBCTL0 & FMC_OBCTL0_DRP)) {
        retval = ERROR;
    } else {
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        
        if(FMC_READY == fmc_state) {
            reg0 &= (~((uint32_t)ob_wp << 16U));
            reg1 &= (~(ob_wp & 0xFFFF0000U));
            FMC_OBCTL0 = reg0;
            FMC_OBCTL1 = reg1;
        
            retval = SUCCESS;
        } else {
            retval = ERROR;
        }
    }
    return retval;
}

/*!
    \brief    disable write protection
    \param[in]  ob_wp: specify sector to be write protected
                one or more parameters can be selected which are shown as below:
      \arg        OB_WP_x(x=0..22):sector x(x = 0,1,2...22)
      \arg        OB_WP_23_27: sector23~27
      \arg        OB_WP_ALL: all sector
    \param[out] none
    \retval     SUCCESS or ERROR
*/
ErrStatus ob_write_protection_disable(uint32_t ob_wp)
{
    ErrStatus retval = SUCCESS;
    uint32_t reg0 = FMC_OBCTL0;
    uint32_t reg1 = FMC_OBCTL1;
    fmc_state_enum fmc_state = FMC_READY;
    if(RESET != (FMC_OBCTL0 & FMC_OBCTL0_DRP)) {
        retval = ERROR;
    } else {
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);
        
        if(FMC_READY == fmc_state) {
            reg0 |= ((uint32_t)ob_wp << 16U);
            reg1 |= (ob_wp & 0xFFFF0000U);
            FMC_OBCTL0 = reg0;
            FMC_OBCTL1 = reg1;
        
            retval = SUCCESS;
        } else {
            retval = ERROR;
        }
    }
    return retval;
}

/*!
    \brief    enable erase/program protection and D-bus read protection
    \param[in]  ob_drp: enable the WPx bits used as erase/program protection and D-bus read protection of each sector
                one or more parameters can be selected which are shown as below:
      \arg        OB_DRP_x(x=0..22): sector x(x = 0,1,2...22)
      \arg        OB_DRP_23_27: sector23~27
      \arg        OB_DRP_ALL: all sector
    \param[out] none
    \retval     none
*/
void ob_drp_enable(uint32_t ob_drp)
{
    uint32_t reg0 = FMC_OBCTL0;
    uint32_t reg1 = FMC_OBCTL1;
    fmc_state_enum fmc_state = FMC_READY;
    uint32_t drp_state = FMC_OBCTL0 & FMC_OBCTL0_DRP;

    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        if(RESET == drp_state) {
            reg0 &= ~FMC_OBCTL0_WP0;
            reg1 &= ~FMC_OBCTL1_WP1;
        }
        reg0 |= ((uint32_t)ob_drp << 16U);
        reg0 |= FMC_OBCTL0_DRP;
        reg1 |= ((uint32_t)ob_drp & 0xFFFF0000U);

        FMC_OBCTL0 = reg0;
        FMC_OBCTL1 = reg1;
    }
}

/*!
    \brief    disable erase/program protection and D-bus read protection
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ob_drp_disable(void)
{
    uint32_t reg0 = FMC_OBCTL0;
    uint32_t reg1 = FMC_OBCTL1;
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        if(((uint8_t)(reg0 >> 8U)) == (uint8_t)FMC_NSPC) {
            /* security protection should be set as low level protection before disable D-BUS read protection */
            reg0 &= ~FMC_OBCTL0_SPC;
            reg0 |= ((uint32_t)FMC_LSPC << 8U);
            FMC_OBCTL0 = reg0;
            /* set the OB_START bit in OBCTL0 register */
            FMC_OBCTL0 |= FMC_OBCTL0_OB_START;
        }

        /* it is necessary to disable the security protection at the same time when D-BUS read protection is disabled */
        reg0 &= ~FMC_OBCTL0_SPC;
        reg0 |= ((uint32_t)FMC_NSPC << 8U);
        reg0 |= FMC_OBCTL0_WP0;
        reg0 &= (~FMC_OBCTL0_DRP);
        FMC_OBCTL0 = reg0;

        reg1 |= FMC_OBCTL1_WP1;
        FMC_OBCTL1 = reg1;

    }
}

/*!
    \brief    configure security protection level
    \param[in]  ob_spc: specify security protection level
                only one parameter can be selected which is shown as below:
      \arg        FMC_NSPC: no security protection
      \arg        FMC_LSPC: low security protection
      \arg        FMC_HSPC: high security protection
    \param[out] none
    \retval     none
*/
void ob_security_protection_config(uint8_t ob_spc)
{
    fmc_state_enum fmc_state = FMC_READY;
    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        uint32_t reg;

        reg = FMC_OBCTL0;
        /* reset the OBCTL0_SPC, set according to ob_spc */
        reg &= ~FMC_OBCTL0_SPC;
        reg |= ((uint32_t)ob_spc << 8U);
        FMC_OBCTL0 = reg;
    }
}

/*!
    \brief    program the FMC user option byte
    \param[in]  ob_fwdgt: option byte watchdog value
                only one parameter can be selected which is shown as below:
      \arg        OB_FWDGT_SW: software free watchdog
      \arg        OB_FWDGT_HW: hardware free watchdog
    \param[in]  ob_deepsleep: option byte deepsleep reset value
                only one parameter can be selected which is shown as below:
      \arg        OB_DEEPSLEEP_NRST: no reset when entering deepsleep mode
      \arg        OB_DEEPSLEEP_RST: generate a reset instead of entering deepsleep mode
    \param[in]  ob_stdby:option byte standby reset value
                only one parameter can be selected which is shown as below:
      \arg        OB_STDBY_NRST: no reset when entering standby mode
      \arg        OB_STDBY_RST: generate a reset instead of entering standby mode
    \param[out] none
    \retval     none
*/
void ob_user_write(uint32_t ob_fwdgt, uint32_t ob_deepsleep, uint32_t ob_stdby)
{
    fmc_state_enum fmc_state = FMC_READY;

    /* wait for the FMC ready */
    fmc_state = fmc_ready_wait(FMC_TIMEOUT_COUNT);

    if(FMC_READY == fmc_state) {
        uint32_t reg;

        reg = FMC_OBCTL0;
        /* reset the OB_FWDGT, OB_DEEPSLEEP and OB_STDBY, set according to ob_fwdgt ,ob_deepsleep and ob_stdby */
        reg &= ~(FMC_OBCTL0_NWDG_HW | FMC_OBCTL0_NRST_DPSLP | FMC_OBCTL0_NRST_STDBY);
        FMC_OBCTL0 = (reg | ob_fwdgt | ob_deepsleep | ob_stdby);
    }
}

/*!
    \brief    program the option byte BOR threshold value
    \param[in]  ob_bor_th: user option byte
                only one parameter can be selected which is shown as below:
      \arg        OB_BOR_TH_VALUE3: BOR threshold value 3
      \arg        OB_BOR_TH_VALUE2: BOR threshold value 2
      \arg        OB_BOR_TH_VALUE1: BOR threshold value 1
      \arg        OB_BOR_TH_OFF: no BOR function
    \param[out] none
    \retval     none
*/
void ob_user_bor_threshold(uint32_t ob_bor_th)
{
    uint32_t reg;

    reg = FMC_OBCTL0;
    /* set the BOR level */
    reg &= ~FMC_OBCTL0_BOR_TH;
    FMC_OBCTL0 = (reg | ob_bor_th);
}

/*!
    \brief    configure the option byte boot bank value
    \param[in]  boot_mode: specifies the option byte boot bank value
                only one parameter can be selected which is shown as below:
      \arg        OB_BB_DISABLE: boot from bank0
      \arg        OB_BB_ENABLE: boot from bank1 or bank0 if bank1 is void
    \param[out] none
    \retval     none
*/
void ob_boot_mode_config(uint32_t boot_mode)
{
    uint32_t reg;

    reg = FMC_OBCTL0;
    /* set option byte boot bank value */
    reg &= ~FMC_OBCTL0_BB;
    FMC_OBCTL0 = (reg | boot_mode);
}

#if defined (GD32F450) || defined (GD32F470)
/*!
    \brief    configure the option byte double bank select, only for 1MB flash memory series
    \param[in]  double_bank: specifies the option byte double bank select
                only one parameter can be selected which is shown as below:
      \arg        OB_DBS_DISABLE: single bank when flash size is 1M bytes
      \arg        OB_DBS_ENABLE: double banks when flash size is 1M bytes
    \param[out] none
    \retval     none
*/
void ob_double_bank_select(uint32_t double_bank)
{
    uint32_t reg;

    reg = FMC_OBCTL0;
    /* set option byte double bank select */
    reg &= ~FMC_OBCTL0_DBS;
    FMC_OBCTL0 = (reg | double_bank);
}
#endif

/*!
    \brief    get the FMC user option byte
    \param[in]  none
    \param[out] none
    \retval     the FMC user option byte values: ob_fwdgt(Bit0), ob_deepsleep(Bit1), ob_stdby(Bit2)
*/
uint8_t ob_user_get(void)
{
    return (uint8_t)((uint8_t)(FMC_OBCTL0 >> 5U) & 0x07U);
}

/*!
    \brief    get the FMC option byte write protection
    \param[in]  none
    \param[out] none
    \retval     the FMC write protection option byte value
*/
uint16_t ob_write_protection0_get(void)
{
    /* return the FMC write protection option byte value */
    return (uint16_t)(((uint16_t)(FMC_OBCTL0 >> 16U)) & 0x0FFFU);
}

/*!
    \brief    get the FMC option byte write protection
    \param[in]  none
    \param[out] none
    \retval     the FMC write protection option byte value
*/
uint16_t ob_write_protection1_get(void)
{
    /* return the the FMC write protection option byte value */
    return (uint16_t)(((uint16_t)(FMC_OBCTL1 >> 16U)) & 0x0FFFU);
}

/*!
    \brief    get the FMC erase/program protection and D-bus read protection option bytes value
    \param[in]  none
    \param[out] none
    \retval     the FMC erase/program protection and D-bus read protection option bytes value
*/
uint16_t ob_drp0_get(void)
{
    uint16_t retval;
    /* return the FMC erase/program protection and D-bus read protection option bytes value */
    if(FMC_OBCTL0 & FMC_OBCTL0_DRP) {
        retval = (uint16_t)(((uint16_t)(FMC_OBCTL0 >> 16U)) & 0x0FFFU);
    } else {
        retval = 0xF000U;
    }
    return retval;
}

/*!
    \brief    get the FMC erase/program protection and D-bus read protection option bytes value
    \param[in]  none
    \param[out] none
    \retval     the FMC erase/program protection and D-bus read protection option bytes value
*/
uint16_t ob_drp1_get(void)
{
    uint16_t retval;
    /* return the FMC erase/program protection and D-bus read protection option bytes value */
    if(FMC_OBCTL0 & FMC_OBCTL0_DRP) {
        retval = (uint16_t)(((uint16_t)(FMC_OBCTL1 >> 16U)) & 0x0FFFU);
    } else {
        retval = 0xF000U;
    }
    return retval;
}

/*!
    \brief    get option byte security protection code value
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus ob_spc_get(void)
{
    FlagStatus spc_state = RESET;

    if(((uint8_t)(FMC_OBCTL0 >> 8U)) != FMC_NSPC) {
        spc_state = SET;
    } else {
        spc_state = RESET;
    }
    return spc_state;
}

/*!
    \brief    get the FMC option byte BOR threshold value
    \param[in]  none
    \param[out] none
    \retval     the FMC BOR threshold value:OB_BOR_TH_OFF,OB_BOR_TH_VALUE1,OB_BOR_TH_VALUE2,OB_BOR_TH_VALUE3
*/
uint8_t ob_user_bor_threshold_get(void)
{
    /* return the FMC BOR threshold value */
    return (uint8_t)((uint8_t)FMC_OBCTL0 & 0x0CU);
}

/*!
    \brief    get flag set or reset
    \param[in]  fmc_flag: check FMC flag
                only one parameter can be selected which is shown as below:
      \arg        FMC_FLAG_BUSY: FMC busy flag bit
      \arg        FMC_FLAG_RDDERR: FMC read D-bus protection error flag bit
      \arg        FMC_FLAG_PGSERR: FMC program sequence error flag bit
      \arg        FMC_FLAG_PGMERR: FMC program size not match error flag bit
      \arg        FMC_FLAG_WPERR: FMC Erase/Program protection error flag bit
      \arg        FMC_FLAG_OPERR: FMC operation error flag bit
      \arg        FMC_FLAG_END: FMC end of operation flag bit
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus fmc_flag_get(uint32_t fmc_flag)
{
    FlagStatus retval = RESET;
    if(FMC_STAT & fmc_flag) {
        retval = SET;
    }
    /* return the state of corresponding FMC flag */
    return retval;
}

/*!
    \brief    clear the FMC pending flag
    \param[in]  FMC_flag: clear FMC flag
      \arg        FMC_FLAG_RDDERR: FMC read D-bus protection error flag bit
      \arg        FMC_FLAG_PGSERR: FMC program sequence error flag bit
      \arg        FMC_FLAG_PGMERR: FMC program size not match error flag bit
      \arg        FMC_FLAG_WPERR: FMC erase/program protection error flag bit
      \arg        FMC_FLAG_OPERR: FMC operation error flag bit
      \arg        FMC_FLAG_END: FMC end of operation flag bit
    \param[out] none
    \retval     none
*/
void fmc_flag_clear(uint32_t fmc_flag)
{
    /* clear the flags */
    FMC_STAT = fmc_flag;
}

/*!
    \brief    enable FMC interrupt
    \param[in]  fmc_int: the FMC interrupt source
      \arg        FMC_INT_END: enable FMC end of program interrupt
      \arg        FMC_INT_ERR: enable FMC error interrupt
    \param[out] none
    \retval     none
*/
void fmc_interrupt_enable(uint32_t fmc_int)
{
    FMC_CTL |= fmc_int;
}

/*!
    \brief    disable FMC interrupt
    \param[in]  fmc_int: the FMC interrupt source
      \arg        FMC_INT_END: disable FMC end of program interrupt
      \arg        FMC_INT_ERR: disable FMC error interrupt
    \param[out] none
    \retval     none
*/
void fmc_interrupt_disable(uint32_t fmc_int)
{
    FMC_CTL &= ~(uint32_t)fmc_int;
}

/*!
    \brief    get FMC interrupt flag set or reset
    \param[in]  fmc_int_flag: FMC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        FMC_INT_FLAG_RDDERR: FMC read D-bus protection error interrupt flag
      \arg        FMC_INT_FLAG_PGSERR: FMC program sequence error interrupt flag
      \arg        FMC_INT_FLAG_PGMERR: FMC program size not match error interrupt flag
      \arg        FMC_INT_FLAG_WPERR: FMC Erase/Program protection error interrupt flag
      \arg        FMC_INT_FLAG_OPERR: FMC operation error interrupt flag
      \arg        FMC_INT_FLAG_END: FMC end of operation interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus fmc_interrupt_flag_get(uint32_t fmc_int_flag)
{
    FlagStatus retval = RESET;
    if(FMC_FLAG_END == fmc_int_flag) {
        /* end of operation interrupt flag */
        if(FMC_CTL & FMC_CTL_ENDIE) {
            if(FMC_STAT & fmc_int_flag) {
                retval = SET;
            }
        }
    } else {
        /* error interrupt flags */
        if(FMC_CTL & FMC_CTL_ERRIE) {
            if(FMC_STAT & fmc_int_flag) {
                retval = SET;
            }
        }
    }

    return retval;
}

/*!
    \brief    clear the FMC interrupt flag
    \param[in]  fmc_int_flag: FMC interrupt flag
      \arg        FMC_INT_FLAG_RDDERR: FMC read D-bus protection error interrupt flag
      \arg        FMC_INT_FLAG_PGSERR: FMC program sequence error interrupt flag
      \arg        FMC_INT_FLAG_PGMERR: FMC program size not match error interrupt flag
      \arg        FMC_INT_FLAG_WPERR: FMC Erase/Program protection error interrupt flag
      \arg        FMC_INT_FLAG_OPERR: FMC operation error interrupt flag
      \arg        FMC_INT_FLAG_END: FMC end of operation interrupt flag
    \param[out] none
    \retval     none
*/
void fmc_interrupt_flag_clear(uint32_t fmc_int_flag)
{
    /* clear the interrupt flag */
    FMC_STAT = fmc_int_flag;
}

/*!
    \brief    get the FMC state
    \param[in]  none
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
*/
fmc_state_enum fmc_state_get(void)
{
    fmc_state_enum fmc_state = FMC_READY;
    uint32_t temp_val = FMC_STAT;

    if(RESET != (temp_val & FMC_FLAG_BUSY)) {
        fmc_state = FMC_BUSY;
    } else if(RESET != (temp_val & FMC_FLAG_RDDERR)) {
        fmc_state = FMC_RDDERR;
    } else if(RESET != (temp_val & FMC_FLAG_PGSERR)) {
        fmc_state = FMC_PGSERR;
    } else if(RESET != (temp_val & FMC_FLAG_PGMERR)) {
        fmc_state = FMC_PGMERR;
    } else if(RESET != (temp_val & FMC_FLAG_WPERR)) {
        fmc_state = FMC_WPERR;
    } else if(RESET != (temp_val & FMC_FLAG_OPERR)) {
        fmc_state = FMC_OPERR;
    } else {
        fmc_state = FMC_READY;
    }

    /* return the FMC state */
    return fmc_state;
}

/*!
    \brief    check whether FMC is ready or not
    \param[in]  none
    \param[out] none
    \retval     state of FMC
      \arg        FMC_READY: the operation has been completed
      \arg        FMC_BUSY: the operation is in progress
      \arg        FMC_RDDERR: read D-bus protection error
      \arg        FMC_PGSERR: program sequence error
      \arg        FMC_PGMERR: program size not match error
      \arg        FMC_WPERR: erase/program protection error
      \arg        FMC_OPERR: operation error
      \arg        FMC_TOERR: timeout error
*/
fmc_state_enum fmc_ready_wait(uint32_t timeout)
{
    fmc_state_enum fmc_state = FMC_BUSY;

    /* wait for FMC ready */
    do {
        /* get FMC state */
        fmc_state = fmc_state_get();
        timeout--;
    } while((FMC_BUSY == fmc_state) && (0U != timeout));

    if(0U == timeout) {
        fmc_state = FMC_TOERR;
    }

    /* return the FMC state */
    return fmc_state;
}

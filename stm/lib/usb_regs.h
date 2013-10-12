/**
  ******************************************************************************
  * @file    usb_regs.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   hardware registers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_OTG_REGS_H__
#define __USB_OTG_REGS_H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"


/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_REGS
  * @brief This file is the 
  * @{
  */ 


/** @defgroup USB_REGS_Exported_Defines
  * @{
  */ 

#define USB_OTG_HS_BASE_ADDR                 0x40040000
#define USB_OTG_FS_BASE_ADDR                 0x50000000

#define USB_OTG_CORE_GLOBAL_REGS_OFFSET      0x000
#define USB_OTG_DEV_GLOBAL_REG_OFFSET        0x800
#define USB_OTG_DEV_IN_EP_REG_OFFSET         0x900
#define USB_OTG_EP_REG_OFFSET                0x20
#define USB_OTG_DEV_OUT_EP_REG_OFFSET        0xB00
#define USB_OTG_HOST_GLOBAL_REG_OFFSET       0x400
#define USB_OTG_HOST_PORT_REGS_OFFSET        0x440
#define USB_OTG_HOST_CHAN_REGS_OFFSET        0x500
#define USB_OTG_CHAN_REGS_OFFSET             0x20
#define USB_OTG_PCGCCTL_OFFSET               0xE00
#define USB_OTG_DATA_FIFO_OFFSET             0x1000
#define USB_OTG_DATA_FIFO_SIZE               0x1000


#define USB_OTG_MAX_TX_FIFOS                 15

#define USB_OTG_HS_MAX_PACKET_SIZE           512
#define USB_OTG_FS_MAX_PACKET_SIZE           64
#define USB_OTG_MAX_EP0_SIZE                 64
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_Types
  * @{
  */ 

/** @defgroup __USB_OTG_Core_register
  * @{
  */
typedef struct _USB_OTG_GREGS  //000h
{
  __IO uint32_t GOTGCTL;      /* USB_OTG Control and Status Register    000h*/
  __IO uint32_t GOTGINT;      /* USB_OTG Interrupt Register             004h*/
  __IO uint32_t GAHBCFG;      /* Core AHB Configuration Register    008h*/
  __IO uint32_t GUSBCFG;      /* Core USB Configuration Register    00Ch*/
  __IO uint32_t GRSTCTL;      /* Core Reset Register                010h*/
  __IO uint32_t GINTSTS;      /* Core Interrupt Register            014h*/
  __IO uint32_t GINTMSK;      /* Core Interrupt Mask Register       018h*/
  __IO uint32_t GRXSTSR;      /* Receive Sts Q Read Register        01Ch*/
  __IO uint32_t GRXSTSP;      /* Receive Sts Q Read & POP Register  020h*/
  __IO uint32_t GRXFSIZ;      /* Receive FIFO Size Register         024h*/
  __IO uint32_t DIEPTXF0_HNPTXFSIZ;   /* EP0 / Non Periodic Tx FIFO Size Register 028h*/
  __IO uint32_t HNPTXSTS;     /* Non Periodic Tx FIFO/Queue Sts reg 02Ch*/
  uint32_t Reserved30[2];     /* Reserved                           030h*/
  __IO uint32_t GCCFG;        /* General Purpose IO Register        038h*/
  __IO uint32_t CID;          /* User ID Register                   03Ch*/
  uint32_t  Reserved40[48];   /* Reserved                      040h-0FFh*/
  __IO uint32_t HPTXFSIZ; /* Host Periodic Tx FIFO Size Reg     100h*/
  __IO uint32_t DIEPTXF[USB_OTG_MAX_TX_FIFOS];/* dev Periodic Transmit FIFO */
}
USB_OTG_GREGS;
/**
  * @}
  */


/** @defgroup __device_Registers
  * @{
  */
typedef struct _USB_OTG_DREGS // 800h
{
  __IO uint32_t DCFG;         /* dev Configuration Register   800h*/
  __IO uint32_t DCTL;         /* dev Control Register         804h*/
  __IO uint32_t DSTS;         /* dev Status Register (RO)     808h*/
  uint32_t Reserved0C;           /* Reserved                     80Ch*/
  __IO uint32_t DIEPMSK;   /* dev IN Endpoint Mask         810h*/
  __IO uint32_t DOEPMSK;  /* dev OUT Endpoint Mask        814h*/
  __IO uint32_t DAINT;     /* dev All Endpoints Itr Reg    818h*/
  __IO uint32_t DAINTMSK; /* dev All Endpoints Itr Mask   81Ch*/
  uint32_t  Reserved20;          /* Reserved                     820h*/
  uint32_t Reserved9;       /* Reserved                     824h*/
  __IO uint32_t DVBUSDIS;    /* dev VBUS discharge Register  828h*/
  __IO uint32_t DVBUSPULSE;  /* dev VBUS Pulse Register      82Ch*/
  __IO uint32_t DTHRCTL;     /* dev thr                      830h*/
  __IO uint32_t DIEPEMPMSK; /* dev empty msk             834h*/
  __IO uint32_t DEACHINT;    /* dedicated EP interrupt       838h*/
  __IO uint32_t DEACHMSK;    /* dedicated EP msk             83Ch*/  
  uint32_t Reserved40;      /* dedicated EP mask           840h*/
  __IO uint32_t DINEP1MSK;  /* dedicated EP mask           844h*/
  uint32_t  Reserved44[15];      /* Reserved                 844-87Ch*/
  __IO uint32_t DOUTEP1MSK; /* dedicated EP msk            884h*/   
}
USB_OTG_DREGS;
/**
  * @}
  */


/** @defgroup __IN_Endpoint-Specific_Register
  * @{
  */
typedef struct _USB_OTG_INEPREGS
{
  __IO uint32_t DIEPCTL; /* dev IN Endpoint Control Reg 900h + (ep_num * 20h) + 00h*/
  uint32_t Reserved04;             /* Reserved                       900h + (ep_num * 20h) + 04h*/
  __IO uint32_t DIEPINT; /* dev IN Endpoint Itr Reg     900h + (ep_num * 20h) + 08h*/
  uint32_t Reserved0C;             /* Reserved                       900h + (ep_num * 20h) + 0Ch*/
  __IO uint32_t DIEPTSIZ; /* IN Endpoint Txfer Size   900h + (ep_num * 20h) + 10h*/
  __IO uint32_t DIEPDMA; /* IN Endpoint DMA Address Reg    900h + (ep_num * 20h) + 14h*/
  __IO uint32_t DTXFSTS;/*IN Endpoint Tx FIFO Status Reg 900h + (ep_num * 20h) + 18h*/
  uint32_t Reserved18;             /* Reserved  900h+(ep_num*20h)+1Ch-900h+ (ep_num * 20h) + 1Ch*/
}
USB_OTG_INEPREGS;
/**
  * @}
  */


/** @defgroup __OUT_Endpoint-Specific_Registers
  * @{
  */
typedef struct _USB_OTG_OUTEPREGS
{
  __IO uint32_t DOEPCTL;       /* dev OUT Endpoint Control Reg  B00h + (ep_num * 20h) + 00h*/
  uint32_t Reserved04;         /* Reserved                      B00h + (ep_num * 20h) + 04h*/
  __IO uint32_t DOEPINT;       /* dev OUT Endpoint Itr Reg      B00h + (ep_num * 20h) + 08h*/
  uint32_t Reserved0C;         /* Reserved                      B00h + (ep_num * 20h) + 0Ch*/
  __IO uint32_t DOEPTSIZ;      /* dev OUT Endpoint Txfer Size   B00h + (ep_num * 20h) + 10h*/
  __IO uint32_t DOEPDMA;       /* dev OUT Endpoint DMA Address  B00h + (ep_num * 20h) + 14h*/
  uint32_t Reserved18[2];      /* Reserved B00h + (ep_num * 20h) + 18h - B00h + (ep_num * 20h) + 1Ch*/
}
USB_OTG_OUTEPREGS;
/**
  * @}
  */


/** @defgroup __Host_Mode_Register_Structures
  * @{
  */
typedef struct _USB_OTG_HREGS
{
  __IO uint32_t HCFG;             /* Host Configuration Register    400h*/
  __IO uint32_t HFIR;      /* Host Frame Interval Register   404h*/
  __IO uint32_t HFNUM;         /* Host Frame Nbr/Frame Remaining 408h*/
  uint32_t Reserved40C;                   /* Reserved                       40Ch*/
  __IO uint32_t HPTXSTS;   /* Host Periodic Tx FIFO/ Queue Status 410h*/
  __IO uint32_t HAINT;   /* Host All Channels Interrupt Register 414h*/
  __IO uint32_t HAINTMSK;   /* Host All Channels Interrupt Mask 418h*/
}
USB_OTG_HREGS;
/**
  * @}
  */


/** @defgroup __Host_Channel_Specific_Registers
  * @{
  */
typedef struct _USB_OTG_HC_REGS
{
  __IO uint32_t HCCHAR;
  __IO uint32_t HCSPLT;
  __IO uint32_t HCINT;
  __IO uint32_t HCINTMSK;
  __IO uint32_t HCTSIZ;
  __IO uint32_t HCDMA;
  uint32_t Reserved[2];
}
USB_OTG_HC_REGS;
/**
  * @}
  */


/** @defgroup __otg_Core_registers
  * @{
  */
typedef struct USB_OTG_core_regs //000h
{
  USB_OTG_GREGS         *GREGS;
  USB_OTG_DREGS         *DREGS;
  USB_OTG_HREGS         *HREGS;
  USB_OTG_INEPREGS      *INEP_REGS[USB_OTG_MAX_TX_FIFOS];
  USB_OTG_OUTEPREGS     *OUTEP_REGS[USB_OTG_MAX_TX_FIFOS];
  USB_OTG_HC_REGS       *HC_REGS[USB_OTG_MAX_TX_FIFOS];
  __IO uint32_t         *HPRT0;
  __IO uint32_t         *DFIFO[USB_OTG_MAX_TX_FIFOS];
  __IO uint32_t         *PCGCCTL;
}
USB_OTG_CORE_REGS , *PUSB_OTG_CORE_REGS;
typedef union _USB_OTG_GOTGCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t sesreqscs :
    1;
uint32_t sesreq :
    1;
uint32_t Reserved2_7 :
    6;
uint32_t hstnegscs :
    1;
uint32_t hnpreq :
    1;
uint32_t hstsethnpen :
    1;
uint32_t devhnpen :
    1;
uint32_t Reserved12_15 :
    4;
uint32_t conidsts :
    1;
uint32_t dbct :
    1;
uint32_t asesvld :
    1;
uint32_t bsesvld :
    1;
uint32_t Reserved20_31 :
    12;
  }
  b;
} USB_OTG_GOTGCTL_TypeDef ;

typedef union _USB_OTG_GOTGINT_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t Reserved0_1 :
    2;
uint32_t sesenddet :
    1;
uint32_t Reserved3_7 :
    5;
uint32_t sesreqsucstschng :
    1;
uint32_t hstnegsucstschng :
    1;
uint32_t reserver10_16 :
    7;
uint32_t hstnegdet :
    1;
uint32_t adevtoutchng :
    1;
uint32_t debdone :
    1;
uint32_t Reserved31_20 :
    12;
  }
  b;
} USB_OTG_GOTGINT_TypeDef ;
typedef union _USB_OTG_GAHBCFG_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t glblintrmsk :
    1;
uint32_t hburstlen :
    4;
uint32_t dmaenable :
    1;
uint32_t Reserved :
    1;
uint32_t nptxfemplvl_txfemplvl :
    1;
uint32_t ptxfemplvl :
    1;
uint32_t Reserved9_31 :
    23;
  }
  b;
} USB_OTG_GAHBCFG_TypeDef ;
typedef union _USB_OTG_GUSBCFG_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t toutcal :
    3;
uint32_t Reserved3_5 :
    3;
uint32_t physel :
    1;
uint32_t Reserved7 :
    1;
uint32_t srpcap :
    1;
uint32_t hnpcap :
    1;
uint32_t usbtrdtim :
    4;
uint32_t Reserved14 :
    1;
uint32_t phylpwrclksel :
    1;
uint32_t Reserved16 :
    1;
uint32_t ulpi_fsls :
    1;
uint32_t ulpi_auto_res :
    1;
uint32_t ulpi_clk_sus_m :
    1;
uint32_t ulpi_ext_vbus_drv :
    1;
uint32_t ulpi_int_vbus_ind :
    1;
uint32_t term_sel_dl_pulse :
    1;
uint32_t ulpi_ind_cpl :
    1;
uint32_t ulpi_passthrough :
    1;       
uint32_t ulpi_protect_disable :
    1; 
uint32_t Reserved26_28 :
    3;     
uint32_t force_host :
    1;
uint32_t force_dev :
    1;
uint32_t corrupt_tx :
    1;
  }
  b;
} USB_OTG_GUSBCFG_TypeDef ;
typedef union _USB_OTG_GRSTCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t csftrst :
    1;
uint32_t hsftrst :
    1;
uint32_t hstfrm :
    1;
uint32_t Reserved3 :
    1;
uint32_t rxfflsh :
    1;
uint32_t txfflsh :
    1;
uint32_t txfnum :
    5;
uint32_t Reserved11_29 :
    19;
uint32_t dmareq :
    1;
uint32_t ahbidle :
    1;
  }
  b;
} USB_OTG_GRSTCTL_TypeDef ;
typedef union _USB_OTG_GINTMSK_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t Reserved0 :
    1;
uint32_t modemismatch :
    1;
uint32_t otgintr :
    1;
uint32_t sofintr :
    1;
uint32_t rxstsqlvl :
    1;
uint32_t nptxfempty :
    1;
uint32_t ginnakeff :
    1;
uint32_t goutnakeff :
    1;
uint32_t Reserved8_9 :
    2;
uint32_t erlysuspend :
    1;
uint32_t usbsuspend :
    1;
uint32_t usbreset :
    1;
uint32_t enumdone :
    1;
uint32_t isooutdrop :
    1;
uint32_t eopframe :
    1;
uint32_t Reserved16 :
    1;
uint32_t epmismatch :
    1;
uint32_t inepintr :
    1;
uint32_t outepintr :
    1;
uint32_t incomplisoin :
    1;
uint32_t incomplisoout :
    1;
uint32_t Reserved22_23 :
    2;
uint32_t portintr :
    1;
uint32_t hcintr :
    1;
uint32_t ptxfempty :
    1;
uint32_t Reserved27 :
    1;
uint32_t conidstschng :
    1;
uint32_t disconnect :
    1;
uint32_t sessreqintr :
    1;
uint32_t wkupintr :
    1;
  }
  b;
} USB_OTG_GINTMSK_TypeDef ;
typedef union _USB_OTG_GINTSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t curmode :
    1;
uint32_t modemismatch :
    1;
uint32_t otgintr :
    1;
uint32_t sofintr :
    1;
uint32_t rxstsqlvl :
    1;
uint32_t nptxfempty :
    1;
uint32_t ginnakeff :
    1;
uint32_t goutnakeff :
    1;
uint32_t Reserved8_9 :
    2;
uint32_t erlysuspend :
    1;
uint32_t usbsuspend :
    1;
uint32_t usbreset :
    1;
uint32_t enumdone :
    1;
uint32_t isooutdrop :
    1;
uint32_t eopframe :
    1;
uint32_t Reserved16_17 :
    2;
uint32_t inepint:
    1;
uint32_t outepintr :
    1;
uint32_t incomplisoin :
    1;
uint32_t incomplisoout :
    1;
uint32_t Reserved22_23 :
    2;
uint32_t portintr :
    1;
uint32_t hcintr :
    1;
uint32_t ptxfempty :
    1;
uint32_t Reserved27 :
    1;
uint32_t conidstschng :
    1;
uint32_t disconnect :
    1;
uint32_t sessreqintr :
    1;
uint32_t wkupintr :
    1;
  }
  b;
} USB_OTG_GINTSTS_TypeDef ;
typedef union _USB_OTG_DRXSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t epnum :
    4;
uint32_t bcnt :
    11;
uint32_t dpid :
    2;
uint32_t pktsts :
    4;
uint32_t fn :
    4;
uint32_t Reserved :
    7;
  }
  b;
} USB_OTG_DRXSTS_TypeDef ;
typedef union _USB_OTG_GRXSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t chnum :
    4;
uint32_t bcnt :
    11;
uint32_t dpid :
    2;
uint32_t pktsts :
    4;
uint32_t Reserved :
    11;
  }
  b;
} USB_OTG_GRXFSTS_TypeDef ;
typedef union _USB_OTG_FSIZ_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t startaddr :
    16;
uint32_t depth :
    16;
  }
  b;
} USB_OTG_FSIZ_TypeDef ;
typedef union _USB_OTG_HNPTXSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
    uint32_t nptxfspcavail :
      16;
    uint32_t nptxqspcavail :
      8;
      struct
        {
          uint32_t terminate :
            1;
          uint32_t token :
            2;
          uint32_t chnum :
            4; 
         } nptxqtop;
     uint32_t Reserved :
        1;
  }
  b;
} USB_OTG_HNPTXSTS_TypeDef ;
typedef union _USB_OTG_DTXFSTSn_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t txfspcavail :
    16;
uint32_t Reserved :
    16;
  }
  b;
} USB_OTG_DTXFSTSn_TypeDef ;

typedef union _USB_OTG_GCCFG_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t Reserved_in :
    16;
uint32_t pwdn :
    1;
uint32_t Reserved_17 :
    1;
uint32_t vbussensingA :
    1;
uint32_t vbussensingB :
    1;
uint32_t sofouten :
    1;
uint32_t disablevbussensing :
    1;
uint32_t Reserved_out :
    10;
  }
  b;
} USB_OTG_GCCFG_TypeDef ;

typedef union _USB_OTG_DCFG_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t devspd :
    2;
uint32_t nzstsouthshk :
    1;
uint32_t Reserved3 :
    1;
uint32_t devaddr :
    7;
uint32_t perfrint :
    2;
uint32_t Reserved12_31 :
    19;
  }
  b;
} USB_OTG_DCFG_TypeDef ;
typedef union _USB_OTG_DCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t rmtwkupsig :
    1;
uint32_t sftdiscon :
    1;
uint32_t gnpinnaksts :
    1;
uint32_t goutnaksts :
    1;
uint32_t tstctl :
    3;
uint32_t sgnpinnak :
    1;
uint32_t cgnpinnak :
    1;
uint32_t sgoutnak :
    1;
uint32_t cgoutnak :
    1;
uint32_t poprg_done :
    1;    
uint32_t Reserved :
    20;
  }
  b;
} USB_OTG_DCTL_TypeDef ;
typedef union _USB_OTG_DSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t suspsts :
    1;
uint32_t enumspd :
    2;
uint32_t errticerr :
    1;
uint32_t Reserved4_7:
    4;
uint32_t soffn :
    14;
uint32_t Reserved22_31 :
    10;
  }
  b;
} USB_OTG_DSTS_TypeDef ;
typedef union _USB_OTG_DIEPINTn_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfercompl :
    1;
uint32_t epdisabled :
    1;
uint32_t Reserved2 :
    1;
uint32_t timeout :
    1;
uint32_t intktxfemp :
    1;
uint32_t Reserved5 :
    1;
uint32_t inepnakeff :
    1;
uint32_t emptyintr :
    1;
uint32_t txfifoundrn :
    1;
uint32_t Reserved14_31 :
    23;
  }
  b;
} USB_OTG_DIEPINTn_TypeDef ;
typedef union _USB_OTG_DIEPINTn_TypeDef   USB_OTG_DIEPMSK_TypeDef ;
typedef union _USB_OTG_DOEPINTn_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfercompl :
    1;
uint32_t epdisabled :
    1;
uint32_t Reserved2 :
    1;
uint32_t setup :
    1;
uint32_t Reserved04_31 :
    28;
  }
  b;
} USB_OTG_DOEPINTn_TypeDef ;
typedef union _USB_OTG_DOEPINTn_TypeDef   USB_OTG_DOEPMSK_TypeDef ;

typedef union _USB_OTG_DAINT_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t in :
    16;
uint32_t out :
    16;
  }
  ep;
} USB_OTG_DAINT_TypeDef ;

typedef union _USB_OTG_DTHRCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t non_iso_thr_en :
    1;
uint32_t iso_thr_en :
    1;
uint32_t tx_thr_len :
    9;
uint32_t Reserved11_15 :
    5;
uint32_t rx_thr_en :
    1;
uint32_t rx_thr_len :
    9;
uint32_t Reserved26 : 
    1;
uint32_t arp_en :  
    1;
uint32_t Reserved28_31 :
    4;   
  }
  b;
} USB_OTG_DTHRCTL_TypeDef ;
typedef union _USB_OTG_DEPCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t mps :
    11;
uint32_t reserved :
    4;
uint32_t usbactep :
    1;
uint32_t dpid :
    1;
uint32_t naksts :
    1;
uint32_t eptype :
    2;
uint32_t snp :
    1;
uint32_t stall :
    1;
uint32_t txfnum :
    4;
uint32_t cnak :
    1;
uint32_t snak :
    1;
uint32_t setd0pid :
    1;
uint32_t setd1pid :
    1;
uint32_t epdis :
    1;
uint32_t epena :
    1;
  }
  b;
} USB_OTG_DEPCTL_TypeDef ;
typedef union _USB_OTG_DEPXFRSIZ_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfersize :
    19;
uint32_t pktcnt :
    10;
uint32_t mc :
    2;
uint32_t Reserved :
    1;
  }
  b;
} USB_OTG_DEPXFRSIZ_TypeDef ;
typedef union _USB_OTG_DEP0XFRSIZ_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfersize :
    7;
uint32_t Reserved7_18 :
    12;
uint32_t pktcnt :
    2;
uint32_t Reserved20_28 :
    9;
uint32_t supcnt :
    2;
    uint32_t Reserved31;
  }
  b;
} USB_OTG_DEP0XFRSIZ_TypeDef ;
typedef union _USB_OTG_HCFG_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t fslspclksel :
    2;
uint32_t fslssupp :
    1;
  }
  b;
} USB_OTG_HCFG_TypeDef ;
typedef union _USB_OTG_HFRMINTRVL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t frint :
    16;
uint32_t Reserved :
    16;
  }
  b;
} USB_OTG_HFRMINTRVL_TypeDef ;

typedef union _USB_OTG_HFNUM_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t frnum :
    16;
uint32_t frrem :
    16;
  }
  b;
} USB_OTG_HFNUM_TypeDef ;
typedef union _USB_OTG_HPTXSTS_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t ptxfspcavail :
    16;
uint32_t ptxqspcavail :
    8;
      struct
        {
          uint32_t terminate :
            1;
          uint32_t token :
            2;
          uint32_t chnum :
            4; 
          uint32_t odd_even :
            1;            
         } ptxqtop;    
  }
  b;
} USB_OTG_HPTXSTS_TypeDef ;
typedef union _USB_OTG_HPRT0_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t prtconnsts :
    1;
uint32_t prtconndet :
    1;
uint32_t prtena :
    1;
uint32_t prtenchng :
    1;
uint32_t prtovrcurract :
    1;
uint32_t prtovrcurrchng :
    1;
uint32_t prtres :
    1;
uint32_t prtsusp :
    1;
uint32_t prtrst :
    1;
uint32_t Reserved9 :
    1;
uint32_t prtlnsts :
    2;
uint32_t prtpwr :
    1;
uint32_t prttstctl :
    4;
uint32_t prtspd :
    2;
uint32_t Reserved19_31 :
    13;
  }
  b;
} USB_OTG_HPRT0_TypeDef ;
typedef union _USB_OTG_HAINT_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t chint :
    16;
uint32_t Reserved :
    16;
  }
  b;
} USB_OTG_HAINT_TypeDef ;
typedef union _USB_OTG_HAINTMSK_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t chint :
    16;
uint32_t Reserved :
    16;
  }
  b;
} USB_OTG_HAINTMSK_TypeDef ;
typedef union _USB_OTG_HCCHAR_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t mps :
    11;
uint32_t epnum :
    4;
uint32_t epdir :
    1;
uint32_t Reserved :
    1;
uint32_t lspddev :
    1;
uint32_t eptype :
    2;
uint32_t multicnt :
    2;
uint32_t devaddr :
    7;
uint32_t oddfrm :
    1;
uint32_t chdis :
    1;
uint32_t chen :
    1;
  }
  b;
} USB_OTG_HCCHAR_TypeDef ;
typedef union _USB_OTG_HCSPLT_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t prtaddr :
    7;
uint32_t hubaddr :
    7;
uint32_t xactpos :
    2;
uint32_t compsplt :
    1;
uint32_t Reserved :
    14;
uint32_t spltena :
    1;
  }
  b;
} USB_OTG_HCSPLT_TypeDef ;
typedef union _USB_OTG_HCINTn_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfercompl :
    1;
uint32_t chhltd :
    1;
uint32_t ahberr :
    1;
uint32_t stall :
    1;
uint32_t nak :
    1;
uint32_t ack :
    1;
uint32_t nyet :
    1;
uint32_t xacterr :
    1;
uint32_t bblerr :
    1;
uint32_t frmovrun :
    1;
uint32_t datatglerr :
    1;
uint32_t Reserved :
    21;
  }
  b;
} USB_OTG_HCINTn_TypeDef ;
typedef union _USB_OTG_HCTSIZn_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfersize :
    19;
uint32_t pktcnt :
    10;
uint32_t pid :
    2;
uint32_t dopng :
    1;
  }
  b;
} USB_OTG_HCTSIZn_TypeDef ;
typedef union _USB_OTG_HCINTMSK_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t xfercompl :
    1;
uint32_t chhltd :
    1;
uint32_t ahberr :
    1;
uint32_t stall :
    1;
uint32_t nak :
    1;
uint32_t ack :
    1;
uint32_t nyet :
    1;
uint32_t xacterr :
    1;
uint32_t bblerr :
    1;
uint32_t frmovrun :
    1;
uint32_t datatglerr :
    1;
uint32_t Reserved :
    21;
  }
  b;
} USB_OTG_HCINTMSK_TypeDef ;

typedef union _USB_OTG_PCGCCTL_TypeDef 
{
  uint32_t d32;
  struct
  {
uint32_t stoppclk :
    1;
uint32_t gatehclk :
    1;
uint32_t Reserved2_3 :
    2;
uint32_t phy_susp :
    1;    
uint32_t Reserved5_31 :
    27;
  }
  b;
} USB_OTG_PCGCCTL_TypeDef ;

/**
  * @}
  */ 


/** @defgroup USB_REGS_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 


#endif //__USB_OTG_REGS_H__


/**
  * @}
  */ 

/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


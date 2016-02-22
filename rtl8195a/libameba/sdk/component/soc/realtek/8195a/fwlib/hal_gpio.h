/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

#define HAL_GPIO_PIN_INT_MODE       0x80

typedef enum {
    _PORT_A = 0,
    _PORT_B = 1,
    _PORT_C = 2,
    _PORT_D = 3,
    _PORT_E = 4,
    _PORT_F = 5,
    _PORT_G = 6,
    _PORT_H = 7,
    _PORT_I = 8,
    _PORT_J = 9,
    _PORT_K = 10,

    _PORT_MAX
} HAL_GPIO_PORT_NAME;

typedef enum {
    _PA_0  = (_PORT_A<<4|0),
    _PA_1  = (_PORT_A<<4|1),
    _PA_2  = (_PORT_A<<4|2),
    _PA_3  = (_PORT_A<<4|3),
    _PA_4  = (_PORT_A<<4|4),
    _PA_5  = (_PORT_A<<4|5),
    _PA_6  = (_PORT_A<<4|6),
    _PA_7  = (_PORT_A<<4|7),

    _PB_0  = (_PORT_B<<4|0),
    _PB_1  = (_PORT_B<<4|1),
    _PB_2  = (_PORT_B<<4|2),
    _PB_3  = (_PORT_B<<4|3),
    _PB_4  = (_PORT_B<<4|4),
    _PB_5  = (_PORT_B<<4|5),
    _PB_6  = (_PORT_B<<4|6),
    _PB_7  = (_PORT_B<<4|7),

    _PC_0  = (_PORT_C<<4|0),
    _PC_1  = (_PORT_C<<4|1),
    _PC_2  = (_PORT_C<<4|2),
    _PC_3  = (_PORT_C<<4|3),
    _PC_4  = (_PORT_C<<4|4),
    _PC_5  = (_PORT_C<<4|5),
    _PC_6  = (_PORT_C<<4|6),
    _PC_7  = (_PORT_C<<4|7),
    _PC_8  = (_PORT_C<<4|8),
    _PC_9  = (_PORT_C<<4|9),

    _PD_0  = (_PORT_D<<4|0),
    _PD_1  = (_PORT_D<<4|1),
    _PD_2  = (_PORT_D<<4|2),
    _PD_3  = (_PORT_D<<4|3),
    _PD_4  = (_PORT_D<<4|4),
    _PD_5  = (_PORT_D<<4|5),
    _PD_6  = (_PORT_D<<4|6),
    _PD_7  = (_PORT_D<<4|7),
    _PD_8  = (_PORT_D<<4|8),
    _PD_9  = (_PORT_D<<4|9),
  
    _PE_0  = (_PORT_E<<4|0),
    _PE_1  = (_PORT_E<<4|1),
    _PE_2  = (_PORT_E<<4|2),
    _PE_3  = (_PORT_E<<4|3),
    _PE_4  = (_PORT_E<<4|4),
    _PE_5  = (_PORT_E<<4|5),
    _PE_6  = (_PORT_E<<4|6),
    _PE_7  = (_PORT_E<<4|7),
    _PE_8  = (_PORT_E<<4|8),
    _PE_9  = (_PORT_E<<4|9),
    _PE_A  = (_PORT_E<<4|10),
  
    _PF_0  = (_PORT_F<<4|0),
    _PF_1  = (_PORT_F<<4|1),
    _PF_2  = (_PORT_F<<4|2),
    _PF_3  = (_PORT_F<<4|3),
    _PF_4  = (_PORT_F<<4|4),
    _PF_5  = (_PORT_F<<4|5),
//    _PF_6  = (_PORT_F<<4|6),
//    _PF_7  = (_PORT_F<<4|7),

    _PG_0  = (_PORT_G<<4|0),
    _PG_1  = (_PORT_G<<4|1),
    _PG_2  = (_PORT_G<<4|2),
    _PG_3  = (_PORT_G<<4|3),
    _PG_4  = (_PORT_G<<4|4),
    _PG_5  = (_PORT_G<<4|5),
    _PG_6  = (_PORT_G<<4|6),
    _PG_7  = (_PORT_G<<4|7),

    _PH_0  = (_PORT_H<<4|0),
    _PH_1  = (_PORT_H<<4|1),
    _PH_2  = (_PORT_H<<4|2),
    _PH_3  = (_PORT_H<<4|3),
    _PH_4  = (_PORT_H<<4|4),
    _PH_5  = (_PORT_H<<4|5),
    _PH_6  = (_PORT_H<<4|6),
    _PH_7  = (_PORT_H<<4|7),

    _PI_0  = (_PORT_I<<4|0),
    _PI_1  = (_PORT_I<<4|1),
    _PI_2  = (_PORT_I<<4|2),
    _PI_3  = (_PORT_I<<4|3),
    _PI_4  = (_PORT_I<<4|4),
    _PI_5  = (_PORT_I<<4|5),
    _PI_6  = (_PORT_I<<4|6),
    _PI_7  = (_PORT_I<<4|7),

    _PJ_0  = (_PORT_J<<4|0),
    _PJ_1  = (_PORT_J<<4|1),
    _PJ_2  = (_PORT_J<<4|2),
    _PJ_3  = (_PORT_J<<4|3),
    _PJ_4  = (_PORT_J<<4|4),
    _PJ_5  = (_PORT_J<<4|5),
    _PJ_6  = (_PORT_J<<4|6),
//    _PJ_7  = (_PORT_J<<4|7),

    _PK_0  = (_PORT_K<<4|0),
    _PK_1  = (_PORT_K<<4|1),
    _PK_2  = (_PORT_K<<4|2),
    _PK_3  = (_PORT_K<<4|3),
    _PK_4  = (_PORT_K<<4|4),
    _PK_5  = (_PORT_K<<4|5),
    _PK_6  = (_PORT_K<<4|6),
//    _PK_7  = (_PORT_K<<4|7),

    // Not connected
    _PIN_NC = (int)0xFFFFFFFF
} HAL_PIN_NAME;

typedef enum
{
  GPIO_PIN_LOW  = 0,
  GPIO_PIN_HIGH = 1,
  GPIO_PIN_ERR  = 2     // read Pin error
} HAL_GPIO_PIN_STATE;

typedef enum {
    DIN_PULL_NONE   = 0,    //floating or high impedance ?
    DIN_PULL_LOW    = 1,
    DIN_PULL_HIGH   = 2,

    DOUT_PUSH_PULL  = 3,
    DOUT_OPEN_DRAIN = 4,

    INT_LOW         = (5|HAL_GPIO_PIN_INT_MODE),    // Interrupt Low level trigger
    INT_HIGH        = (6|HAL_GPIO_PIN_INT_MODE),    // Interrupt High level trigger
    INT_FALLING     = (7|HAL_GPIO_PIN_INT_MODE),    // Interrupt Falling edge trigger
    INT_RISING      = (8|HAL_GPIO_PIN_INT_MODE)     // Interrupt Rising edge trigger        
} HAL_GPIO_PIN_MODE;

enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2,
    GPIO_PORT_D = 3
};

typedef enum {
    hal_PullNone  = 0,
    hal_PullUp    = 1,
    hal_PullDown  = 2,
    hal_OpenDrain = 3,
    hal_PullDefault = hal_PullNone
} HAL_PinMode;

typedef struct _HAL_GPIO_PORT_ {
    u32 out_data;       // to write the GPIO port
    u32 in_data;        // to read the GPIO port
    u32 dir;            // config each pin direction    
}HAL_GPIO_PORT, *PHAL_GPIO_PORT;

#define HAL_GPIO_PIN_NAME(port,pin)         (((port)<<5)|(pin))
#define HAL_GPIO_GET_PORT_BY_NAME(x)        ((x>>5) & 0x03)
#define HAL_GPIO_GET_PIN_BY_NAME(x)         (x & 0x1f)

typedef struct _HAL_GPIO_PIN_ {
    HAL_GPIO_PIN_MODE pin_mode;
    u32 pin_name;    // Pin: [7:5]: port number, [4:0]: pin number
}HAL_GPIO_PIN, *PHAL_GPIO_PIN;

typedef struct _HAL_GPIO_OP_ {
#if defined(__ICCARM__)
    void* dummy;
#endif
}HAL_GPIO_OP, *PHAL_GPIO_OP;

typedef void (*GPIO_IRQ_FUN)(VOID *Data, u32 Id);
typedef void (*GPIO_USER_IRQ_FUN)(u32 Id);

typedef struct _HAL_GPIO_ADAPTER_ {
    IRQ_HANDLE IrqHandle;   // GPIO HAL IRQ Handle
    GPIO_USER_IRQ_FUN UserIrqHandler;   // GPIO IRQ Handler
    GPIO_IRQ_FUN PortA_IrqHandler[32]; // The interrupt handler triggered by Port A[x]
    VOID *PortA_IrqData[32];
    VOID (*EnterCritical)(void);
    VOID (*ExitCritical)(void);
    u32 Local_Gpio_Dir[3];  // to record direction setting: 0- IN, 1- Out
    u8 Gpio_Func_En;    // Is GPIO HW function enabled ?
    u8 Locked;
}HAL_GPIO_ADAPTER, *PHAL_GPIO_ADAPTER;

u32 
HAL_GPIO_GetPinName(
    u32 chip_pin
);

VOID 
HAL_GPIO_PullCtrl(
    u32 pin,
    u32 mode    
);

VOID 
HAL_GPIO_Init(
    HAL_GPIO_PIN  *GPIO_Pin
);

VOID 
HAL_GPIO_Irq_Init(
    HAL_GPIO_PIN  *GPIO_Pin
);

#endif  // end of "#define _HAL_GPIO_H_"


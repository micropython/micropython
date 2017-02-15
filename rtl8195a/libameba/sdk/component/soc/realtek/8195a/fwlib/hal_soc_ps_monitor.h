#ifndef _HAL_SOCPWR_
#define _HAL_SOCPWR_



#define MAX_BACKUP_SIZE 129
#define MAXFUNC     10
#define FSTREG      0xFF

#define REG_VDR_ANACK_CAL_CTRL 0xA0

#define PS_MASK 0xFFFFFFFF

//pwr state
#define HWACT    0
#define HWCG     1
#define HWINACT  2
#define UNDEF    3
#define ALLMET   0xff

//SLP
#define     SLP_STIMER    BIT0
#define     SLP_GTIMER    BIT1
#define     SLP_GPIO      BIT2
#define     SLP_WL        BIT3
#define     SLP_NFC       BIT4
#define     SLP_SDIO      BIT5
#define     SLP_USB       BIT6
#define     SLP_TIMER33   BIT7

//DSTBY
#define     DSTBY_STIMER  BIT0
#define     DSTBY_NFC     BIT1
#define     DSTBY_TIMER33 BIT2
#define     DSTBY_GPIO    BIT3

//DS wake event
#define DS_TIMER33 BIT0
#define DS_GPIO    BIT1

enum power_state_idx{
    ACT    = 0,
    WFE    = 1,
    WFI    = 2,
    SNOOZE = 3,
    SLPCG  = 4,
    SLPPG  = 5,
    DSTBY  = 6,
    DSLP   = 7,
    INACT  = 8,
    MAXSTATE = 9
};

enum clk_idx{
    ANACK = 0,
    A33CK = 1,
};


typedef struct _power_state_{
    u8  FuncIdx;
    u8  PowerState;
}POWER_STATE, *pPOWER_STATE;

typedef struct _reg_power_state_{
    u8  FuncIdx;
    u8  PwrState;
}REG_POWER_STATE, *pPREG_POWER_STATE;

#if 0
typedef struct _power_state_{
    u8  FuncIdx;
    u8  PowerState;
    u32 ReqDuration;
    u32 RegCount;
    u32 RemainDuration;
}POWER_STATE, *pPOWER_STATE;

typedef struct _reg_power_state_{
    u8  FuncIdx;
    u8  PwrState;
    u32 ReqDuration;
    //u8 StateIdx;
}REG_POWER_STATE, *pPREG_POWER_STATE;
#endif

typedef struct _power_mgn_{
    u8          ActFuncCount;
    POWER_STATE PwrState[MAXFUNC];
    u8          CurrentState;
    u8          SDREn;
    u32         MSPbackup[MAX_BACKUP_SIZE];
    u32         CPURegbackup[25];
    u32         CPUPSP;
    u32         WakeEventFlag;
    BOOL        SleepFlag;
    //u32         CPUReg[13];
    //u32         MSBackUp[128];
}Power_Mgn, *pPower_Mgn;

typedef struct _SYS_ADAPTER_ {
    u8      function;
}SYS_ADAPTER, *PSYS_ADAPTER;

extern Power_Mgn PwrAdapter;

u8 ChangeSoCPwrState(
    IN  u8  RequestState,
    IN  u32 ReqCount
);

VOID PrintCPU(VOID);
void WakeFromSLPPG(void);
VOID SOCPSTestApp(VOID *Data);


__inline static VOID 
CPURegBackUp(
    VOID
)
{
#if defined (__ICCARM__)
    // TODO: IAR has different way using assembly
#elif defined (__GNUC__)
	//backup cpu reg
    #if 0
    asm volatile
    (
        "PUSH {PSR, PC, LR, R12,R3,R2,R1,R0}\n"
    );
    #endif
    #if 0
    asm volatile
    (
        "PUSH {r0,r1,r2,r3,r4}\n"
    );
    #endif
    
    asm volatile
    (

        "MOV %0, r0\n"
        :"=r"(PwrAdapter.CPURegbackup[0])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r1\n"
        :"=r"(PwrAdapter.CPURegbackup[1])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r2\n"
        :"=r"(PwrAdapter.CPURegbackup[2])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r3\n"
        :"=r"(PwrAdapter.CPURegbackup[3])
        ::"memory"
    );
    
    asm volatile
    (
        "MOV %0, r4\n"
        :"=r"(PwrAdapter.CPURegbackup[4])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r5\n"
        :"=r"(PwrAdapter.CPURegbackup[5])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r6\n"
        :"=r"(PwrAdapter.CPURegbackup[6])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r7\n"
        :"=r"(PwrAdapter.CPURegbackup[7])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r8\n"
        :"=r"(PwrAdapter.CPURegbackup[8])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r9\n"
        :"=r"(PwrAdapter.CPURegbackup[9])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r10\n"
        :"=r"(PwrAdapter.CPURegbackup[10])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r11\n"
        :"=r"(PwrAdapter.CPURegbackup[11])
        ::"memory"
    );
    asm volatile
    (
        "MOV %0, r12\n"
        :"=r"(PwrAdapter.CPURegbackup[12])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r13\n"
        :"=r"(PwrAdapter.CPURegbackup[13])
        ::"memory"
    );
    asm volatile
    (
        //"MOV %0, r14\n"
        "LDR %0, =SLPPG_WAKEUP_POINT\n"
        "ADD %0, #1\n"
        :"=r"(PwrAdapter.CPURegbackup[14])
        ::"memory"
    );
    asm volatile
    (
        "LDR %0, =SLPPG_WAKEUP_POINT\n"
        "ADD %0, #1\n"
        :"=r"(PwrAdapter.CPURegbackup[15])
        ::"memory"
    );
    asm volatile
    (
        "MRS %0, PSR\n"
        :"=r"(PwrAdapter.CPURegbackup[16])
        ::"memory"
    );

#if 1
    asm volatile
    (
        "mov %0, r13\n"
        "MOV %1, PC\n" 
        "MRS %2, CONTROL\n"
        "MRS %3, PSP\n" 
        "MRS %4, MSP\n" 
        :"=r"(PwrAdapter.CPURegbackup[24]),"=r"(PwrAdapter.CPURegbackup[23]),"=r"(PwrAdapter.CPURegbackup[22]),"=r"(PwrAdapter.CPURegbackup[21]),"=r"(PwrAdapter.CPURegbackup[20])
        ::"memory"
    );
#endif
    #ifdef CONFIG_SOC_PS_VERIFY
    PrintCPU();
    #endif  //#ifdef CONFIG_SOC_PS_VERIFY
#endif //#elif defined (__GNUC__)
}

VOID RegPowerState(REG_POWER_STATE RegPwrState);

#endif  //_HAL_SOCPWR_

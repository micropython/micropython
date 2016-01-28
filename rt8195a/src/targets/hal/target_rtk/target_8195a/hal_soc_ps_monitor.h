#ifndef _HAL_SOCPWR_
#define _HAL_SOCPWR_



#define MAX_BACKUP_SIZE 129
#define MAXFUNC     10
#define FSTREG      0xFF

#define REG_VDR_ANACK_CAL_CTRL 0xA0

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

typedef struct _power_mgn_{
    u8          ActFuncCount;
    POWER_STATE PwrState[MAXFUNC];
    u8          CurrentState;
    //u32         CPUReg[13];
    //u32         MSBackUp[128];
}Power_Mgn, *pPower_Mgn;

typedef struct _SYS_ADAPTER_ {
    u8      function;
}SYS_ADAPTER, *PSYS_ADAPTER;

extern u32 CPURegbackup[];
extern u32 CPUPSP;


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
        :"=r"(CPURegbackup[0])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r1\n"
        :"=r"(CPURegbackup[1])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r2\n"
        :"=r"(CPURegbackup[2])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r3\n"
        :"=r"(CPURegbackup[3])
        ::"memory"
    );
    
    asm volatile
    (
        "MOV %0, r4\n"
        :"=r"(CPURegbackup[4])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r5\n"
        :"=r"(CPURegbackup[5])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r6\n"
        :"=r"(CPURegbackup[6])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r7\n"
        :"=r"(CPURegbackup[7])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r8\n"
        :"=r"(CPURegbackup[8])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r9\n"
        :"=r"(CPURegbackup[9])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r10\n"
        :"=r"(CPURegbackup[10])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r11\n"
        :"=r"(CPURegbackup[11])
        ::"memory"
    );
    asm volatile
    (
        "MOV %0, r12\n"
        :"=r"(CPURegbackup[12])
        ::"memory"
    );

    asm volatile
    (
        "MOV %0, r13\n"
        :"=r"(CPURegbackup[13])
        ::"memory"
    );
    asm volatile
    (
        //"MOV %0, r14\n"
        "LDR %0, =SLPPG_WAKEUP_POINT\n"
        "ADD %0, #1\n"
        :"=r"(CPURegbackup[14])
        ::"memory"
    );
    asm volatile
    (
        "LDR %0, =SLPPG_WAKEUP_POINT\n"
        "ADD %0, #1\n"
        :"=r"(CPURegbackup[15])
        ::"memory"
    );
    asm volatile
    (
        "MRS %0, PSR\n"
        :"=r"(CPURegbackup[16])
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
        :"=r"(CPURegbackup[24]),"=r"(CPURegbackup[23]),"=r"(CPURegbackup[22]),"=r"(CPURegbackup[21]),"=r"(CPURegbackup[20])
        ::"memory"
    );
#endif
    #ifdef CONFIG_SOC_PS_VERIFY
    PrintCPU();
    #endif  //#ifdef CONFIG_SOC_PS_VERIFY
}

#endif  //_HAL_SOCPWR_

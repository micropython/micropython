#include "beep.h"

#if MICROPY_HW_HAS_LCD

static bool is_on[3];


static uint16_t BEL1_CNT_DWN;        // akt. Count-Down in dieser Phase
static uint8_t BEL1_PHASE;          // 1, 2, 3: = 1.Pause, Beep, 2.Pause
static uint8_t BEL1_OUTPUT;         // 0=inactive, 55=LOW, AA=HIGH
// units of 20 msec
static uint16_t RAM1_BEL1_PARAMS[3] = {
    6,    // _DDD2_BEL1_PAR1: Dauer 1. Pause
    4,    // _DDD2_BEL1_PAR2: Dauer der ON-Phase
    6     // _DDD2_BEL1_PAR3: Dauer 2. Pause
};


static uint8_t BEL2_PHASE;          // 1, 2: Beep, Pause
static uint16_t BEL2_CNT_DWN;        // akt. Count-Down in dieser Phase (Dauer)
static uint16_t BEL2_CNT_DWN2;       // akt. Count-Down: Anzahl Pulse
static uint8_t BEL2_OUTPUT;         // 0=inactive, 55=LOW, AA=HIGH
//
static uint16_t RAM1_BEL2_PAR1 = 3;
static uint16_t RAM1_BEL2_PAR2 = 2;
static uint16_t RAM1_BEL2_PAR3 = 5;


static uint8_t BEL3_PHASE;          // 1, 2, 3: Beep, 1.Pause, 2.Pause
static uint16_t BEL3_CNT_DWN;        // akt. Count-Down in dieser Phase (Dauer)
static uint16_t BEL3_CNT_DWN2;       // akt. Count-Down Anz Pulse in Gruppe
static uint16_t BEL3_CNT_DWN3;       // akt. Count-Down Anz Puls-Gruppen
static uint8_t BEL3_OUTPUT;         // 0=inactive, 55=LOW, AA=HIGH
//
static uint16_t RAM1_BEL3_PAR1 = 5;  // Dauer: ON
static uint16_t RAM1_BEL3_PAR2 = 5;  // Dauer: PAUSE
static uint16_t RAM1_BEL3_PAR3 = 4;  // Anzahl Pulse in Gruppe
static uint16_t RAM1_BEL3_PAR4 = 100;// PAUSE zwischen Gruppen
static uint16_t RAM1_BEL3_PAR5 = 6;  // Anz. Gruppen (nn, unendl)

void beep_init(void)
{
	is_on[BEEP_CLICK] = false;
    is_on[BEEP_BEEP] = false;
    is_on[BEEP_ALARM] = false;
}

void beep_on(uint8_t type)
{
	switch(type)
    {
        case BEEP_CLICK:
        {
            BEL1_CNT_DWN = RAM1_BEL1_PARAMS[0];
            BEL1_PHASE = 1;
            BEL1_OUTPUT = OUT_LOW;
            break;
        }
        case BEEP_BEEP:
        {
            BEL2_CNT_DWN = RAM1_BEL2_PAR1;
            BEL2_CNT_DWN2 = RAM1_BEL2_PAR3;
            BEL2_PHASE = 1;
            BEL2_OUTPUT = OUT_HIGH;
            break;
        }
        case BEEP_ALARM:
        {
            BEL3_CNT_DWN = RAM1_BEL3_PAR1;
            BEL3_CNT_DWN2 = RAM1_BEL3_PAR3;
            BEL3_CNT_DWN3 = RAM1_BEL3_PAR5;
            BEL3_PHASE = 1;
            BEL3_OUTPUT = OUT_HIGH;
            break;
        }
        default:
        {
            return;
        }
    }

    is_on[type] = true;
}
void beep_off(uint8_t type)
{
    switch(type)
    {
        case BEEP_CLICK:
        case BEEP_BEEP:
        case BEEP_ALARM:
        {
            is_on[type] = false;
        }
    }
}

void beep_set_params
(
    uint16_t* params
)
{
    RAM1_BEL1_PARAMS[0] = params[0];
    RAM1_BEL1_PARAMS[1] = params[1];
    RAM1_BEL1_PARAMS[2] = params[2];

    RAM1_BEL2_PAR1 = params[3];
    RAM1_BEL2_PAR2 = params[4];
    RAM1_BEL2_PAR3 = params[5];

    RAM1_BEL3_PAR1 = params[6];
    RAM1_BEL3_PAR2 = params[7];
    RAM1_BEL3_PAR3 = params[8];
    RAM1_BEL3_PAR4 = params[9];
    RAM1_BEL3_PAR5 = params[10];
}


uint8_t beep_process()
{    
	if(is_on[BEEP_ALARM])
    {
        if(BEL3_CNT_DWN==0xffff || --BEL3_CNT_DWN != 0)
        {
            return BEL3_OUTPUT;
        }

        switch(BEL3_PHASE)
        {
            case 1:
            {
                BEL3_PHASE = 2;
                BEL3_CNT_DWN = RAM1_BEL3_PAR2;
                BEL3_OUTPUT = OUT_LOW;
                return BEL3_OUTPUT;
            }
            case 2:
            {
                if(BEL3_CNT_DWN2==0xffff || --BEL3_CNT_DWN2 != 0)
                {
                    BEL3_PHASE = 1;
                    BEL3_CNT_DWN = RAM1_BEL3_PAR1;
                    BEL3_OUTPUT = OUT_HIGH;
                    return BEL3_OUTPUT;
                }

                BEL3_PHASE = 3;
                if(BEL3_CNT_DWN3 == 1)
                {
                    is_on[BEEP_ALARM] = false;
                    return OUT_INACT;
                }

                BEL3_CNT_DWN = RAM1_BEL3_PAR4;
                BEL3_PHASE = 3;
                BEL3_OUTPUT = OUT_LOW;
                return BEL3_OUTPUT;
            }
            case 3:
            {
                if(BEL3_CNT_DWN3==0xffff || --BEL3_CNT_DWN3 != 0)
                {
                    BEL3_CNT_DWN = RAM1_BEL3_PAR1;
                    BEL3_CNT_DWN2 = RAM1_BEL3_PAR3;
                    BEL3_PHASE = 1;
                    BEL3_OUTPUT = OUT_HIGH;
                    return BEL3_OUTPUT;
                }

                is_on[BEEP_ALARM] = false;
                return OUT_INACT;
            }
        }
    }
    else if(is_on[BEEP_BEEP])
    {
        if(BEL2_CNT_DWN == 0xffff || --BEL2_CNT_DWN != 0)
        {
            return BEL2_OUTPUT;
        }

        // next phase
        if(BEL2_PHASE == 1)
        {
            BEL2_PHASE = 2;
            BEL2_CNT_DWN = RAM1_BEL2_PAR2;
            BEL2_OUTPUT = OUT_LOW;
            return BEL2_OUTPUT;
        }
        else
        {
            if(BEL2_CNT_DWN2 == 0xffff || --BEL2_CNT_DWN2 != 0)
            {
                BEL2_PHASE = 1;
                BEL2_CNT_DWN = RAM1_BEL2_PAR1;
                BEL2_OUTPUT = OUT_HIGH;
                return BEL2_OUTPUT;
            }

            is_on[BEEP_BEEP] = false;
            return OUT_INACT;
        }
    }
    else if(is_on[BEEP_CLICK])
    {
        if(--BEL1_CNT_DWN != 0)
        {
            return BEL1_OUTPUT;
        }

        // next phase
        if(++BEL1_PHASE == 4)
        {
            is_on[BEEP_CLICK] = false;
            return OUT_INACT;
        }

        BEL1_OUTPUT = ~BEL1_OUTPUT;
        BEL1_CNT_DWN = RAM1_BEL1_PARAMS[BEL1_PHASE - 1];
        return BEL1_OUTPUT;
    }

  return OUT_INACT;

}
#endif

#include "py/runtime.h"


#if MICROPY_HW_HAS_LCD

#include <string.h>

#include "kbd.h"
#include "beep.h"
#include "td01-lcd.h"
#include "ti-2def2.h"
#include "timer.h"

#define MASK LEN

bool RAM1_KEY_CLICK = false;

extern struct fifo_descriptor o_buf;
extern struct fifo_descriptor kbd_buf;

uint8_t RAM1_KBS_ATAB[] = 
{
    0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7
};

static uint8_t RAM1_KEY_CODES[128];

static uint8_t RAM1_KEY_SCODES[128];

#define NS	0x20    // 0010 0000B	; No-SHIFT-Code: diese Taste generiert
#define NS_BIT	5	//		keinen Code, wenn zusammen
			//		mit SHIFT-Funktion gedr.

#define NC	0x10    // 00010000B	; No-CTRL-Code: diese Taste generiert
#define NC_BIT	4	//		keinen Code, wenn zusammen
			//		mit CTRL-Taste gedrueckt !

#define NB	0x08    // 00001000B	; No-Buff-Code: diese Taste generiert
#define NB_BIT	3	//		keinen Tasten-Code im
			//		Tastatur-Buffer

#define SC	0x04    // 00000100B	; Single-Character (kein Repeat)
#define SC_BIT	2	//

#define CK	0x01    // 00000001B	; CTRL-Key
#define SL	0x02    // 00000010B	; Shift-LOCK-Key
#define SK	0x03    // 00000011B	; Shift-Key
#define SHIFT_CTRL_MASK	0x03    // 11B	; Maske fuer SHIFT + CTRL Attribute

#define NK	0	// NORMAL-Key (Character + Repeat)

static uint8_t RAM1_KEY_ATTRIB[] = {
										NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC, // 00-07
										NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC, // 08-0F
										NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC, // 10-17
										NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC ,NK+NC, // 18-1F

										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 20-27
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 28-2F
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 30-37
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 38-3F

										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 40-47
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 48-4F
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 50-57
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 58-5F

										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 60-67
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 68-6F
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK,    // 70-77
										NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK    ,NK     // 78-7F
};

#define INACTIVE  false	// In-Aktiv
#define ACTIVE	  true	// Aktiv

static bool _KB_PREV_SHIFT_LOCK = INACTIVE; 

static bool _KB_PREV_SHIFT = INACTIVE;

static bool _KB_CTRL = INACTIVE;

static bool _KB_SHIFT_LOCK = INACTIVE;

static bool _KB_SHIFT = INACTIVE;

static bool _KB_SHIFT_TOGGLE = INACTIVE; 

static bool _KB_KEY_FLG = INACTIVE;

static uint8_t _KB_KEY_NR; 

static uint8_t _KB_KEY_NR;

static uint8_t _KB_KEY_ATTR;

static uint8_t _KB_KEY_REP_STAT;

static uint8_t _KB_KEY_REP_CNT;

static uint16_t RAM1_AREP_DEL = 30; // Auto-Repeat-Delay     (in 16ms)

static uint16_t RAM1_AREP_FREQ = 8; // Auto-Repeat-Frequence (in 16ms)

static bool _KB_PREV_KEY_FLG = INACTIVE; // :DS BYTE ; Key-pressed Flag: INACTIVE / ACTICE
static uint8_t _KB_PREV_KEY_NR; //: DS BYTE ; Wenn "_KB_KEY_FLG" = ACTIVE, steht hier

static int8_t RAM1_CTRL_OFFS = -0x40;
static uint8_t RAM1_CTRL_LOW = 0x40;
static uint8_t RAM1_CTRL_HIGH = 0x7F+1; 
static uint8_t RAM1_CTRL_UMASK = 0xdf;

static uint8_t RAM1_SCAN_COL = 16;
static uint8_t RAM1_DIP_COL[] =
{
    false, false, false, false, false, false,  true,  true, 
    false, false, false, false, false, false, false, false
};

//here is not static in the original code
static void process_current_column();
static void process_full_run(); 

static uint8_t column = 0;
static uint8_t scan_matrix[16];

static bool kbd_echo = false;

// defined in td01-lcd.c
extern volatile uint16_t* portDataControl;
extern volatile uint8_t* portData;
extern volatile uint8_t* portDataIn;
extern const machine_pin_obj_t *xport_aclk;
extern const machine_pin_obj_t *xport_dclk;
extern const machine_pin_obj_t *xport_ine;
extern void add_to_kbd_buf(uint8_t ch, bool echo);


static uint8_t DIP_SWITCH_BU[16];


//extern int8_t eport_phys_offs;
//extern bool xports_flg;


shared_iram_s *shared_iram;

void kbd_init(void)
{
	for(uint8_t i = 0; i < 128; i++)
	{
        RAM1_KEY_CODES[i] = i;
        RAM1_KEY_SCODES[i] = 128 + i;
	}
	shared_iram = MP_STATE_PORT(SHARED_IRAM_ADDR);//MP_STATE_PORT NEEDS TO BE DONE at RUNTIME

}

void kbd_set_key_table(uint8_t *params)
{
	uint8_t i = 0;
    for(; i<128; ++i)
    {
        RAM1_KEY_CODES[i] = params[i];
    }
}

void kbd_set_key_shift_table(uint8_t *params)
{
    uint8_t i = 0;
    for(; i<128; ++i)
    {
         RAM1_KEY_SCODES[i] = params[i];
    }
}

void kbd_set_key_attribute_table(uint8_t* params)
{
    uint8_t i = 0;
    for(; i<128; ++i)
    {
        RAM1_KEY_ATTRIB[i] = params[i];
    }
}

void kbd_set_column_addresses(uint8_t* params)
{
    uint8_t i = 0;
    for(; i<16; ++i)
    {
        RAM1_KBS_ATAB[i] = params[i];
    }
}

static void kbd_add_to_buf(void)
{
     if((_KB_KEY_ATTR & NB) != 0)
    {
        return;
    }

    uint8_t* key_codes = RAM1_KEY_CODES;

    if(_KB_CTRL == ACTIVE)
    {
        if((_KB_KEY_ATTR & NC) != 0)
        {
            return;
        }

        if(_KB_SHIFT_LOCK==ACTIVE || _KB_SHIFT==ACTIVE || _KB_SHIFT_TOGGLE==ACTIVE)
        {
            if((_KB_KEY_ATTR & NS) != 0)
            {
                return;
            }
            key_codes = RAM1_KEY_SCODES;
        }

        uint8_t ch = key_codes[_KB_KEY_NR];
        if(ch < RAM1_CTRL_LOW || ch >= RAM1_CTRL_HIGH)
        {
            return;
        }

        ch &= RAM1_CTRL_UMASK;
        ch += RAM1_CTRL_OFFS;
        add_to_kbd_buf(ch, kbd_echo);

        return;
    }


    if(_KB_SHIFT_LOCK == ACTIVE || _KB_SHIFT == ACTIVE || _KB_SHIFT_TOGGLE == ACTIVE)
    {
        key_codes = RAM1_KEY_SCODES;
    }
    add_to_kbd_buf(key_codes[_KB_KEY_NR], kbd_echo);
}

void kbd_set_auto_repeat(uint8_t delay, uint8_t freq)
{
    RAM1_AREP_DEL = delay;
    RAM1_AREP_FREQ = freq;
}

extern uint8_t RAM1_SLED_BMASK;
extern uint8_t RAM1_SLED_PORT;
extern bool RAM1_SLED_TFFLG;
static void kbd_led(void)
{  
    uint8_t physAddr;
	
	const machine_pin_obj_t *xport_aclkLocale = xport_aclk;
	const machine_pin_obj_t *xport_dclkLocale = xport_dclk;
    
    // Interrupts speichern
    mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
    
    DELAY_CONFIG(TIM10);
    
    // Alles OUTPUT setzen
    DATA_PORT_TO_OUTPUT(portDataControl);

    // 1. address
    if(xports_flg)
    {
        physAddr = RAM1_SLED_PORT;
    }
    else
    {
        physAddr = RAM1_SLED_PORT + eport_phys_offs;
    }
    *portData = physAddr;
    
    
	DELAY_SET(TIM10, TIMINGS_ACLK);
	mp_hal_pin_toggle(xport_aclkLocale);
	DELAY_WAIT(TIM10);
	mp_hal_pin_toggle(xport_aclkLocale);
    
 
    // 2. write data to bus
    if(_KB_SHIFT_TOGGLE == ACTIVE)
    {
        shared_iram->xport_out_data[physAddr] |= RAM1_SLED_BMASK;
    }
    else
    {
        shared_iram->xport_out_data[physAddr] &= ~RAM1_SLED_BMASK;
    }
    if(RAM1_SLED_TFFLG != 0)
    {
        shared_iram->xport_out_data[physAddr] ^= RAM1_SLED_BMASK;
    }
    *portData = shared_iram->xport_out_data[physAddr];

    
	DELAY_SET(TIM10, TIMINGS_DCLK);
	mp_hal_pin_toggle(xport_dclkLocale);
	DELAY_WAIT(TIM10);
	mp_hal_pin_toggle(xport_dclkLocale);
    

    MICROPY_END_ATOMIC_SECTION(irq_state);
}

void kbd_scan(void)
{
	if(RAM1_SCAN_COL == 0)
    {
		DATA_PORT_TO_OUTPUT(portDataControl);
        return;
    }
	
	const machine_pin_obj_t *xport_aclkLocale = xport_aclk;
	const machine_pin_obj_t *xport_ineLocale = xport_ine;
  
    // Interrupts speichern
    mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
	
    DELAY_CONFIG(TIM10);
    
    DATA_PORT_TO_OUTPUT(portDataControl);
    if(xports_flg)
    {
        *portData = RAM1_KBS_ATAB[column];       // addr to scan
    }
    else
    {
        // Hier benutzen wir das E-Port System, dh. die Adressen muessen noch
        // von LOG->PHYS umgerechnet werden
        // => beim Tiger-1 werden FW Funktionen dafuer aufgerufen, diese
        // sind aber aktuell nicht verpointern, dies wollen wir also
        // hier verhindern.
        *portData = RAM1_KBS_ATAB[column] + eport_phys_offs;      // addr to scan
    }
	
    
	DELAY_SET(TIM10, TIMINGS_ACLK);
	mp_hal_pin_toggle(xport_aclkLocale);
	DELAY_WAIT(TIM10);
	mp_hal_pin_toggle(xport_aclkLocale);
    
    DATA_PORT_TO_INPUT(portDataControl);
    volatile uint8_t* portDataInLocal = portDataIn; 

	DELAY_SET(TIM10, TIMINGS_INE);
	mp_hal_pin_toggle(xport_ineLocale);
	DELAY_WAIT(TIM10);
	scan_matrix[column] = *portDataInLocal;
	mp_hal_pin_toggle(xport_ineLocale);
	// Ensure LCD bus lines are configured back to output mode
	DATA_PORT_TO_OUTPUT(portDataControl);
    
    MICROPY_END_ATOMIC_SECTION(irq_state);


    if(scan_matrix[column] != 0xff)
    {
        process_current_column();
    }

    if(++column != 16)
    {
		DATA_PORT_TO_OUTPUT(portDataControl);
        return;
    }
    column = 0;

    if(column >= RAM1_SCAN_COL)
    {
        return;
    }

    process_full_run();

	_KB_KEY_FLG = INACTIVE;
    //not used  _KB_KEY_CODE_FLG = INACTIVE;
    _KB_CTRL = INACTIVE;
    _KB_SHIFT_LOCK = INACTIVE;
    _KB_SHIFT = INACTIVE;
	
	DATA_PORT_TO_OUTPUT(portDataControl);
}

static void process_current_column(void)
{
    if(RAM1_DIP_COL[column] == true)
    {
        return;
    }

    // get key number: 0x00..0x7f
    uint8_t key = column<<3;

    uint16_t mask = 1;
    for(; mask<(1<<8); mask<<=1)
    {
        //A_LOG("..%#x\n", key);

        if((scan_matrix[column] & mask) == 0)
        {
            //A_LOG("__%#x | %#x\n", RAM1_KEY_ATTRIB[key], RAM1_KEY_ATTRIB[key] & SHIFT_CTRL_MASK);
            // check if key is a special like SHIFT, SHIFT_LOCK oder CTRL
            switch(RAM1_KEY_ATTRIB[key] & SHIFT_CTRL_MASK)
            {
                case 0: // ein char
                {
                    _KB_KEY_NR = key;
                    _KB_KEY_FLG = ACTIVE;
                    _KB_KEY_ATTR = RAM1_KEY_ATTRIB[key];
                    break;
                }
                case CK:
                {
                    _KB_CTRL = ACTIVE;
                    break;
                }
                case SL:
                {
                    _KB_SHIFT_LOCK = ACTIVE;
                    break;
                }
                case SK:
                {
                    _KB_SHIFT = ACTIVE;
                    break;
                }
            }
        }

        ++key;
    }
}

static void process_full_run(void)
{
    // SHIFT, SHIFT_LOCK, CTRL
    if(_KB_SHIFT == ACTIVE && _KB_PREV_SHIFT == ACTIVE)
    {
        _KB_SHIFT_TOGGLE = INACTIVE;
        kbd_led();
    }
    if(_KB_SHIFT_LOCK == ACTIVE && _KB_PREV_SHIFT_LOCK == ACTIVE)
    {
        _KB_SHIFT_TOGGLE = ACTIVE;
        kbd_led();
    }
    _KB_PREV_SHIFT_LOCK = _KB_SHIFT_LOCK;
    _KB_PREV_SHIFT = _KB_SHIFT;

    // "normal" key
    if(_KB_KEY_FLG == ACTIVE)
    {
        // new key
        if(_KB_PREV_KEY_FLG == INACTIVE || _KB_KEY_NR != _KB_PREV_KEY_NR)
        {
            kbd_add_to_buf();

            // init repeat feature
            _KB_KEY_REP_STAT = 0;
            if((_KB_KEY_ATTR & SC) == 0)
            {
                _KB_KEY_REP_STAT = 1;
                _KB_KEY_REP_CNT = RAM1_AREP_DEL;
            }

            // old key: repeat?
        }
        else
        {
            if(_KB_KEY_REP_STAT != 0 && --_KB_KEY_REP_CNT == 0)
            {
                _KB_KEY_REP_CNT = RAM1_AREP_FREQ;
                _KB_KEY_REP_STAT = 2;

                kbd_add_to_buf();
            }
        }
    }
    else
    {
        _KB_KEY_REP_STAT = 0;
    }

    _KB_PREV_KEY_FLG = _KB_KEY_FLG;
    _KB_PREV_KEY_NR = _KB_KEY_NR;

    memcpy(DIP_SWITCH_BU, scan_matrix, 16);
}

uint32_t kbd_read_dip(uint8_t* dest, uint8_t from, uint8_t count)
{
    uint8_t i = from;
    for(; i<(from+count) && i<16; ++i)
    {
        *dest++ = DIP_SWITCH_BU[i];
    }

    return 16 - from;
}

void kbd_set_dip_switches(uint8_t num_scan_cols, uint8_t* col_is_dip)
{
    if(num_scan_cols > 16)
    {
        num_scan_cols = 16;
    }

    RAM1_SCAN_COL = num_scan_cols;

    uint8_t i = 0;
    for(; i<16; ++i)
    {
        RAM1_DIP_COL[i] = (col_is_dip[i]==0 ? true : false);
    }
}

void kbd_set_echo(uint8_t ja_oder_nein)
{
    kbd_echo = (ja_oder_nein==0 ? true : false);
    //A_LOG("set-echo: %u | %u\n", ja_oder_nein, kbd_echo ? 1 : 0);
}

void kbd_set_control(uint8_t* params)
{
    RAM1_CTRL_OFFS = params[0];
    RAM1_CTRL_LOW = params[1];
    RAM1_CTRL_HIGH = params[2];
    RAM1_CTRL_UMASK = params[3];
}

void add_to_kbd_buf(uint8_t ch, bool echo)
{
    if(echo)
    {
        if(o_buf.cnt < MASK)
        {
            o_buf.buf[ o_buf.tail++ ] = ch;
            o_buf.tail &= MASK;
            o_buf.cnt++;
        }
    }

    // buffer full
    if(kbd_buf.cnt >= MASK)
    {
        if(RAM1_KEY_CLICK)
        {
            beep_on(BEEP_BEEP);
        }
        return;
    }

    kbd_buf.buf[ kbd_buf.tail++ ] = ch;
    kbd_buf.tail &= MASK;
    kbd_buf.cnt++;

    if(RAM1_KEY_CLICK)
    {
        beep_on(BEEP_CLICK);
    }
}


bool read_from_kbd_buf(uint8_t *buffer, size_t len)
{
	if(!len) return false;
	if(kbd_buf.cnt < len) return false;
	
	size_t avail = LEN - kbd_buf.head;
	
	if(avail >= len)
	{
		memcpy((void*)buffer, (void const*)(kbd_buf.buf + kbd_buf.head), len);
		kbd_buf.head += len;
	}
	else
	{
		memcpy((void*)buffer, (void const*)(kbd_buf.buf + kbd_buf.head), avail);
		kbd_buf.head = 0;
		memcpy((void*)(buffer+avail), (void const*)(kbd_buf.buf + kbd_buf.head),  len-avail);
		kbd_buf.head  += len-avail;
	}
	kbd_buf.head &= MASK;
	
	mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
	kbd_buf.cnt -= len;
	MICROPY_END_ATOMIC_SECTION(irq_state);
	
	return true;
}

#endif //#if MICROPY_HW_HAS_LCD
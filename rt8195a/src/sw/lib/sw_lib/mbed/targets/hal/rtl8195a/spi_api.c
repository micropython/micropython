#include "platform_options.h"
#include "objects.h"
#include "spi_api.h"
#include "PinNames.h"
#include "pinmap.h"
#include "hal_ssi.h"

extern u32 SystemGetCpuClk(VOID);

// Extended Function Prototypes
void spi_slave_receive_interrupt(spi_t *obj, int *rx_buffer, int length);
void spi_master_write_interrupt(spi_t *obj, int *tx_buffer, int length);
// OOC Function Prototypes
spi_t* spi_class_get_instance(uint32_t index, uint32_t pinmux);


uint8_t SPI0_IS_AS_SLAVE = 0;

static const PinMap PinMap_SSI_MOSI[] = {
    {PE_2,  RTL_PIN_PERI(SPI0, 0, S0), RTL_PIN_FUNC(SPI0, S0)},
    {PC_2,  RTL_PIN_PERI(SPI0, 0, S1), RTL_PIN_FUNC(SPI0, S1)},
    {PA_1,  RTL_PIN_PERI(SPI1, 1, S0), RTL_PIN_FUNC(SPI1, S0)},
    {PB_6,  RTL_PIN_PERI(SPI1, 1, S1), RTL_PIN_FUNC(SPI1, S1)},
    {PD_6,  RTL_PIN_PERI(SPI1, 1, S2), RTL_PIN_FUNC(SPI1, S2)},
    {PG_2,  RTL_PIN_PERI(SPI2, 2, S0), RTL_PIN_FUNC(SPI2, S0)},
    {PE_6,  RTL_PIN_PERI(SPI2, 2, S1), RTL_PIN_FUNC(SPI2, S1)},
    {PD_2,  RTL_PIN_PERI(SPI2, 2, S2), RTL_PIN_FUNC(SPI2, S2)},
    {NC,    NC,     0}
};

static const PinMap PinMap_SSI_MISO[] = {
    {PE_3,  RTL_PIN_PERI(SPI0, 0, S0), RTL_PIN_FUNC(SPI0, S0)},
    {PC_3,  RTL_PIN_PERI(SPI0, 0, S1), RTL_PIN_FUNC(SPI0, S1)},
    {PA_0,  RTL_PIN_PERI(SPI1, 1, S0), RTL_PIN_FUNC(SPI1, S0)},
    {PB_7,  RTL_PIN_PERI(SPI1, 1, S1), RTL_PIN_FUNC(SPI1, S1)},
    {PD_7,  RTL_PIN_PERI(SPI1, 1, S2), RTL_PIN_FUNC(SPI1, S2)},
    {PG_3,  RTL_PIN_PERI(SPI2, 2, S0), RTL_PIN_FUNC(SPI2, S0)},
    {PE_7,  RTL_PIN_PERI(SPI2, 2, S1), RTL_PIN_FUNC(SPI2, S1)},
    {PD_3,  RTL_PIN_PERI(SPI2, 2, S2), RTL_PIN_FUNC(SPI2, S2)},
    {NC,    NC,     0}
};


void spi_init (spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel)
{
    SSI_DBG_ENTRANCE("spi_init()\n");

    //TODO: Replace pinmap_peripheral() & pinmap_merge() with spi_class_get_instance();
    uint32_t ssi_mosi, ssi_miso, ssi_peri;
    uint8_t  ssi_idx, ssi_pinmux;
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    uint32_t SystemClock = SystemGetCpuClk();
    uint32_t MaxSsiFreq  = (SystemClock >> 2) >> 1;
    uint32_t SsiClockDivider = MaxSsiFreq / 1000000;  // 1MHz

    /* SsiClockDivider doesn't support odd number */
    SsiClockDivider = (SsiClockDivider + 1) & 0xfffe;

    DBG_SSI_INFO("SystemClock: %d\n", SystemClock);
    DBG_SSI_INFO("MaxSsiFreq : %d, SSI ClockDivider: %d (1MHz)\n", MaxSsiFreq, SsiClockDivider);

    ssi_mosi = pinmap_peripheral(mosi, PinMap_SSI_MOSI);
    ssi_miso = pinmap_peripheral(miso, PinMap_SSI_MISO);
    //DBG_SSI_INFO("ssi_mosi: %d, ssi_miso: %d\n", ssi_mosi, ssi_miso);

    ssi_peri = pinmap_merge(ssi_mosi, ssi_miso);
    if (unlikely(ssi_peri == NC)) {
        DBG_SSI_ERR("spi_init(): Cannot find matched SSI index.\n");
        return;
    }
    ssi_idx = RTL_GET_PERI_IDX(ssi_peri);
    ssi_pinmux = RTL_GET_PERI_SEL(ssi_peri);
    DBG_SSI_INFO("ssi_peri: %d, ssi_idx: %d, ssi_pinmux: %d\n", ssi_peri, ssi_idx, ssi_pinmux);


    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    pHalSsiAdaptor->Index = ssi_idx;
    pHalSsiAdaptor->PinmuxSelect = ssi_pinmux;

    // XXX: Only for test
    if ((ssi_idx == 0) && (SPI0_IS_AS_SLAVE == 1)) {
        //DBG_SSI_INFO("SSI%d will be as slave. (spi0_is_slave: %d)\n", index, spi0_is_slave);
        pHalSsiAdaptor->Role = SSI_SLAVE;
    }
    else {
        //DBG_SSI_INFO("SSI%d will be as master. (spi0_is_slave: %d)\n", index, spi0_is_slave);
        pHalSsiAdaptor->Role = SSI_MASTER;
    }

    HalSsiOpInit(pHalSsiOp);

    pHalSsiOp->HalSsiSetDeviceRole(pHalSsiAdaptor, pHalSsiAdaptor->Role);

    pHalSsiOp->HalSsiPinmuxEnable(pHalSsiAdaptor);

    /* Pinmux workaround */
    if ((ssi_idx == 0) && (ssi_pinmux == SSI0_MUX_TO_GPIOC)) {
        EEPROM_PIN_CTRL(OFF);
    }

    if ((ssi_idx == 0) && (ssi_pinmux == SSI0_MUX_TO_GPIOE)) {
        JTAG_PIN_FCTRL(OFF);
    }

    //TODO: Load default Setting: It should be loaded from external setting file.
    DW_SSI_DEFAULT_SETTING SpiDefaultSetting =
    {
        .RxCompCallback    = NULL,
        .RxCompCbPara      = NULL,
        .RxData            = NULL,
        .TxCompCallback    = NULL,
        .TxCompCbPara      = NULL,
        .TxData            = NULL,
        .DmaRxDataLevel    =    7,
        .DmaTxDataLevel    =   48,
        .InterruptPriority = 0x20,
        .RxLength          =    0,
        .RxLengthRemainder =    0,
        .RxThresholdLevel  =    0,
        .TxLength          =    0,
        .TxThresholdLevel  =    1,
        .SlaveSelectEnable =    0,
        .ClockDivider      = SYSTEM_CLK/1000000,
        .DataFrameNumber   =    0,
        .ControlFrameSize  = CFS_1_BIT,
        .DataFrameFormat   = FRF_MOTOROLA_SPI,
        .DataFrameSize     = DFS_8_BITS,
        .DmaControl        = SSI_TRDMA_ENABLE,
        .InterruptMask     =  0x0,
        .MicrowireDirection    = MW_DIRECTION_MASTER_TO_SLAVE,
        .MicrowireHandshaking  = MW_HANDSHAKE_DISABLE,
        .MicrowireTransferMode = MW_TMOD_NONSEQUENTIAL,
        .SclkPhase             = SCPH_TOGGLES_AT_START,
        .SclkPolarity          = SCPOL_INACTIVE_IS_HIGH,
        .SlaveOutputEnable     = SLV_TXD_ENABLE,  // Slave
        .TransferMode          = TMOD_TR,
        .TransferMechanism     = SSI_DTM_INTERRUPT
    };

    //TODO: Implement default setting structure.
    pHalSsiOp->HalSsiLoadSetting(pHalSsiAdaptor, &SpiDefaultSetting);

#if 1
    //XXX: Interrupt Transfer Mechanism Test
    if (ssi_idx == 0) {  // SPI 0 Slave
        pHalSsiAdaptor->TransferMechanism = SSI_DTM_INTERRUPT;
        pHalSsiAdaptor->RxThresholdLevel = 0; // 0 + 1 = 1
    }
#endif

#if 1
    if (ssi_idx == 1) { // SPI 1 Master
        pHalSsiAdaptor->TransferMechanism = SSI_DTM_INTERRUPT;
        pHalSsiAdaptor->TxThresholdLevel = 20;
    }
#endif

    pHalSsiOp->HalSsiInit(pHalSsiAdaptor);
}

void spi_free (spi_t *obj)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    pHalSsiOp->HalSsiInterruptDisable(pHalSsiAdaptor);
    pHalSsiOp->HalSsiDisable(pHalSsiAdaptor);
}

void spi_format (spi_t *obj, int bits, int mode, int slave)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    pHalSsiAdaptor->DataFrameSize = (bits - 1);

    /*
     * mode | POL PHA
     * -----+--------
     *   0  |  0   0
     *   1  |  0   1
     *   2  |  1   0
     *   3  |  1   1
     *
     * SCPOL_INACTIVE_IS_LOW  = 0,
     * SCPOL_INACTIVE_IS_HIGH = 1
     *
     * SCPH_TOGGLES_IN_MIDDLE = 0,
     * SCPH_TOGGLES_AT_START  = 1
     */
    switch (mode)
    {
        case 0:
            pHalSsiAdaptor->SclkPolarity = SCPOL_INACTIVE_IS_LOW;
            pHalSsiAdaptor->SclkPhase    = SCPH_TOGGLES_IN_MIDDLE;
            break;
        case 1:
            pHalSsiAdaptor->SclkPolarity = SCPOL_INACTIVE_IS_LOW;
            pHalSsiAdaptor->SclkPhase    = SCPH_TOGGLES_AT_START;
            break;
        case 2:
            pHalSsiAdaptor->SclkPolarity = SCPOL_INACTIVE_IS_HIGH;
            pHalSsiAdaptor->SclkPhase    = SCPH_TOGGLES_IN_MIDDLE;
            break;
        case 3:
            pHalSsiAdaptor->SclkPolarity = SCPOL_INACTIVE_IS_HIGH;
            pHalSsiAdaptor->SclkPhase    = SCPH_TOGGLES_AT_START;
            break;
        default:  // same as 3
            pHalSsiAdaptor->SclkPolarity = SCPOL_INACTIVE_IS_HIGH;
            pHalSsiAdaptor->SclkPhase    = SCPH_TOGGLES_AT_START;
            break;
    }

    if (slave == 1) {
        if (pHalSsiAdaptor->Index == 0) {
            pHalSsiAdaptor->Role = SSI_SLAVE;
            SPI0_IS_AS_SLAVE = 1;
            //DBG_SSI_INFO("SPI0 is as slave\n");
        }
        else {
            pHalSsiAdaptor->Role = SSI_MASTER;
        }
    }
    else {
        pHalSsiAdaptor->Role = SSI_MASTER;
    }

    pHalSsiOp->HalSsiInit(pHalSsiAdaptor);
}

void spi_frequency (spi_t *obj, int hz)
{
    // Frequency depend of ASIC clock
    /**
     * TODO: if (hz > 1000000) {...}
     */
    u32 spi_clock;
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    if (hz > SYSTEM_CLK) {
        pHalSsiAdaptor->ClockDivider = SYSTEM_CLK/1000000;  // force 1M Hz
        pHalSsiOp->HalSsiInit(pHalSsiAdaptor);
        return;
    }

    if (hz < 500000) {
        spi_clock = 250000;  // 250K Hz
    }
    else if ((hz >= 500000) && (hz < 1000000)) {
        spi_clock = 500000;  // 500K Hz
    }
    else if ((hz >= 1000000) && (hz < 2000000)) {
        spi_clock = 1000000;  // 1M Hz
    }
    else if ((hz >= 2000000) && (hz < 4000000)) {
        spi_clock = 2000000;  // 2M Hz
    }
    else if ((hz >= 4000000) && (hz < 8000000)) {
        spi_clock = 4000000;  // 4M Hz
    }
    else if ((hz >= 8000000) && (hz < 16000000)) {
        spi_clock = 8000000;  // 8M Hz
    }
    else if ((hz >= 16000000) && (hz < 20000000)) {
        spi_clock = 16000000;  // 16M Hz
    }
    else { // >= 20000000 (20M Hz)
        if (pHalSsiAdaptor->Index == 1) {
            spi_clock = 40000000;  // SPI1 max speed: 41.5M Hz
        }
        else {
            spi_clock = 20000000;  // max speed: 20M Hz
        }
    }

    pHalSsiAdaptor->ClockDivider = SYSTEM_CLK/spi_clock;
    pHalSsiOp->HalSsiInit(pHalSsiAdaptor);
}

static inline void ssi_write (spi_t *obj, int value)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    while (!pHalSsiOp->HalSsiWriteable(pHalSsiAdaptor));
    pHalSsiOp->HalSsiWrite((VOID*)pHalSsiAdaptor, value);
}

static inline int ssi_read(spi_t *obj)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    while (!pHalSsiOp->HalSsiReadable(pHalSsiAdaptor));
    return (int)pHalSsiOp->HalSsiRead(pHalSsiAdaptor);
}

int spi_master_write (spi_t *obj, int value)
{
    ssi_write(obj, value);
    return ssi_read(obj);
}

int spi_slave_receive (spi_t *obj)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;
    int Readable;
    int Busy;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    Readable = pHalSsiOp->HalSsiReadable(pHalSsiAdaptor);
    Busy     = (int)pHalSsiOp->HalSsiBusy(pHalSsiAdaptor);
    return ((Readable && !Busy) ? 1 : 0);
}

int spi_slave_read (spi_t *obj)
{
    return ssi_read(obj);
}

void spi_slave_write (spi_t *obj, int value)
{
    ssi_write(obj, value);
}

int spi_busy (spi_t *obj)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    return (int)pHalSsiOp->HalSsiBusy(pHalSsiAdaptor);
}

void spi_slave_receive_interrupt(spi_t *obj, int *rx_buffer, int length)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;
    
    //DBG_SSI_INFO("rx_buffer addr: %X, length: %d\n", rx_buffer, length);
    pHalSsiOp->HalSsiReadInterrupt(pHalSsiAdaptor, rx_buffer, length);
}

void spi_master_write_interrupt(spi_t *obj, int *tx_buffer, int length)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor;
    PHAL_SSI_OP pHalSsiOp;

    pHalSsiAdaptor = &obj->spi_adp;
    pHalSsiOp = &obj->spi_op;

    //DBG_SSI_INFO("tx_buffer addr: %X, length: %d\n", tx_buffer, length);
    pHalSsiOp->HalSsiWriteInterrupt(pHalSsiAdaptor, tx_buffer, length);
}


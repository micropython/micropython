// *FORMAT-OFF*
/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "tusb_option.h"

#if CFG_TUD_ENABLED

#include "device/dcd.h"
#include "dcd_ensemble_def.h"

#if CFG_TUSB_OS == OPT_OS_ZEPHYR
#include <zephyr/cache.h>
#include <soc_common.h>
#include <soc_memory_map.h>
#else
#include "RTE_Components.h"
#include CMSIS_device_header
#include "sys_clocks.h"
#include "sys_utils.h"
#include "sys_ctrl_usb.h"
#endif

// Logging definitions
#if defined(TUSB_ALIF_DEBUG)
#if (TUSB_ALIF_DEBUG_DEPTH > 1)
#define LOG(...)      { int _bi = bi++; memset(logbuf[_bi % TUSB_ALIF_DEBUG_DEPTH], ' ', 48);\
                        snprintf(logbuf[(_bi) % TUSB_ALIF_DEBUG_DEPTH], 48, __VA_ARGS__); }
char logbuf[TUSB_ALIF_DEBUG_DEPTH][48] __attribute__((aligned(16)));
int bi = 0;
#else
#define LOG(...)      memset(logbuf, ' ', 48);\
                          snprintf(logbuf, 48, __VA_ARGS__)
char logbuf[48];
#endif
#else
#define LOG(...)
#endif

// Defines --------------------------------------------------------
#if CFG_TUSB_OS == OPT_OS_ZEPHYR

// USB Registers
#define EXPMST_USB_GPIO0            (EXPMST_BASE + 0xA0)
#define EXPMST_USB_STAT0            (EXPMST_BASE + 0xA4)
#define EXPMST_USB_CTRL1            (EXPMST_BASE + 0xA8)
#define EXPMST_USB_CTRL2            (EXPMST_BASE + 0xAC)

// Enable USB_CLK and 10M_CLK (CLK_ENA Register)
#define CLK_ENA_CLK20M              BIT(22)

// Enable clock for USB (PERIPH_CLK_ENA Register)
#define PERIPH_CLK_ENA_USB_CKEN     BIT(20)

// USB PHY Power Control (PWR_CTRL Register)
#define PWR_CTRL_UPHY_ISO           BIT(17) // USB PHY Isolation Enable
#define PWR_CTRL_UPHY_PWR_MASK      BIT(16) // USB PHY Power Mask

// USB PHY PoR Reset Mask (USB_CTRL2 Register)
#define USB_CTRL2_POR_RST_MASK      BIT(8)

// USB Interrupt Number Definition
#define USB_NODE DT_NODELABEL(usb0)
#define USB_IRQ_IRQn DT_IRQN(USB_NODE)

#endif

// Packet size
#define MAX_PACKET_SIZE                 512
#define MAX_PACKET_SIZE_ON_USB_RESET    64
#define SETUP_PACKET_SIZE               8

// Buffer size
#define EVT_BUF_SIZE            1024
#define CTRL_BUF_SIZE           64  // TODO: Check if correct value is SETUP_PACKET_SIZE

// Structs and Buffers --------------------------------------------------------

// TODO: Use dynamic buffer allocation to reduce memory usage
// USB Event buffer
static uint32_t _evnt_buf[EVT_BUF_SIZE] CFG_TUSB_MEM_SECTION TU_ATTR_ALIGNED(4096);
// Control buffer
static uint8_t _ctrl_buf[CTRL_BUF_SIZE] CFG_TUSB_MEM_SECTION TU_ATTR_ALIGNED(32);
// USB Transfer request Blocks
static uint32_t _xfer_trb[2*TUP_DCD_ENDPOINT_MAX][4] CFG_TUSB_MEM_SECTION TU_ATTR_ALIGNED(32);
// Transfer bytes
static uint16_t _xfer_bytes[2*TUP_DCD_ENDPOINT_MAX];

static volatile uint32_t *_evnt_tail = NULL;
static bool _ctrl_long_data = false;
static volatile bool _xfer_cfgd = false;
static uint32_t _sts_stage = 0;
static uint16_t _ep_dir_out_mps[4] = {MAX_PACKET_SIZE, 0, 0, 0};

/// Private Functions ----------------------------------------------------------

static void _dcd_handle_depevt(uint8_t rhport, uint8_t ep, uint8_t evt, uint8_t sts, uint16_t par);
static void _dcd_handle_devt(uint8_t rhport, uint8_t evt, uint16_t info);
static uint8_t _dcd_start_xfer(uint8_t ep, void* buf, uint32_t size, uint8_t type);
static uint8_t _dcd_cmd_wait(uint8_t ep, uint8_t typ, uint16_t param);

// Helpers

static inline uint32_t _get_transfered_bytes(uint8_t ep) {
    trb_t *trb = (trb_t *)_xfer_trb[ep];
    TU_ASSERT(_xfer_bytes[ep] >= trb->bufsiz, 0);
    return _xfer_bytes[ep] - trb->bufsiz;
}

#if CFG_TUSB_OS == OPT_OS_ZEPHYR

static inline void enable_cgu_clk20m(void) {
    sys_set_bits(CGU_CLK_ENA, CLK_ENA_CLK20M);
}

static inline void disable_cgu_clk20m(void) {
    sys_clear_bits(CGU_CLK_ENA, CLK_ENA_CLK20M);
}

static inline void enable_usb_periph_clk(void) {
    sys_set_bits(EXPMST_PERIPH_CLK_EN, PERIPH_CLK_ENA_USB_CKEN);
}

static inline void disable_usb_periph_clk(void) {
    sys_clear_bits(EXPMST_PERIPH_CLK_EN, PERIPH_CLK_ENA_USB_CKEN);
}

static inline void enable_usb_phy_power(void) {
    sys_clear_bits(VBAT_PWR_CTRL, PWR_CTRL_UPHY_PWR_MASK);
}

static inline void disable_usb_phy_power(void) {
    sys_set_bits(VBAT_PWR_CTRL, PWR_CTRL_UPHY_PWR_MASK);
}

static inline void enable_usb_phy_isolation(void) {
    sys_set_bits(VBAT_PWR_CTRL, PWR_CTRL_UPHY_ISO);
}

static inline void disable_usb_phy_isolation(void) {
    sys_clear_bits(VBAT_PWR_CTRL, PWR_CTRL_UPHY_ISO);
}

static inline void usb_ctrl2_phy_power_on_reset_set(void) {
    sys_set_bits(EXPMST_USB_CTRL2, USB_CTRL2_POR_RST_MASK);
}

static inline void usb_ctrl2_phy_power_on_reset_clear(void) {
    sys_clear_bits(EXPMST_USB_CTRL2, USB_CTRL2_POR_RST_MASK);
}

static inline void _dcd_busy_wait(uint32_t usec) {
    k_busy_wait(usec);
}

static inline void _dcd_clean_dcache(void* dptr, size_t size) {
    sys_cache_data_flush_range(dptr, size);
}

static inline void _dcd_invalidate_dcache(void* dptr, size_t size) {
    sys_cache_data_invd_range(dptr, size);
}

static inline uint32_t _dcd_local_to_global(const volatile void *local_addr) {
    return local_to_global(local_addr);
}

#else

#define CLK_ENA_CLK20M              (1U << 22)   // Enable USB and 10M_CLK
#define VBAT_PWR_CTRL_UPHY_PWR_MASK (1U << 16)   // Mask off the power supply for USB PHY
#define VBAT_PWR_CTRL_UPHY_ISO      (1U << 17)   // Enable isolation for USB PHY

static inline void enable_cgu_clk20m(void) {
    CGU->CLK_ENA |= CLK_ENA_CLK20M;
}

static inline void disable_cgu_clk20m(void) {
    CGU->CLK_ENA &= ~CLK_ENA_CLK20M;
}

static inline void enable_usb_phy_power(void) {
    VBAT->PWR_CTRL &= ~VBAT_PWR_CTRL_UPHY_PWR_MASK;
}

static inline void disable_usb_phy_power(void) {
    VBAT->PWR_CTRL |= VBAT_PWR_CTRL_UPHY_PWR_MASK;
}

static inline void enable_usb_phy_isolation(void) {
    VBAT->PWR_CTRL |= VBAT_PWR_CTRL_UPHY_ISO;
}

static inline void disable_usb_phy_isolation(void) {
    VBAT->PWR_CTRL &= ~VBAT_PWR_CTRL_UPHY_ISO;
}

static inline void usb_ctrl2_phy_power_on_reset_set(void) {
    usb_phy_por_set();
}

static inline void usb_ctrl2_phy_power_on_reset_clear(void) {
    usb_phy_por_clear();
}

static inline void _dcd_busy_wait(uint32_t usec) {
    sys_busy_loop_us(usec);
}

static inline void _dcd_clean_dcache(void* dptr, size_t size) {
    RTSS_CleanDCache_by_Addr(dptr, size);
}

static inline void _dcd_invalidate_dcache(void* dptr, size_t size) {
    RTSS_InvalidateDCache_by_Addr(dptr, size);
}

static inline uint32_t _dcd_local_to_global(const volatile void *local_addr) {
    return LocalToGlobal(local_addr);
}

#endif

/// Device Setup ---------------------------------------------------------------

// Initializes the USB peripheral for device mode and enables it.
// This function should enable internal D+/D- pull-up for enumeration.
bool dcd_init(uint8_t rhport, const tusb_rhport_init_t* rh_init) {
    (void) rh_init;

    // Enable 20mhz clock
    enable_cgu_clk20m();
    // Enable usb peripheral clock
    enable_usb_periph_clk();
    // Power up usb phy
    enable_usb_phy_power();
    // Disable usb phy isolation
    disable_usb_phy_isolation();
    // Clear usb phy power-on-reset signal
    usb_ctrl2_phy_power_on_reset_clear();

    // NOTE: Force stop/disconnect could be used for debug purpose only
    //dcd_disconnect(rhport);

    // NOTE: The order of the following operations is not important, except for
    // the first and last steps (DCTL[CSFTRST] = 0x1 and DCTL[RUN_STOP] = 0x1).

    // Reset the USB device controller
    udev->dctl_b.csftrst = 1;           // Set CSFTRST to 1
    while(0 != udev->dctl_b.csftrst);   // and wait for a read to return 0x0

    // NOTE: Core and UTMI PHY reset could be used for debug purpose only
    //_dcd_busy_wait(50000);
    //ugbl->gctl_b.coresoftreset = 1;
    //ugbl->gusb2phycfg0_b.physoftrst = 1;
    //_dcd_busy_wait(50000);
    //ugbl->gusb2phycfg0_b.physoftrst = 0;
    //_dcd_busy_wait(50000);
    //ugbl->gctl_b.coresoftreset = 0;
    //_dcd_busy_wait(50000);

    // Configure USB SoC BUS
    ugbl->gsbuscfg0_b.incr16brstena = 1; // INCR16 burst type enable
    // Leave the default values for GSBUSCFG1

    // Leave the default values for GTXTHRCFG/GRXTHRCFG, unless thresholding is enabled

    // Read the ID register to find the controller version
    // and configure the driver for any version-specific features.
    // Check controller ID
    uint32_t gsnpsid = ugbl->gsnpsid;
    if ((gsnpsid & 0xFFFF0000) != GSNPSID_HIGH) {
        // Invalid USB controller ID!
        LOG("INVLUSBCTRLID=0x%08x(expected 0x%08x)",
            gsnpsid, (GSNPSID_HIGH | GSNPSID_LOW));
        return false;
    }
    if ((gsnpsid & 0x0000FFFF) != GSNPSID_LOW) {
        // Unsupported USB controller version!
        LOG("USBCTRLVER=0x%08x(expected 0x%08x)",
            gsnpsid, (GSNPSID_HIGH | GSNPSID_LOW));
    }

    // Configure USB2 PHY
    ugbl->gusb2phycfg0_b.usbtrdtim = GUSB2PHYCFG0_USBTRDTIM_16BIT;  // UTMI+
    ugbl->gusb2phycfg0_b.phyif = 1;                                 // 16bit
    // Leave the default values for TOUTCAL

    // Leave the default values for GTXFIFOSIZn and GRXFIFOSIZn
    // NOTE: check the values for isochronous endpoints

    // Allocate ring buffer for events
    memset(_evnt_buf, 0, sizeof(_evnt_buf));
    _dcd_clean_dcache(_evnt_buf, sizeof(_evnt_buf));
    _evnt_tail = _evnt_buf;
    // Set Event Buffer Address
    ugbl->gevntadrlo0 = (uint32_t) _dcd_local_to_global(_evnt_buf);
    // Set Event Buffer Size
    ugbl->gevntsiz0_b.eventsiz = (uint32_t) sizeof(_evnt_buf);
    // Clear Event Buffer counter
    ugbl->gevntcount0_b.evntcount = 0;

    // Leave the default values for GCTL

    // Set USB speed
#if CFG_TUD_MAX_SPEED == OPT_MODE_DEFAULT_SPEED || \
    CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED
    udev->dcfg_b.devspd = DCFG_DEVSPD_HS;
#elif CFG_TUD_MAX_SPEED == OPT_MODE_FULL_SPEED
    udev->dcfg_b.devspd = DCFG_DEVSPD_FS;
#else
    #error "Alif USB supports full/high speed only."
#endif

    // Enable USB events
    udev->devten_b.usbrstevten = 1;     // USB reset
    udev->devten_b.connectdoneevten = 1;// Connection done
    udev->devten_b.dissconnevten = 1;   // Disconnect detected
    udev->devten_b.ulstcngen = 1;       // Link state change
    // NOTE: additional events coudl be used for debug purpose

    // Endpoint Configuration - Start config phase
    _dcd_cmd_wait(PHY_EP0, CMDTYP_DEPSTARTCFG, 0);

    // Endpoint Configuration - EP0 OUT
    depcfg_params_t *depcfg_params = (depcfg_params_t *)udev->depcmd[PHY_EP0].params;
    depcfg_params->eptype = 0;                              // Control Endpoint
    depcfg_params->mps = MAX_PACKET_SIZE;                   // Max Packet Size
    depcfg_params->fifonum = 0;                             // FIFO number
    depcfg_params->brstsiz = 0;                             // Burst Size
    depcfg_params->configaction = DEPCFG_CONFIGACTION_INIT; // Initialize EP state
    depcfg_params->intrnum = 0;                             // Interrupt number
    depcfg_params->xfercmplen = 1;                          // XferComplete Enable
    depcfg_params->xferinprogen = 0;                        // XferInProgress Disable
    depcfg_params->xfernrdyen = 1;                          // XferNotReady Enable
    depcfg_params->streamevten = 0;                         // Stream Event Disable
    depcfg_params->tbrnotupdt = 0;                          // Write back the TRB descriptor
    depcfg_params->ebcen = 0;                               // EBC mode Disable
    depcfg_params->bintervalm1 = 0;                         // bInterval
    depcfg_params->strmcap = 0;                             // Not Stream Capable
    depcfg_params->epnum = PHY_EP0;                         // OUT endpoint
    depcfg_params->fifobased = 0;                           // Not FIFO based
    _dcd_cmd_wait(PHY_EP0, CMDTYP_DEPCFG, 0);

    // Endpoint Configuration - EP0 IN
    depcfg_params = (depcfg_params_t *)udev->depcmd[PHY_EP1].params;
    depcfg_params->eptype = 0;                              // Control Endpoint
    depcfg_params->mps = MAX_PACKET_SIZE;                   // Max Packet Size
    depcfg_params->fifonum = 0;                             // FIFO number
    depcfg_params->brstsiz = 0;                             // Burst Size
    depcfg_params->configaction = DEPCFG_CONFIGACTION_INIT; // Initialize EP state
    depcfg_params->intrnum = 0;                             // Interrupt number
    depcfg_params->xfercmplen = 1;                          // XferComplete Enable
    depcfg_params->xferinprogen = 0;                        // XferInProgress Disable
    depcfg_params->xfernrdyen = 1;                          // XferNotReady Enable
    depcfg_params->streamevten = 0;                         // Stream Event Disable
    depcfg_params->tbrnotupdt = 0;                          // Write back the TRB descriptor
    depcfg_params->ebcen = 0;                               // EBC mode Disable
    depcfg_params->bintervalm1 = 0;                         // bInterval
    depcfg_params->strmcap = 0;                             // Not Stream Capable
    depcfg_params->epnum = PHY_EP1;                         // IN endpoint
    depcfg_params->fifobased = 0;                           // Not FIFO based
    _dcd_cmd_wait(PHY_EP1, CMDTYP_DEPCFG, 0);

    // Set initial XFER configuration for CONTROL eps
    depxfercfg_params_t *depxfercfg_params = (depxfercfg_params_t *)udev->depcmd[0].params;
    depxfercfg_params->numxferres = 1;// Number of Transfer Resources
    _dcd_cmd_wait(PHY_EP0, CMDTYP_DEPXFERCFG, 0);
    depxfercfg_params = (depxfercfg_params_t *)udev->depcmd[1].params;
    depxfercfg_params->numxferres = 1;// Number of Transfer Resources
    _dcd_cmd_wait(PHY_EP1, CMDTYP_DEPXFERCFG, 0);

    // Prepare TRB for the first setup packet
    memset(_ctrl_buf, 0, sizeof(_ctrl_buf));
    trb_t *xfer_trb0 = (trb_t *)_xfer_trb[0];
    memset(xfer_trb0, 0, sizeof(trb_t));
    xfer_trb0->bptrl = (uint32_t) _dcd_local_to_global(_ctrl_buf);
    xfer_trb0->bptrh = 0;
    xfer_trb0->bufsiz = SETUP_PACKET_SIZE;  // Buffer size
    xfer_trb0->intcmpl = 1;                 // Interrupt on Short Packet
    xfer_trb0->ispimi = 1;                  // Interrupt on Short Packet
    xfer_trb0->trbctl = TRBCTL_CTL_SETUP;   // Control Setup
    xfer_trb0->lst = 1;                     // Last TRB
    xfer_trb0->hwo = 1;                     // Set HWO to create TRB
    _dcd_clean_dcache(xfer_trb0, sizeof(trb_t));

    // Send TRB to the USB DMA
    depstrtxfer_params_t *depstrtxfer_params = (depstrtxfer_params_t *)udev->depcmd[0].params;
    depstrtxfer_params->tdaddrlow = (uint32_t) _dcd_local_to_global(xfer_trb0);
    depstrtxfer_params->tdaddrhigh = 0;
    _dcd_cmd_wait(PHY_EP0, CMDTYP_DEPSTRTXFER, 0);

    // Enable event interrupts for EP0-OUT and EP0-IN
    udev->dalepena_b.usbactep0out = 1;
    udev->dalepena_b.usbactep0in = 1;

    // Allow device to attach to the host (enable pull-ups)
    dcd_connect(rhport);

    // Enable interrupts in the NVIC
    NVIC_ClearPendingIRQ(USB_IRQ_IRQn);
    NVIC_SetPriority(USB_IRQ_IRQn, 5);
#if CFG_TUSB_OS == OPT_OS_ZEPHYR
    IRQ_CONNECT(USB_IRQ_IRQn, 5, dcd_int_handler, NULL, 0);
#else
    dcd_int_enable(rhport);
#endif

    return true;
}

// Processes all the hardware generated events e.g bus reset, new data packet
// from host... It will be called by application in the MCU USB interrupt handler.
void dcd_int_handler(uint8_t rhport)
{
    LOG("%010u IRQ enter, evntcount %u", DWT->CYCCNT, ugbl->gevntcount0_b.evntcount);

    // process failures first
    if (ugbl->gsts_b.device_ip) {
        if (ugbl->gsts_b.csrtimeout || ugbl->gsts_b.buserraddrvld) {
            // buserraddrvld is usually set when USB tries to write to protected
            // memory region. Check linker script to ensure USB event buffer and
            // TRBs reside in bulk memory or other NS-allowed region
            // LOG("Bus error:CSRTimeout=%d,BusErrAddrVld=%d",
            //      ugbl->gsts_b.csrtimeout, ugbl->gsts_b.buserraddrvld);
            __BKPT(0);
        }
    }

    // cycle through event queue
    // evaluate on every iteration to prevent unnecessary isr exit/reentry
    while (ugbl->gevntcount0_b.evntcount > 0) {
        _dcd_invalidate_dcache(_evnt_buf, sizeof(_evnt_buf));
        evt_t e = {.val = *_evnt_tail++};

        LOG("%010u IRQloop, evtcnt %u evt %08x", DWT->CYCCNT,
            ugbl->gevntcount0_b.evntcount, e.val);

        // wrap around
        if (_evnt_tail >= (_evnt_buf + EVT_BUF_SIZE))
            _evnt_tail = _evnt_buf;

        // dispatch the right handler for the event type
        if (e.depevt.sig == 0) { // DEPEVT
            _dcd_handle_depevt(rhport, e.depevt.ep, e.depevt.evt, e.depevt.sts, e.depevt.par);
        } else if (e.devt.sig == 1) { // DEVT
            _dcd_handle_devt(rhport, e.devt.evt, e.devt.info);
        } else {
            // Malformed event
            LOG("Unknown event %u", e.val);
            __BKPT(0);
        }

        // consume one event
        //ugbl->gevntcount0 = 4;
        // Decrement the event counter by the one event size
        ugbl->gevntcount0_b.evntcount = sizeof(evt_t);
    }

    // Clear handler busy flag, after finishing event processing
    //ugbl->gevntcount0_b.evnt_handler_busy = 0;

    LOG("%010u IRQ exit, evntcount %u", DWT->CYCCNT, ugbl->gevntcount0_b.evntcount);
}

// Enable the USB device interrupt.
void dcd_int_enable (uint8_t rhport) {
    (void) rhport;
    NVIC_EnableIRQ(USB_IRQ_IRQn);
}

// Disables the USB device interrupt.
// May be used to prevent concurrency issues when mutating data structures
// shared between main code and the interrupt handler.
void dcd_int_disable(uint8_t rhport) {
    (void) rhport;

    NVIC_DisableIRQ(USB_IRQ_IRQn);
}

// Receive Set Address request, mcu port must also include status IN response.
// If your peripheral automatically changes address during enumeration you may
// leave this empty and also no queue an event for the corresponding SETUP packet.
void dcd_set_address(uint8_t rhport, uint8_t dev_addr) {
    LOG("%010u >%s", DWT->CYCCNT, __func__);
    // Set device address
    udev->dcfg_b.devaddr = dev_addr;
    dcd_edpt_xfer(rhport, tu_edpt_addr(0, TUSB_DIR_IN), NULL, 0);

    // NOTE: After receiving the DEPEVT_XFERNOTREADY event,
    // CMDTYP_DEPSTRTXFER will be issued pointing to a Status TRB
}

// Called to remote wake up host when suspended (e.g hid keyboard)
void dcd_remote_wakeup(uint8_t rhport) {
    (void) rhport;
    LOG("%010u >%s", DWT->CYCCNT, __func__);
}

// Connect by enabling internal pull-up resistor on D+/D-
void dcd_connect(uint8_t rhport) {
    (void) rhport;
    udev->dctl_b.run_stop = 1;
}

// Disconnect by disabling internal pull-up resistor on D+/D-
void dcd_disconnect(uint8_t rhport) {
    (void) rhport;

    // TODO: after all transfer completion, get controller into
    // the Set up a Control-Setup TRB / Start Transfer

    // TODO: Issue a DEPENDXFER command for any active transfers

    // Disconnect from the host
    udev->dctl_b.run_stop = 0;
    // TODO: Wait until device controller is halted
    //while(udev->dsts_b.devctrlhlt != 1);
}

// Enable/Disable Start-of-frame interrupt. Default is disabled
void dcd_sof_enable(uint8_t rhport, bool en) {
    (void) rhport, (void) en;
    LOG("%010u >%s", DWT->CYCCNT, __func__);
    // TODO: Check if folowing line is correct
    //udev->devten_b.softevten = 1;       // Enable Start of (micro)frame
}

/// Endpoint Management --------------------------------------------------------

// Invoked when a control transfer's status stage is complete.
// May help DCD to prepare for next control transfer, this API is optional.
void dcd_edpt0_status_complete(uint8_t rhport, tusb_control_request_t const * request) {
    (void) rhport, (void) request;
    _ctrl_long_data = false;
    _dcd_start_xfer(TUSB_DIR_OUT, _ctrl_buf, 8, TRBCTL_CTL_SETUP);
}

// Opening an endpoint is done for all non-control endpoints once the host picks
// a configuration that the device should use.
// At this point, the endpoint should be enabled in the peripheral and
// configured to match the endpoint descriptor.
// Pay special attention to the direction of the endpoint you can get from the
// helper methods above. It will likely change what registers you are setting.
// Also make sure to enable endpoint specific interrupts.
bool dcd_edpt_open(uint8_t rhport, tusb_desc_endpoint_t const * desc_ep) {
    (void) rhport;

    LOG("%010u >%s 0x%x %s %u %u", DWT->CYCCNT, __func__, desc_ep->bEndpointAddress,
        desc_ep->bmAttributes.xfer == TUSB_XFER_BULK ? "bulk" : "int",
        desc_ep->wMaxPacketSize, desc_ep->bInterval);

    if (TUSB_XFER_ISOCHRONOUS == desc_ep->bmAttributes.xfer)
        return false;

    // Controller requires max size requests on OUT endpoints,
    // save MaxPacketSize on EP opening
    if (tu_edpt_dir(desc_ep->bEndpointAddress) == TUSB_DIR_OUT) {
        _ep_dir_out_mps[tu_edpt_number(desc_ep->bEndpointAddress)] = desc_ep->wMaxPacketSize;
    }

    uint8_t ep = (tu_edpt_number(desc_ep->bEndpointAddress) << 1) |
                  tu_edpt_dir(desc_ep->bEndpointAddress);
    // [TODO] verify that the num doesn't exceed hw max

    if (false == _xfer_cfgd) {
        // Endpoint Configuration - Start config phase
        _dcd_cmd_wait(0, CMDTYP_DEPSTARTCFG, 2);
        // check here!! - _dcd_cmd_wait(ep, CMDTYP_DEPSTARTCFG, 2) ??
        _xfer_cfgd = true;
    }

    uint8_t fifo_num = TUSB_DIR_IN == tu_edpt_dir(desc_ep->bEndpointAddress) ?
                        tu_edpt_number(desc_ep->bEndpointAddress) : 0;
    uint8_t interval = desc_ep->bInterval > 0 ? (desc_ep->bInterval - 1) : 0;

    depcfg_params_t *depcfg_params = (depcfg_params_t *)udev->depcmd[ep].params;
    depcfg_params->eptype = desc_ep->bmAttributes.xfer;     // Endpoint type
    depcfg_params->mps = desc_ep->wMaxPacketSize;           // Max Packet Size
    depcfg_params->fifonum = fifo_num;                      // FIFO number
    depcfg_params->brstsiz = 0;                             // Burst Size
    depcfg_params->configaction = DEPCFG_CONFIGACTION_INIT; // Initialize EP state
    depcfg_params->intrnum = 0;                             // Interrupt number
    depcfg_params->xfercmplen = 1;                          // XferComplete Enable
    depcfg_params->xferinprogen = 0;                        // XferInProgress Disable
    depcfg_params->xfernrdyen = 1;                          // XferNotReady Enable
    depcfg_params->streamevten = 0;                         // Stream Event Disable
    depcfg_params->tbrnotupdt = 0;                          // Write back the TRB descriptor
    depcfg_params->ebcen = 0;                               // EBC mode Disable
    depcfg_params->bintervalm1 = interval;                  // bInterval    TODO: 0 in FS mode
    depcfg_params->strmcap = 0;                             // Not Stream Capable
    depcfg_params->epnum = ep;                              // Endpoint Number
    depcfg_params->fifobased = 0;                           // FIXME: ISO endpoint not implemented
    _dcd_cmd_wait(ep, CMDTYP_DEPCFG, 0);

    depxfercfg_params_t *depxfercfg_params = (depxfercfg_params_t *)udev->depcmd[ep].params;
    depxfercfg_params->numxferres = 1;  // Number of Transfer Resources
    _dcd_cmd_wait(ep, CMDTYP_DEPXFERCFG, 0);

    udev->dalepena |= (1 << ep);

    return true;
}

// Close all non-control endpoints, cancel all pending transfers if any.
// Invoked when switching from a non-zero Configuration by SET_CONFIGURE therefore
// required for multiple configuration support.
void dcd_edpt_close_all(uint8_t rhport) {
    (void) rhport;
    //dcd_int_disable(rhport);
    //dcd_int_enable(rhport);
    LOG("%010u >%s", DWT->CYCCNT, __func__);
}

// Close an endpoint. his function is used for implementing alternate settings.
// After calling this, the device should not respond to any packets directed
// towards this endpoint. When called, this function must abort any transfers in
// progress through this endpoint, before returning.
// Implementation is optional. Must be called from the USB task.
// Interrupts could be disabled or enabled during the call.
/**
 * @brief Close (disable) the specified endpoint and abort any in-flight transfers.
 *
 * After this call, the USB controller will no longer accept or send packets
 * on this endpoint. Any pending TRB is canceled, and driver state is reset
 * so no further callbacks occur.
 *
 * Based on Alif E7 USB controller behavior (DALEPENA bit disables the endpoint,
 * DEPENDXFER command aborts an active TRB).
 *
 * @param rhport   Root hub port (unused for single-port controllers)
 * @param ep_addr  TinyUSB endpoint address (logical endpoint + direction)
 */
TU_ATTR_WEAK void dcd_edpt_close(uint8_t rhport, uint8_t ep_addr) {
    (void) rhport, (void) ep_addr;
}
#if 0 // optional implementation
void dcd_edpt_close(uint8_t rhport, uint8_t ep_addr) {
    (void) rhport;

    // Convert TinyUSB endpoint address to physical index:
    // physical_index = (endpoint_number << 1) | direction
    uint8_t ep = (tu_edpt_number(ep_addr) << 1) | tu_edpt_dir(ep_addr);

    // 1) Disable endpoint so hardware ignores further packets
    udev->dalepena &= (1 << ep);

    // 2) If a TRB is still owned by hardware (HWO bit set), abort it:
    if (_xfer_trb[ep][3] & (1U << 0)) {
        // Temporarily allow CMDTYP_DEPENDXFER to take effect
        ugbl->guctl2_b.rst_actbitlater = 1;
        _dcd_cmd_wait(ep, CMDTYP_DEPENDXFER, 0);
        ugbl->guctl2_b.rst_actbitlater = 0;
    }

    // 3) Clear TRB descriptor and reset bookkeeping
    _xfer_trb[ep][3] = 0;   // clear control bits (incl. HWO/IOC)
    _xfer_bytes[ep] = 0;    // no bytes pending

    //LOG_ALIF_INFO("Endpoint %u %s closed",
    //            (unsigned) (ep >> 1),
    //            (ep & 1) ? "IN" : "OUT");
}
#endif

// Submit a transfer, When complete dcd_event_xfer_complete() is invoked to
// notify the stack
bool dcd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t * buffer, uint16_t total_bytes) {

    LOG("%010u >%s %x %x %u", DWT->CYCCNT, __func__, ep_addr, (uint32_t) buffer, total_bytes);

    uint8_t ep = (tu_edpt_number(ep_addr) << 1) | tu_edpt_dir(ep_addr);

    switch (ep) {
        case 0: { // CONTROL OUT
            if (total_bytes > 0) {
                // DATA OUT request
                _xfer_bytes[0] = total_bytes;
                _dcd_start_xfer(0, buffer, total_bytes, TRBCTL_CTL_STAT3);
            } else {
                // TinyUSB explicitly requests STATUS OUT fetch after DATA IN
                if (++_sts_stage == 2) {
                    _sts_stage = 0;
                    dcd_event_xfer_complete(rhport, tu_edpt_addr(0, TUSB_DIR_OUT),
                                          0, XFER_RESULT_SUCCESS, true);
                }
            }
        } break;
        case 1: { // CONTROL IN
            _xfer_bytes[1] = total_bytes;

            if (total_bytes > 0) {
                // DATA IN request
                _dcd_clean_dcache(buffer, total_bytes);
                // Choose TRB type based on long-data flag
                uint8_t type = _ctrl_long_data ? TRBCTL_NORMAL : TRBCTL_CTL_DATA;
                if (total_bytes == 64) {
                    _ctrl_long_data = true;
                }
                _dcd_start_xfer(1, buffer, total_bytes, type);
            } else {
                // status events are handled directly from the ISR when USB
                // controller triggers XferNotReady event for status stage
                if (++_sts_stage == 2) {
                    _sts_stage = 0;
                    dcd_event_xfer_complete(rhport, tu_edpt_addr(0, TUSB_DIR_IN),
                                            0, XFER_RESULT_SUCCESS, true);
                }
                //  dcd_event_xfer_complete(rhport, tu_edpt_addr(0, TUSB_DIR_IN),
                //                          0, XFER_RESULT_SUCCESS, false);
            }
        } break;
        default: { // DATA EPs (BULK & INTERRUPT only)
            if (tu_edpt_dir(ep_addr) == TUSB_DIR_IN) {
                _dcd_clean_dcache(buffer, total_bytes);
            } else {
                // Controller requires max size requests on OUT endpoints
                if (total_bytes < _ep_dir_out_mps[tu_edpt_number(ep_addr)]) {
                    total_bytes = _ep_dir_out_mps[tu_edpt_number(ep_addr)];
                    LOG(">%s ep%u total_bytes = %u", __func__, ep, total_bytes);
                }
            }
            _xfer_bytes[ep] = total_bytes;
            _dcd_start_xfer(ep, buffer, total_bytes,
                            total_bytes ? TRBCTL_NORMAL : TRBCTL_NORMAL_ZLP);
        }
    }

    return true;
}

// Submit a transfer using fifo, When complete dcd_event_xfer_complete() is invoked to notify the stack
// This API is optional, may be useful for register-based for transferring data.
TU_ATTR_WEAK bool dcd_edpt_xfer_fifo(uint8_t rhport, uint8_t ep_addr, tu_fifo_t * ff, uint16_t total_bytes) {
    (void) rhport, (void) ep_addr, (void) ff, (void) total_bytes;
    return true;
}

// Stall endpoint, any queuing transfer should be removed from endpoint
void dcd_edpt_stall(uint8_t rhport, uint8_t ep_addr) {
    (void) rhport;

    uint8_t ep = (tu_edpt_number(ep_addr) << 1) | tu_edpt_dir(ep_addr);
    _dcd_cmd_wait(ep, CMDTYP_DEPSSTALL, 0);

    if (0 == tu_edpt_number(ep_addr)) {
        _ctrl_long_data = false;
        _dcd_start_xfer(TUSB_DIR_OUT, _ctrl_buf, 8, TRBCTL_CTL_SETUP);
    }
}

// clear stall, data toggle is also reset to DATA0
// This API never calls with control endpoints, since it is auto cleared when
// receiving setup packet
void dcd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr) {
    (void) rhport;
    uint8_t ep = (tu_edpt_number(ep_addr) << 1) | tu_edpt_dir(ep_addr);
    _dcd_cmd_wait(ep, CMDTYP_DEPCSTALL, 0);
}

bool dcd_deinit(uint8_t rhport) {
    (void) rhport;
    usb_ctrl2_phy_power_on_reset_set();
    enable_usb_phy_isolation(); // enable usb phy isolation
    disable_usb_phy_power(); // power down usb phy
    disable_usb_periph_clk(); // disable usb peripheral clock
    return true;
}

static uint8_t _dcd_cmd_wait(uint8_t ep, uint8_t typ, uint16_t param) {
    // Store PHY state
    uint32_t phycfg = ugbl->gusb2phycfg0;
    // Disable LPM
    ugbl->gusb2phycfg0_b.enblslpm = 0;
    // Disable suspend
    ugbl->gusb2phycfg0_b.suspendusb20 = 0;

    // Set up command in depcmd register
    udev->depcmd[ep].depcmd_b.cmdtyp = typ;
    udev->depcmd[ep].depcmd_b.cmdioc = 0;
    udev->depcmd[ep].depcmd_b.commandparam = param;

    // Dispatch command and wait for completion
    udev->depcmd[ep].depcmd_b.cmdact = 1;
    while(0 != udev->depcmd[ep].depcmd_b.cmdact);

    // Restore PHY state
    ugbl->gusb2phycfg0 = phycfg;
    // Get command status
    return udev->depcmd[ep].depcmd_b.cmdstatus;
}

/**
 * \brief Handle a DEPEVT (Device Endpoint Event) from the USB event ring.
 *
 * \param ep        Physical endpoint index (0 = EP0 OUT, 1 = EP0 IN, 2+ = other endpoints)
 * \param evt       DEPEVT event type (XFERCOMPLETE, XFERINPROGRESS, XFERNOTREADY, etc.)
 * \param sts       DEPEVT status field (used for “Not Ready” codes or other flags)
 * \param par       DEPEVT parameter field (typically unused for IN/OUT transfers)
 */
static void _dcd_handle_depevt(uint8_t rhport, uint8_t ep, uint8_t evt, uint8_t sts, uint16_t par)
{
    (void) par;
    if (!(ep < 2*TUP_DCD_ENDPOINT_MAX)) {
        TU_MESS_FAILED();
        TU_BREAKPOINT();
        return;
    }

    LOG("%010u DEPEVT ep%u evt%u sts%u", DWT->CYCCNT, ep, evt, sts);

    depevt_sts_t depevt_sts = {.val = sts};

    switch (evt) {
        case DEPEVT_XFERCOMPLETE: {
            LOG("Transfer complete");
            _dcd_invalidate_dcache(_xfer_trb[ep], sizeof(trb_t));
            if (ep == 0) {
                trb_t *trb = (trb_t *)_xfer_trb[0];
                LOG("ep0 xfer trb3 = %08x", _xfer_trb[0][3]);
                if (trb->trbctl == TRBCTL_CTL_SETUP) {
                    _dcd_invalidate_dcache(_ctrl_buf, sizeof(_ctrl_buf));
                    LOG("%02x %02x %02x %02x %02x %02x %02x %02x",
                        _ctrl_buf[0], _ctrl_buf[1], _ctrl_buf[2], _ctrl_buf[3],
                        _ctrl_buf[4], _ctrl_buf[5], _ctrl_buf[6], _ctrl_buf[7]);
                    dcd_event_setup_received(rhport, _ctrl_buf, true);
                } else if (trb->trbctl == TRBCTL_CTL_STAT3) {
                    if (_xfer_bytes[0] > 0) {
                        _dcd_invalidate_dcache((void*) trb->bptrl, _xfer_bytes[0]);
                        dcd_event_xfer_complete(rhport, tu_edpt_addr(0, TUSB_DIR_OUT),
                                                _get_transfered_bytes(0),
                                                XFER_RESULT_SUCCESS, true);
                    } else {
                        if (++_sts_stage == 2) {
                            _sts_stage = 0;
                            dcd_event_xfer_complete(rhport, tu_edpt_addr(0, TUSB_DIR_OUT),
                                                    0, XFER_RESULT_SUCCESS, true);
                        }
                    }
                } else {
                    // invalid TRBCTL value
                    __BKPT(0);
                }
            } else if (ep == 1) {
                trb_t *trb = (trb_t *)_xfer_trb[1];
                LOG("ep1 xfer trb3 = %08x trb2 = %08x", _xfer_trb[1][3], _xfer_trb[1][2]);
                if (trb->trbctl != TRBCTL_CTL_STAT2) { // STATUS IN notification is done at xfer request
                    dcd_event_xfer_complete(rhport, tu_edpt_addr(0, TUSB_DIR_IN),
                                            _get_transfered_bytes(1),
                                            XFER_RESULT_SUCCESS, true);
                } else {
                    if (++_sts_stage == 2) {
                        _sts_stage = 0;
                        dcd_event_xfer_complete(rhport, tu_edpt_addr(0, TUSB_DIR_IN),
                                                0, XFER_RESULT_SUCCESS, true);
                    }
                }
            } else {
                // TODO: check if ep is open
                LOG("ep%u xfer trb3 = %08x trb2 = %08x", ep, _xfer_trb[ep][3], _xfer_trb[ep][2]);
                trb_t *trb = (trb_t *)_xfer_trb[ep];
                if (tu_edpt_dir(tu_edpt_addr(ep >> 1, ep & 1)) == TUSB_DIR_OUT) {
                    _dcd_invalidate_dcache((void*) trb->bptrl, _get_transfered_bytes(ep));
                }
                dcd_event_xfer_complete(TUD_OPT_RHPORT, tu_edpt_addr(ep >> 1, ep & 1),
                                    _get_transfered_bytes(ep),
                                    XFER_RESULT_SUCCESS, true);
            }
        } break;
        case DEPEVT_XFERINPROGRESS: {
            LOG("Transfer in progress");
        } break;
        case DEPEVT_XFERNOTREADY: {
            // Transfer-not-ready indicates endpoint needs re-arming
            LOG("Transfer not ready: %s", sts & 8 ? "no TRB" : "no XFER");
            // XferNotReady NotActive for status stage
            if ((ep == 1) &&
                depevt_sts.xfernotready.stage == DEPEVT_XFERNOTREADY_STS_CTRLSTS) {
                _dcd_start_xfer(1, NULL, 0, TRBCTL_CTL_STAT2);
                break;
            }

            if ((ep == 0) &&
                depevt_sts.xfernotready.stage == DEPEVT_XFERNOTREADY_STS_CTRLSTS) {
                _xfer_bytes[0] = 0;
                _dcd_start_xfer(0, _ctrl_buf, CTRL_BUF_SIZE, TRBCTL_CTL_STAT3);
                break;
            }

            if ((ep < 1) &&
                depevt_sts.xfernotready.active) {
                trb_t *trb = (trb_t *)_xfer_trb[ep];
                if (trb->hwo) { // transfer was configured
                    // dependxfer can only block when actbitlater is set
                    ugbl->guctl2_b.rst_actbitlater = 1;
                    _dcd_cmd_wait(ep, CMDTYP_DEPENDXFER, 0);
                    ugbl->guctl2_b.rst_actbitlater = 0;

                    // Reset the TRB byte count and clean the D-cache
                    _dcd_invalidate_dcache(trb, sizeof(trb_t));
                    trb->bufsiz = _xfer_bytes[ep];
                    _dcd_clean_dcache(trb, sizeof(trb_t));

                    // Prepare EP command
                    depstrtxfer_params_t *depstrtxfer_params = (depstrtxfer_params_t *)udev->depcmd[ep].params;
                    depstrtxfer_params->tdaddrlow = (uint32_t) _dcd_local_to_global(trb);
                    depstrtxfer_params->tdaddrhigh = 0;
                    // Issue the block command
                    _dcd_cmd_wait(ep, CMDTYP_DEPSTRTXFER, 0);
                }
            }
        } break;
        //case DEPEVT_EPCMDCMPLT:// redundant, currently no commands are issued with IOC bit set
        //case DEPEVT_STREAMEVT:
        default:{
            LOG("Unhandled link status change: %d", evt);
        }
    }
}

static void _dcd_handle_devt(uint8_t rhport, uint8_t evt, uint16_t info) {
    LOG("%010u DEVT evt%u info%u", DWT->CYCCNT, evt, info);
    switch (evt) {
        case DEVT_USBRST: {
            LOG("USB reset");

            // TODO: check this flag
            _xfer_cfgd = false;

            // FIXME: Issue CMDTYP_DEPENDXFER for any active transfers (except for EP0)

            // FIXME: Issue CMDTYP_DEPCSTALL for any EP in stall mode (excluding EP0)

            // Set DevAddr to 0
            udev->dcfg_b.devaddr = 0;

            // Set USB speed
            #if CFG_TUD_MAX_SPEED == OPT_MODE_DEFAULT_SPEED || \
                CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED
                udev->dcfg_b.devspd = DCFG_DEVSPD_HS;
                dcd_event_bus_reset(rhport, TUSB_SPEED_HIGH, true);
            #elif CFG_TUD_MAX_SPEED == OPT_MODE_FULL_SPEED
                udev->dcfg_b.devspd = DCFG_DEVSPD_FS;
                dcd_event_bus_reset(rhport, TUSB_SPEED_FULL, true);
            #else
                #error "Alif USB supports full/high speed only."
            #endif
        } break;
        case DEVT_CONNECTDONE: {
            LOG("Connect Done");

            // Check connection speed
            #if CFG_TUD_MAX_SPEED == OPT_MODE_DEFAULT_SPEED || \
                CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED
            if (udev->dsts_b.connectspd != DSTS_CONNECTSPD_HS) {
                LOG("Wrong device speed [%d], expected HS", udev->dsts_b.connectspd);
            }
            #elif CFG_TUD_MAX_SPEED == OPT_MODE_FULL_SPEED
            if (udev->dsts_b.connectspd != DSTS_CONNECTSPD_FS) {
                LOG("Wrong device speed [%d], expected FS", udev->dsts_b.connectspd);
            }
            #endif

            // NOTE: After USB reset RAMClkSel (GCTL) is set to AHB bus clock

            // Endpoint Configuration - EP0 OUT
            // NOTE: Using the same EP characteristics from Power-On Reset,
            // but MaxPacketSize should be 64
            depcfg_params_t *depcfg_params = (depcfg_params_t *)udev->depcmd[0].params;
            depcfg_params->eptype = 0;                                  // Control Endpoint
            depcfg_params->mps = MAX_PACKET_SIZE_ON_USB_RESET;          // Max Packet Size
            depcfg_params->fifonum = 0;                                 // FIFO number
            depcfg_params->brstsiz = 0;                                 // Burst Size
            depcfg_params->configaction = DEPCFG_CONFIGACTION_MODIFY;   // Modify EP state
            depcfg_params->intrnum = 0;                                 // Interrupt number
            depcfg_params->xfercmplen = 1;                              // XferComplete Enable
            depcfg_params->xferinprogen = 0;                            // XferInProgress Disable
            depcfg_params->xfernrdyen = 1;                              // XferNotReady Enable
            depcfg_params->streamevten = 0;                             // Stream Event Disable
            depcfg_params->tbrnotupdt = 0;                              // Write back the TRB descriptor
            depcfg_params->ebcen = 0;                                   // EBC mode Disable
            depcfg_params->bintervalm1 = 0;                             // bInterval
            depcfg_params->strmcap = 0;                                 // Not Stream Capable
            depcfg_params->epnum = PHY_EP0;                             // OUT endpoint
            depcfg_params->fifobased = 0;                               // Not FIFO based
            _dcd_cmd_wait(PHY_EP0, CMDTYP_DEPCFG, 0);

            // Endpoint Configuration - EP0 IN
            depcfg_params = (depcfg_params_t *)udev->depcmd[1].params;
            depcfg_params->eptype = 0;                                  // Control Endpoint
            depcfg_params->mps = MAX_PACKET_SIZE_ON_USB_RESET;          // Max Packet Size
            depcfg_params->fifonum = 0;                                 // FIFO number
            depcfg_params->brstsiz = 0;                                 // Burst Size
            depcfg_params->configaction = DEPCFG_CONFIGACTION_MODIFY;   // Modify EP state
            depcfg_params->intrnum = 0;                                 // Interrupt number
            depcfg_params->xfercmplen = 1;                              // XferComplete Enable
            depcfg_params->xferinprogen = 0;                            // XferInProgress Disable
            depcfg_params->xfernrdyen = 1;                              // XferNotReady Enable
            depcfg_params->streamevten = 0;                             // Stream Event Disable
            depcfg_params->tbrnotupdt = 0;                              // Write back the TRB descriptor
            depcfg_params->ebcen = 0;                                   // EBC mode Disable
            depcfg_params->bintervalm1 = 0;                             // bInterval
            depcfg_params->strmcap = 0;                                 // Not Stream Capable
            depcfg_params->epnum = PHY_EP1;                             // IN endpoint
            depcfg_params->fifobased = 0;                               // Not FIFO based
            _dcd_cmd_wait(PHY_EP1, CMDTYP_DEPCFG, 0);

            // NOTE: (optional) Based on new MaxPacketSize of EP0 IN,
            // TX FIFO size could be re-allocated by writing to GTXFIFOSIZn
        } break;
        case DEVT_ULSTCHNG: {
            LOG("Link status change");
            switch (info & 0x0F) {
                case DEVT_ULSTCHNG_L2SUSP: { // suspend (L2)
                    dcd_event_bus_signal(rhport, DCD_EVENT_SUSPEND, true);
                } break;
                case DEVT_ULSTCHNG_DISCONNECTED: { // disconnected
                    dcd_event_bus_signal(rhport, DCD_EVENT_UNPLUGGED, true);
                } break;
                case DEVT_ULSTCHNG_RESUME: { // resume
                    dcd_event_bus_signal(rhport, DCD_EVENT_RESUME, true);
                } break;
                //case DEVT_ULSTCHNG_ON:
                //case DEVT_ULSTCHNG_L1SLP:
                //case DEVT_ULSTCHNG_EARLYSUSP:
                //case DEVT_ULSTCHNG_RESET:
                default: {
                    LOG("Unhandled link status change: %d", info & 0x0F);
                }
          }
        } break;
        case DEVT_ERRTICERR: {
            LOG("Erratic Error Event");
            __BKPT(0);
        } break;
        //case DEVT_DISCONNEVT: {
        //  //TODO: check if it's correct
        //  udev->dctl_b.ulstchngreq = DEVT_ULSTCHNG_EARLYSUSP;
        //} break;
        //case DEVT_HIBRQ:
        //case DEVT_USBSUSP:
        //case DEVT_SOF:
        //case DEVT_L1SUSP:
        //case DEVT_CMDCMPLT:
        //case DEVT_EVNTOVERFLOW:
        //case DEVT_VNDDEVTSTRCVED:
        //case DEVT_L1RESM:
        //case DEVT_ECCERR:
        default: {
            LOG("Unhandled DEVT event: %d", evt);
        }
    }
}

//! \brief Program a Transfer Request Block (TRB) and kick off a transfer.
//! \param ep    Physical endpoint index (0–31)
//! \param buf   Pointer to the data buffer (local address)
//! \param size  Number of bytes to transfer
//! \param type  TRBCTL_* type (e.g. TRBCTL_NORMAL, TRBCTL_CTL_DATA, etc.)
//! \return      Status returned by _dcd_cmd_wait (0 = success)
static uint8_t _dcd_start_xfer(uint8_t ep, void* buf, uint32_t size, uint8_t type) {
    // Prevent race conditions between programming TRB and ISR handling
    dcd_int_disable(TUD_OPT_RHPORT);

    // Populate the TRB fields
    trb_t *trb = (trb_t *)_xfer_trb[ep];
    memset(trb, 0, sizeof(trb_t));
    trb->bptrl = buf ? (uint32_t) _dcd_local_to_global(buf) : 0U;
    trb->bptrh = 0;
    trb->bufsiz = size;               // Transfer length
    trb->intcmpl = 1;                 // Interrupt on Complete
    trb->ispimi = 1;                  // Interrupt on Short Packet
    trb->trbctl = type;               // TRB type
    trb->lst = 1;                     // Last TRB
    trb->hwo = 1;                     // Set HWO to create TRB
    // Clean D-cache so USB controller sees updated TRB
    _dcd_clean_dcache(trb, sizeof(trb_t));

    // Prepare EP command
    depstrtxfer_params_t *depstrtxfer_params = (depstrtxfer_params_t *)udev->depcmd[ep].params;
    depstrtxfer_params->tdaddrlow = (uint32_t) _dcd_local_to_global(trb);
    depstrtxfer_params->tdaddrhigh = 0;

    // Re-enable USB interrupt before issuing command
    dcd_int_enable(TUD_OPT_RHPORT);

    // Issue the block command and pass the status
    return _dcd_cmd_wait(ep, CMDTYP_DEPSTRTXFER, 0);
}

#endif

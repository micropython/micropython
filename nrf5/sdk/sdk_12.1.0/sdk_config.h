

#ifndef SDK_CONFIG_H
#define SDK_CONFIG_H
// <<< Use Configuration Wizard in Context Menu >>>\n
#ifdef USE_APP_CONFIG
#include "app_config.h"
#endif
// <h> nRF_BLE 

//==========================================================
// <q> BLE_ADVERTISING_ENABLED  - ble_advertising - Advertising module
 

#ifndef BLE_ADVERTISING_ENABLED
#define BLE_ADVERTISING_ENABLED 1
#endif

// <q> BLE_DTM_ENABLED  - ble_dtm - Module for testing RF/PHY using DTM commands
 

#ifndef BLE_DTM_ENABLED
#define BLE_DTM_ENABLED 0
#endif

// <q> BLE_RACP_ENABLED  - ble_racp - Record Access Control Point library
 

#ifndef BLE_RACP_ENABLED
#define BLE_RACP_ENABLED 0
#endif

// <q> NRF_BLE_QWR_ENABLED  - nrf_ble_qwr - Queued writes support module (prepare/execute write)
 

#ifndef NRF_BLE_QWR_ENABLED
#define NRF_BLE_QWR_ENABLED 0
#endif

// <q> PEER_MANAGER_ENABLED  - peer_manager - Peer Manager
 

#ifndef PEER_MANAGER_ENABLED
#define PEER_MANAGER_ENABLED 1
#endif

// </h> 
//==========================================================

// <h> nRF_BLE_Services 

//==========================================================
// <q> BLE_ANCS_C_ENABLED  - ble_ancs_c - Apple Notification Service Client
 

#ifndef BLE_ANCS_C_ENABLED
#define BLE_ANCS_C_ENABLED 0
#endif

// <q> BLE_ANS_C_ENABLED  - ble_ans_c - Alert Notification Service Client
 

#ifndef BLE_ANS_C_ENABLED
#define BLE_ANS_C_ENABLED 0
#endif

// <q> BLE_BAS_C_ENABLED  - ble_bas_c - Battery Service Client
 

#ifndef BLE_BAS_C_ENABLED
#define BLE_BAS_C_ENABLED 0
#endif

// <q> BLE_BAS_ENABLED  - ble_bas - Battery Service
 

#ifndef BLE_BAS_ENABLED
#define BLE_BAS_ENABLED 0
#endif

// <q> BLE_CSCS_ENABLED  - ble_cscs - Cycling Speed and Cadence Service
 

#ifndef BLE_CSCS_ENABLED
#define BLE_CSCS_ENABLED 0
#endif

// <q> BLE_CTS_C_ENABLED  - ble_cts_c - Current Time Service Client
 

#ifndef BLE_CTS_C_ENABLED
#define BLE_CTS_C_ENABLED 0
#endif

// <q> BLE_DIS_ENABLED  - ble_dis - Device Information Service
 

#ifndef BLE_DIS_ENABLED
#define BLE_DIS_ENABLED 0
#endif

// <q> BLE_GLS_ENABLED  - ble_gls - Glucose Service
 

#ifndef BLE_GLS_ENABLED
#define BLE_GLS_ENABLED 0
#endif

// <q> BLE_HIDS_ENABLED  - ble_hids - Human Interface Device Service
 

#ifndef BLE_HIDS_ENABLED
#define BLE_HIDS_ENABLED 0
#endif

// <q> BLE_HRS_C_ENABLED  - ble_hrs_c - Heart Rate Service Client
 

#ifndef BLE_HRS_C_ENABLED
#define BLE_HRS_C_ENABLED 0
#endif

// <q> BLE_HRS_ENABLED  - ble_hrs - Heart Rate Service
 

#ifndef BLE_HRS_ENABLED
#define BLE_HRS_ENABLED 0
#endif

// <q> BLE_HTS_ENABLED  - ble_hts - Health Thermometer Service
 

#ifndef BLE_HTS_ENABLED
#define BLE_HTS_ENABLED 0
#endif

// <q> BLE_IAS_C_ENABLED  - ble_ias_c - Immediate Alert Service Client
 

#ifndef BLE_IAS_C_ENABLED
#define BLE_IAS_C_ENABLED 0
#endif

// <q> BLE_IAS_ENABLED  - ble_ias - Immediate Alert Service
 

#ifndef BLE_IAS_ENABLED
#define BLE_IAS_ENABLED 0
#endif

// <q> BLE_LBS_C_ENABLED  - ble_lbs_c - Nordic LED Button Service Client
 

#ifndef BLE_LBS_C_ENABLED
#define BLE_LBS_C_ENABLED 0
#endif

// <q> BLE_LBS_ENABLED  - ble_lbs - LED Button Service
 

#ifndef BLE_LBS_ENABLED
#define BLE_LBS_ENABLED 0
#endif

// <q> BLE_LLS_ENABLED  - ble_lls - Link Loss Service
 

#ifndef BLE_LLS_ENABLED
#define BLE_LLS_ENABLED 0
#endif

// <q> BLE_NUS_C_ENABLED  - ble_nus_c - Nordic UART Central Service
 

#ifndef BLE_NUS_C_ENABLED
#define BLE_NUS_C_ENABLED 0
#endif

// <q> BLE_NUS_ENABLED  - ble_nus - Nordic UART Service
 

#ifndef BLE_NUS_ENABLED
#define BLE_NUS_ENABLED 1
#endif

// <q> BLE_RSCS_C_ENABLED  - ble_rscs_c - Running Speed and Cadence Client
 

#ifndef BLE_RSCS_C_ENABLED
#define BLE_RSCS_C_ENABLED 0
#endif

// <q> BLE_RSCS_ENABLED  - ble_rscs - Running Speed and Cadence Service
 

#ifndef BLE_RSCS_ENABLED
#define BLE_RSCS_ENABLED 0
#endif

// <q> BLE_TPS_ENABLED  - ble_tps - TX Power Service
 

#ifndef BLE_TPS_ENABLED
#define BLE_TPS_ENABLED 0
#endif

// </h> 
//==========================================================

// <h> nRF_Drivers 

//==========================================================
// <e> ADC_ENABLED - nrf_drv_adc - Driver for ADC peripheral (nRF51)
//==========================================================
#ifndef ADC_ENABLED
#define ADC_ENABLED 0
#endif
#if  ADC_ENABLED
// <o> ADC_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef ADC_CONFIG_IRQ_PRIORITY
#define ADC_CONFIG_IRQ_PRIORITY 6
#endif

#endif //ADC_ENABLED
// </e>

// <e> CLOCK_ENABLED - nrf_drv_clock - CLOCK peripheral driver
//==========================================================
#ifndef CLOCK_ENABLED
#define CLOCK_ENABLED 1
#endif
#if  CLOCK_ENABLED
// <o> CLOCK_CONFIG_XTAL_FREQ  - HF XTAL Frequency
 
// <0=> Default (64 MHz) 

#ifndef CLOCK_CONFIG_XTAL_FREQ
#define CLOCK_CONFIG_XTAL_FREQ 0
#endif

// <o> CLOCK_CONFIG_LF_SRC  - LF Clock Source
 
// <0=> RC 
// <1=> XTAL 
// <2=> Synth 

#ifndef CLOCK_CONFIG_LF_SRC
#define CLOCK_CONFIG_LF_SRC 1
#endif

// <o> CLOCK_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef CLOCK_CONFIG_IRQ_PRIORITY
#define CLOCK_CONFIG_IRQ_PRIORITY 6
#endif

#endif //CLOCK_ENABLED
// </e>

// <e> COMP_ENABLED - nrf_drv_comp - COMP peripheral driver
//==========================================================
#ifndef COMP_ENABLED
#define COMP_ENABLED 0
#endif
#if  COMP_ENABLED
// <o> COMP_CONFIG_REF  - Reference voltage
 
// <0=> Internal 1.2V 
// <1=> Internal 1.8V 
// <2=> Internal 2.4V 
// <4=> VDD 
// <7=> ARef 

#ifndef COMP_CONFIG_REF
#define COMP_CONFIG_REF 1
#endif

// <o> COMP_CONFIG_MAIN_MODE  - Main mode
 
// <0=> Single ended 
// <1=> Differential 

#ifndef COMP_CONFIG_MAIN_MODE
#define COMP_CONFIG_MAIN_MODE 0
#endif

// <o> COMP_CONFIG_SPEED_MODE  - Speed mode
 
// <0=> Low power 
// <1=> Normal 
// <2=> High speed 

#ifndef COMP_CONFIG_SPEED_MODE
#define COMP_CONFIG_SPEED_MODE 2
#endif

// <o> COMP_CONFIG_HYST  - Hystheresis
 
// <0=> No 
// <1=> 50mV 

#ifndef COMP_CONFIG_HYST
#define COMP_CONFIG_HYST 0
#endif

// <o> COMP_CONFIG_ISOURCE  - Current Source
 
// <0=> Off 
// <1=> 2.5 uA 
// <2=> 5 uA 
// <3=> 10 uA 

#ifndef COMP_CONFIG_ISOURCE
#define COMP_CONFIG_ISOURCE 0
#endif

// <o> COMP_CONFIG_INPUT  - Analog input
 
// <0=> 0 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef COMP_CONFIG_INPUT
#define COMP_CONFIG_INPUT 0
#endif

// <o> COMP_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef COMP_CONFIG_IRQ_PRIORITY
#define COMP_CONFIG_IRQ_PRIORITY 6
#endif

#endif //COMP_ENABLED
// </e>

// <q> EGU_ENABLED  - nrf_drv_swi - SWI(EGU) peripheral driver
 

#ifndef EGU_ENABLED
#define EGU_ENABLED 0
#endif

// <e> GPIOTE_ENABLED - nrf_drv_gpiote - GPIOTE peripheral driver
//==========================================================
#ifndef GPIOTE_ENABLED
#define GPIOTE_ENABLED 1
#endif
#if  GPIOTE_ENABLED
// <o> GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS - Number of lower power input pins 
#ifndef GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS
#define GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 4
#endif

// <o> GPIOTE_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef GPIOTE_CONFIG_IRQ_PRIORITY
#define GPIOTE_CONFIG_IRQ_PRIORITY 6
#endif

#endif //GPIOTE_ENABLED
// </e>

// <e> I2S_ENABLED - nrf_drv_i2s - I2S peripheral driver
//==========================================================
#ifndef I2S_ENABLED
#define I2S_ENABLED 0
#endif
#if  I2S_ENABLED
// <o> I2S_CONFIG_SCK_PIN - SCK pin  <0-31> 


#ifndef I2S_CONFIG_SCK_PIN
#define I2S_CONFIG_SCK_PIN 31
#endif

// <o> I2S_CONFIG_LRCK_PIN - LRCK pin  <1-31> 


#ifndef I2S_CONFIG_LRCK_PIN
#define I2S_CONFIG_LRCK_PIN 30
#endif

// <o> I2S_CONFIG_MCK_PIN - MCK pin 
#ifndef I2S_CONFIG_MCK_PIN
#define I2S_CONFIG_MCK_PIN 255
#endif

// <o> I2S_CONFIG_SDOUT_PIN - SDOUT pin  <0-31> 


#ifndef I2S_CONFIG_SDOUT_PIN
#define I2S_CONFIG_SDOUT_PIN 29
#endif

// <o> I2S_CONFIG_SDIN_PIN - SDIN pin  <0-31> 


#ifndef I2S_CONFIG_SDIN_PIN
#define I2S_CONFIG_SDIN_PIN 28
#endif

// <o> I2S_CONFIG_MASTER  - Mode
 
// <0=> Master 
// <1=> Slave 

#ifndef I2S_CONFIG_MASTER
#define I2S_CONFIG_MASTER 0
#endif

// <o> I2S_CONFIG_FORMAT  - Format
 
// <0=> I2S 
// <1=> Aligned 

#ifndef I2S_CONFIG_FORMAT
#define I2S_CONFIG_FORMAT 0
#endif

// <o> I2S_CONFIG_ALIGN  - Alignment
 
// <0=> Left 
// <1=> Right 

#ifndef I2S_CONFIG_ALIGN
#define I2S_CONFIG_ALIGN 0
#endif

// <o> I2S_CONFIG_SWIDTH  - Sample width (bits)
 
// <0=> 8 
// <1=> 16 
// <2=> 24 

#ifndef I2S_CONFIG_SWIDTH
#define I2S_CONFIG_SWIDTH 1
#endif

// <o> I2S_CONFIG_CHANNELS  - Channels
 
// <0=> Stereo 
// <1=> Left 
// <2=> Right 

#ifndef I2S_CONFIG_CHANNELS
#define I2S_CONFIG_CHANNELS 1
#endif

// <o> I2S_CONFIG_MCK_SETUP  - MCK behavior
 
// <0=> Disabled 
// <2147483648=> 32MHz/2 
// <1342177280=> 32MHz/3 
// <1073741824=> 32MHz/4 
// <805306368=> 32MHz/5 
// <671088640=> 32MHz/6 
// <536870912=> 32MHz/8 
// <402653184=> 32MHz/10 
// <369098752=> 32MHz/11 
// <285212672=> 32MHz/15 
// <268435456=> 32MHz/16 
// <201326592=> 32MHz/21 
// <184549376=> 32MHz/23 
// <142606336=> 32MHz/30 
// <138412032=> 32MHz/31 
// <134217728=> 32MHz/32 
// <100663296=> 32MHz/42 
// <68157440=> 32MHz/63 
// <34340864=> 32MHz/125 

#ifndef I2S_CONFIG_MCK_SETUP
#define I2S_CONFIG_MCK_SETUP 536870912
#endif

// <o> I2S_CONFIG_RATIO  - MCK/LRCK ratio
 
// <0=> 32x 
// <1=> 48x 
// <2=> 64x 
// <3=> 96x 
// <4=> 128x 
// <5=> 192x 
// <6=> 256x 
// <7=> 384x 
// <8=> 512x 

#ifndef I2S_CONFIG_RATIO
#define I2S_CONFIG_RATIO 2000
#endif

// <o> I2S_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef I2S_CONFIG_IRQ_PRIORITY
#define I2S_CONFIG_IRQ_PRIORITY 6
#endif

#endif //I2S_ENABLED
// </e>

// <e> LPCOMP_ENABLED - nrf_drv_lpcomp - LPCOMP peripheral driver
//==========================================================
#ifndef LPCOMP_ENABLED
#define LPCOMP_ENABLED 0
#endif
#if  LPCOMP_ENABLED
// <o> LPCOMP_CONFIG_REFERENCE  - Reference voltage
 
// <0=> Supply 1/8 
// <1=> Supply 2/8 
// <2=> Supply 3/8 
// <3=> Supply 4/8 
// <4=> Supply 5/8 
// <5=> Supply 6/8 
// <6=> Supply 7/8 
// <8=> Supply 1/16 (nRF52) 
// <9=> Supply 3/16 (nRF52) 
// <10=> Supply 5/16 (nRF52) 
// <11=> Supply 7/16 (nRF52) 
// <12=> Supply 9/16 (nRF52) 
// <13=> Supply 11/16 (nRF52) 
// <14=> Supply 13/16 (nRF52) 
// <15=> Supply 15/16 (nRF52) 
// <7=> External Ref 0 
// <65543=> External Ref 1 

#ifndef LPCOMP_CONFIG_REFERENCE
#define LPCOMP_CONFIG_REFERENCE 3
#endif

// <o> LPCOMP_CONFIG_DETECTION  - Detection
 
// <0=> Crossing 
// <1=> Up 
// <2=> Down 

#ifndef LPCOMP_CONFIG_DETECTION
#define LPCOMP_CONFIG_DETECTION 2
#endif

// <o> LPCOMP_CONFIG_INPUT  - Analog input
 
// <0=> 0 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef LPCOMP_CONFIG_INPUT
#define LPCOMP_CONFIG_INPUT 0
#endif

// <o> LPCOMP_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef LPCOMP_CONFIG_IRQ_PRIORITY
#define LPCOMP_CONFIG_IRQ_PRIORITY 6
#endif

#endif //LPCOMP_ENABLED
// </e>

// <e> PDM_ENABLED - nrf_drv_pdm - PDM peripheral driver
//==========================================================
#ifndef PDM_ENABLED
#define PDM_ENABLED 0
#endif
#if  PDM_ENABLED
// <o> PDM_CONFIG_MODE  - Mode
 
// <0=> Stereo 
// <1=> Mono 

#ifndef PDM_CONFIG_MODE
#define PDM_CONFIG_MODE 1
#endif

// <o> PDM_CONFIG_EDGE  - Edge
 
// <0=> Left falling 
// <1=> Left rising 

#ifndef PDM_CONFIG_EDGE
#define PDM_CONFIG_EDGE 0
#endif

// <o> PDM_CONFIG_CLOCK_FREQ  - Clock frequency
 
// <134217728=> 1000k 
// <138412032=> 1032k (default) 
// <142606336=> 1067k 

#ifndef PDM_CONFIG_CLOCK_FREQ
#define PDM_CONFIG_CLOCK_FREQ 138412032
#endif

// <o> PDM_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef PDM_CONFIG_IRQ_PRIORITY
#define PDM_CONFIG_IRQ_PRIORITY 6
#endif

#endif //PDM_ENABLED
// </e>

// <q> PERIPHERAL_RESOURCE_SHARING_ENABLED  - nrf_drv_common - Peripheral drivers common module
 

#ifndef PERIPHERAL_RESOURCE_SHARING_ENABLED
#define PERIPHERAL_RESOURCE_SHARING_ENABLED 0
#endif

// <q> PPI_ENABLED  - nrf_drv_ppi - PPI peripheral driver
 

#ifndef PPI_ENABLED
#define PPI_ENABLED 0
#endif

// <e> PWM_ENABLED - nrf_drv_pwm - PWM peripheral driver
//==========================================================
#ifndef PWM_ENABLED
#define PWM_ENABLED 0
#endif
#if  PWM_ENABLED
// <o> PWM_DEFAULT_CONFIG_OUT0_PIN - Out0 pin  <0-31> 


#ifndef PWM_DEFAULT_CONFIG_OUT0_PIN
#define PWM_DEFAULT_CONFIG_OUT0_PIN 31
#endif

// <o> PWM_DEFAULT_CONFIG_OUT1_PIN - Out1 pin  <0-31> 


#ifndef PWM_DEFAULT_CONFIG_OUT1_PIN
#define PWM_DEFAULT_CONFIG_OUT1_PIN 31
#endif

// <o> PWM_DEFAULT_CONFIG_OUT2_PIN - Out2 pin  <0-31> 


#ifndef PWM_DEFAULT_CONFIG_OUT2_PIN
#define PWM_DEFAULT_CONFIG_OUT2_PIN 31
#endif

// <o> PWM_DEFAULT_CONFIG_OUT3_PIN - Out3 pin  <0-31> 


#ifndef PWM_DEFAULT_CONFIG_OUT3_PIN
#define PWM_DEFAULT_CONFIG_OUT3_PIN 31
#endif

// <o> PWM_DEFAULT_CONFIG_BASE_CLOCK  - Base clock
 
// <0=> 16 MHz 
// <1=> 8 MHz 
// <2=> 4 MHz 
// <3=> 2 MHz 
// <4=> 1 MHz 
// <5=> 500 kHz 
// <6=> 250 kHz 
// <7=> 125 MHz 

#ifndef PWM_DEFAULT_CONFIG_BASE_CLOCK
#define PWM_DEFAULT_CONFIG_BASE_CLOCK 4
#endif

// <o> PWM_DEFAULT_CONFIG_COUNT_MODE  - Count mode
 
// <0=> Up 
// <1=> Up and Down 

#ifndef PWM_DEFAULT_CONFIG_COUNT_MODE
#define PWM_DEFAULT_CONFIG_COUNT_MODE 0
#endif

// <o> PWM_DEFAULT_CONFIG_TOP_VALUE - Top value 
#ifndef PWM_DEFAULT_CONFIG_TOP_VALUE
#define PWM_DEFAULT_CONFIG_TOP_VALUE 1000
#endif

// <o> PWM_DEFAULT_CONFIG_LOAD_MODE  - Load mode
 
// <0=> Common 
// <1=> Grouped 
// <2=> Individual 
// <3=> Waveform 

#ifndef PWM_DEFAULT_CONFIG_LOAD_MODE
#define PWM_DEFAULT_CONFIG_LOAD_MODE 0
#endif

// <o> PWM_DEFAULT_CONFIG_STEP_MODE  - Step mode
 
// <0=> Auto 
// <1=> Triggered 

#ifndef PWM_DEFAULT_CONFIG_STEP_MODE
#define PWM_DEFAULT_CONFIG_STEP_MODE 0
#endif

// <o> PWM_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef PWM_DEFAULT_CONFIG_IRQ_PRIORITY
#define PWM_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <q> PWM0_ENABLED  - Enable PWM0 instance
 

#ifndef PWM0_ENABLED
#define PWM0_ENABLED 0
#endif

// <q> PWM1_ENABLED  - Enable PWM1 instance
 

#ifndef PWM1_ENABLED
#define PWM1_ENABLED 0
#endif

// <q> PWM2_ENABLED  - Enable PWM2 instance
 

#ifndef PWM2_ENABLED
#define PWM2_ENABLED 0
#endif

#endif //PWM_ENABLED
// </e>

// <e> QDEC_ENABLED - nrf_drv_qdec - QDEC peripheral driver
//==========================================================
#ifndef QDEC_ENABLED
#define QDEC_ENABLED 0
#endif
#if  QDEC_ENABLED
// <o> QDEC_CONFIG_REPORTPER  - Report period
 
// <0=> 10 Samples 
// <1=> 40 Samples 
// <2=> 80 Samples 
// <3=> 120 Samples 
// <4=> 160 Samples 
// <5=> 200 Samples 
// <6=> 240 Samples 
// <7=> 280 Samples 

#ifndef QDEC_CONFIG_REPORTPER
#define QDEC_CONFIG_REPORTPER 0
#endif

// <o> QDEC_CONFIG_SAMPLEPER  - Sample period
 
// <0=> 128 us 
// <1=> 256 us 
// <2=> 512 us 
// <3=> 1024 us 
// <4=> 2048 us 
// <5=> 4096 us 
// <6=> 8192 us 
// <7=> 16384 us 

#ifndef QDEC_CONFIG_SAMPLEPER
#define QDEC_CONFIG_SAMPLEPER 7
#endif

// <o> QDEC_CONFIG_PIO_A - A pin  <0-31> 


#ifndef QDEC_CONFIG_PIO_A
#define QDEC_CONFIG_PIO_A 31
#endif

// <o> QDEC_CONFIG_PIO_B - B pin  <0-31> 


#ifndef QDEC_CONFIG_PIO_B
#define QDEC_CONFIG_PIO_B 31
#endif

// <o> QDEC_CONFIG_PIO_LED - LED pin  <0-31> 


#ifndef QDEC_CONFIG_PIO_LED
#define QDEC_CONFIG_PIO_LED 31
#endif

// <o> QDEC_CONFIG_LEDPRE - LED pre 
#ifndef QDEC_CONFIG_LEDPRE
#define QDEC_CONFIG_LEDPRE 511
#endif

// <o> QDEC_CONFIG_LEDPOL  - LED polarity
 
// <0=> Active low 
// <1=> Active high 

#ifndef QDEC_CONFIG_LEDPOL
#define QDEC_CONFIG_LEDPOL 1
#endif

// <q> QDEC_CONFIG_DBFEN  - Debouncing enable
 

#ifndef QDEC_CONFIG_DBFEN
#define QDEC_CONFIG_DBFEN 0
#endif

// <q> QDEC_CONFIG_SAMPLE_INTEN  - Sample ready interrupt enable
 

#ifndef QDEC_CONFIG_SAMPLE_INTEN
#define QDEC_CONFIG_SAMPLE_INTEN 0
#endif

// <o> QDEC_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef QDEC_CONFIG_IRQ_PRIORITY
#define QDEC_CONFIG_IRQ_PRIORITY 6
#endif

#endif //QDEC_ENABLED
// </e>

// <e> RNG_ENABLED - nrf_drv_rng - RNG peripheral driver
//==========================================================
#ifndef RNG_ENABLED
#define RNG_ENABLED 0
#endif
#if  RNG_ENABLED
// <q> RNG_CONFIG_ERROR_CORRECTION  - Error correction
 

#ifndef RNG_CONFIG_ERROR_CORRECTION
#define RNG_CONFIG_ERROR_CORRECTION 0
#endif

// <o> RNG_CONFIG_POOL_SIZE - Pool size 
#ifndef RNG_CONFIG_POOL_SIZE
#define RNG_CONFIG_POOL_SIZE 8
#endif

// <o> RNG_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef RNG_CONFIG_IRQ_PRIORITY
#define RNG_CONFIG_IRQ_PRIORITY 6
#endif

#endif //RNG_ENABLED
// </e>

// <e> RTC_ENABLED - nrf_drv_rtc - RTC peripheral driver
//==========================================================
#ifndef RTC_ENABLED
#define RTC_ENABLED 0
#endif
#if  RTC_ENABLED
// <o> RTC_DEFAULT_CONFIG_FREQUENCY - Frequency  <16-32768> 


#ifndef RTC_DEFAULT_CONFIG_FREQUENCY
#define RTC_DEFAULT_CONFIG_FREQUENCY 32768
#endif

// <q> RTC_DEFAULT_CONFIG_RELIABLE  - Ensures safe compare event triggering
 

#ifndef RTC_DEFAULT_CONFIG_RELIABLE
#define RTC_DEFAULT_CONFIG_RELIABLE 0
#endif

// <o> RTC_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef RTC_DEFAULT_CONFIG_IRQ_PRIORITY
#define RTC_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <q> RTC0_ENABLED  - Enable RTC0 instance
 

#ifndef RTC0_ENABLED
#define RTC0_ENABLED 0
#endif

// <q> RTC1_ENABLED  - Enable RTC1 instance
 

#ifndef RTC1_ENABLED
#define RTC1_ENABLED 0
#endif

// <q> RTC2_ENABLED  - Enable RTC2 instance
 

#ifndef RTC2_ENABLED
#define RTC2_ENABLED 0
#endif

// <o> NRF_MAXIMUM_LATENCY_US - Maximum possible time[us] in highest priority interrupt 
#ifndef NRF_MAXIMUM_LATENCY_US
#define NRF_MAXIMUM_LATENCY_US 2000
#endif

#endif //RTC_ENABLED
// </e>

// <e> SAADC_ENABLED - nrf_drv_saadc - SAADC peripheral driver
//==========================================================
#ifndef SAADC_ENABLED
#define SAADC_ENABLED 0
#endif
#if  SAADC_ENABLED
// <o> SAADC_CONFIG_RESOLUTION  - Resolution
 
// <0=> 8 bit 
// <1=> 10 bit 
// <2=> 12 bit 
// <3=> 14 bit 

#ifndef SAADC_CONFIG_RESOLUTION
#define SAADC_CONFIG_RESOLUTION 1
#endif

// <o> SAADC_CONFIG_OVERSAMPLE  - Sample period
 
// <0=> Disabled 
// <1=> 2x 
// <2=> 4x 
// <3=> 8x 
// <4=> 16x 
// <5=> 32x 
// <6=> 64x 
// <7=> 128x 
// <8=> 256x 

#ifndef SAADC_CONFIG_OVERSAMPLE
#define SAADC_CONFIG_OVERSAMPLE 0
#endif

// <q> SAADC_CONFIG_LP_MODE  - Enabling low power mode
 

#ifndef SAADC_CONFIG_LP_MODE
#define SAADC_CONFIG_LP_MODE 0
#endif

// <o> SAADC_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef SAADC_CONFIG_IRQ_PRIORITY
#define SAADC_CONFIG_IRQ_PRIORITY 6
#endif

#endif //SAADC_ENABLED
// </e>

// <e> SPIS_ENABLED - nrf_drv_spis - SPI Slave driver
//==========================================================
#ifndef SPIS_ENABLED
#define SPIS_ENABLED 0
#endif
#if  SPIS_ENABLED
// <o> SPIS_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef SPIS_DEFAULT_CONFIG_IRQ_PRIORITY
#define SPIS_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <o> SPIS_DEFAULT_MODE  - Mode
 
// <0=> MODE_0 
// <1=> MODE_1 
// <2=> MODE_2 
// <3=> MODE_3 

#ifndef SPIS_DEFAULT_MODE
#define SPIS_DEFAULT_MODE 0
#endif

// <o> SPIS_DEFAULT_BIT_ORDER  - SPIS default bit order
 
// <0=> MSB first 
// <1=> LSB first 

#ifndef SPIS_DEFAULT_BIT_ORDER
#define SPIS_DEFAULT_BIT_ORDER 0
#endif

// <o> SPIS_DEFAULT_DEF - SPIS default DEF character  <0-255> 


#ifndef SPIS_DEFAULT_DEF
#define SPIS_DEFAULT_DEF 255
#endif

// <o> SPIS_DEFAULT_ORC - SPIS default ORC character  <0-255> 


#ifndef SPIS_DEFAULT_ORC
#define SPIS_DEFAULT_ORC 255
#endif

// <q> SPIS0_ENABLED  - Enable SPIS0 instance
 

#ifndef SPIS0_ENABLED
#define SPIS0_ENABLED 0
#endif

// <q> SPIS1_ENABLED  - Enable SPIS1 instance
 

#ifndef SPIS1_ENABLED
#define SPIS1_ENABLED 0
#endif

// <q> SPIS2_ENABLED  - Enable SPIS2 instance
 

#ifndef SPIS2_ENABLED
#define SPIS2_ENABLED 0
#endif

#endif //SPIS_ENABLED
// </e>

// <e> SPI_ENABLED - nrf_drv_spi - SPI/SPIM peripheral driver
//==========================================================
#ifndef SPI_ENABLED
#define SPI_ENABLED 0
#endif
#if  SPI_ENABLED
// <e> SPI_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef SPI_CONFIG_LOG_ENABLED
#define SPI_CONFIG_LOG_ENABLED 0
#endif
#if  SPI_CONFIG_LOG_ENABLED
// <o> SPI_CONFIG_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef SPI_CONFIG_LOG_LEVEL
#define SPI_CONFIG_LOG_LEVEL 3
#endif

// <o> SPI_CONFIG_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef SPI_CONFIG_INFO_COLOR
#define SPI_CONFIG_INFO_COLOR 0
#endif

// <o> SPI_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef SPI_CONFIG_DEBUG_COLOR
#define SPI_CONFIG_DEBUG_COLOR 0
#endif

#endif //SPI_CONFIG_LOG_ENABLED
// </e>

// <o> SPI_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef SPI_DEFAULT_CONFIG_IRQ_PRIORITY
#define SPI_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <e> SPI0_ENABLED - Enable SPI0 instance
//==========================================================
#ifndef SPI0_ENABLED
#define SPI0_ENABLED 0
#endif
#if  SPI0_ENABLED
// <q> SPI0_USE_EASY_DMA  - Use EasyDMA
 

#ifndef SPI0_USE_EASY_DMA
#define SPI0_USE_EASY_DMA 1
#endif

#endif //SPI0_ENABLED
// </e>

// <e> SPI1_ENABLED - Enable SPI1 instance
//==========================================================
#ifndef SPI1_ENABLED
#define SPI1_ENABLED 0
#endif
#if  SPI1_ENABLED
// <q> SPI1_USE_EASY_DMA  - Use EasyDMA
 

#ifndef SPI1_USE_EASY_DMA
#define SPI1_USE_EASY_DMA 1
#endif

#endif //SPI1_ENABLED
// </e>

// <e> SPI2_ENABLED - Enable SPI2 instance
//==========================================================
#ifndef SPI2_ENABLED
#define SPI2_ENABLED 0
#endif
#if  SPI2_ENABLED
// <q> SPI2_USE_EASY_DMA  - Use EasyDMA
 

#ifndef SPI2_USE_EASY_DMA
#define SPI2_USE_EASY_DMA 1
#endif

#endif //SPI2_ENABLED
// </e>

#endif //SPI_ENABLED
// </e>

// <e> TIMER_ENABLED - nrf_drv_timer - TIMER periperal driver
//==========================================================
#ifndef TIMER_ENABLED
#define TIMER_ENABLED 0
#endif
#if  TIMER_ENABLED
// <o> TIMER_DEFAULT_CONFIG_FREQUENCY  - Timer frequency if in Timer mode
 
// <0=> 16 MHz 
// <1=> 8 MHz 
// <2=> 4 MHz 
// <3=> 2 MHz 
// <4=> 1 MHz 
// <5=> 500 kHz 
// <6=> 250 kHz 
// <7=> 125 kHz 
// <8=> 62.5 kHz 
// <9=> 31.25 kHz 

#ifndef TIMER_DEFAULT_CONFIG_FREQUENCY
#define TIMER_DEFAULT_CONFIG_FREQUENCY 0
#endif

// <o> TIMER_DEFAULT_CONFIG_MODE  - Timer mode or operation
 
// <0=> Timer 
// <1=> Counter 

#ifndef TIMER_DEFAULT_CONFIG_MODE
#define TIMER_DEFAULT_CONFIG_MODE 0
#endif

// <o> TIMER_DEFAULT_CONFIG_BIT_WIDTH  - Timer counter bit width
 
// <0=> 16 bit 
// <1=> 8 bit 
// <2=> 24 bit 
// <3=> 32 bit 

#ifndef TIMER_DEFAULT_CONFIG_BIT_WIDTH
#define TIMER_DEFAULT_CONFIG_BIT_WIDTH 0
#endif

// <o> TIMER_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef TIMER_DEFAULT_CONFIG_IRQ_PRIORITY
#define TIMER_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <q> TIMER0_ENABLED  - Enable TIMER0 instance
 

#ifndef TIMER0_ENABLED
#define TIMER0_ENABLED 0
#endif

// <q> TIMER1_ENABLED  - Enable TIMER1 instance
 

#ifndef TIMER1_ENABLED
#define TIMER1_ENABLED 0
#endif

// <q> TIMER2_ENABLED  - Enable TIMER2 instance
 

#ifndef TIMER2_ENABLED
#define TIMER2_ENABLED 0
#endif

// <q> TIMER3_ENABLED  - Enable TIMER3 instance
 

#ifndef TIMER3_ENABLED
#define TIMER3_ENABLED 0
#endif

// <q> TIMER4_ENABLED  - Enable TIMER4 instance
 

#ifndef TIMER4_ENABLED
#define TIMER4_ENABLED 0
#endif

#endif //TIMER_ENABLED
// </e>

// <e> TWIS_ENABLED - nrf_drv_twis - TWIS peripheral driver
//==========================================================
#ifndef TWIS_ENABLED
#define TWIS_ENABLED 0
#endif
#if  TWIS_ENABLED
// <o> TWIS_DEFAULT_CONFIG_ADDR0 - Address0 
#ifndef TWIS_DEFAULT_CONFIG_ADDR0
#define TWIS_DEFAULT_CONFIG_ADDR0 0
#endif

// <o> TWIS_DEFAULT_CONFIG_ADDR1 - Address1 
#ifndef TWIS_DEFAULT_CONFIG_ADDR1
#define TWIS_DEFAULT_CONFIG_ADDR1 0
#endif

// <o> TWIS_DEFAULT_CONFIG_SCL_PULL  - SCL pin pull configuration
 
// <0=> Disabled 
// <1=> Pull down 
// <3=> Pull up 

#ifndef TWIS_DEFAULT_CONFIG_SCL_PULL
#define TWIS_DEFAULT_CONFIG_SCL_PULL 0
#endif

// <o> TWIS_DEFAULT_CONFIG_SDA_PULL  - SDA pin pull configuration
 
// <0=> Disabled 
// <1=> Pull down 
// <3=> Pull up 

#ifndef TWIS_DEFAULT_CONFIG_SDA_PULL
#define TWIS_DEFAULT_CONFIG_SDA_PULL 0
#endif

// <o> TWIS_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef TWIS_DEFAULT_CONFIG_IRQ_PRIORITY
#define TWIS_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <q> TWIS0_ENABLED  - Enable TWIS0 instance
 

#ifndef TWIS0_ENABLED
#define TWIS0_ENABLED 0
#endif

// <q> TWIS1_ENABLED  - Enable TWIS1 instance
 

#ifndef TWIS1_ENABLED
#define TWIS1_ENABLED 0
#endif

// <q> TWIS_ASSUME_INIT_AFTER_RESET_ONLY  - Assume that any instance would be initialized only once
 

// <i> Optimization flag. Registers used by TWIS are shared by other peripherals. Normally, during initialization driver tries to clear all registers to known state before doing the initialization itself. This gives initialization safe procedure, no matter when it would be called. If you activate TWIS only once and do never uninitialize it - set this flag to 1 what gives more optimal code.

#ifndef TWIS_ASSUME_INIT_AFTER_RESET_ONLY
#define TWIS_ASSUME_INIT_AFTER_RESET_ONLY 0
#endif

// <q> TWIS_NO_SYNC_MODE  - Remove support for synchronous mode
 

// <i> Synchronous mode would be used in specific situations. And it uses some additional code and data memory to safely process state machine by polling it in status functions. If this functionality is not required it may be disabled to free some resources.

#ifndef TWIS_NO_SYNC_MODE
#define TWIS_NO_SYNC_MODE 0
#endif

#endif //TWIS_ENABLED
// </e>

// <e> TWI_ENABLED - nrf_drv_twi - TWI/TWIM peripheral driver
//==========================================================
#ifndef TWI_ENABLED
#define TWI_ENABLED 0
#endif
#if  TWI_ENABLED
// <o> TWI_DEFAULT_CONFIG_FREQUENCY  - Frequency
 
// <26738688=> 100k 
// <67108864=> 250k 
// <104857600=> 400k 

#ifndef TWI_DEFAULT_CONFIG_FREQUENCY
#define TWI_DEFAULT_CONFIG_FREQUENCY 26738688
#endif

// <q> TWI_DEFAULT_CONFIG_CLR_BUS_INIT  - Enables bus clearing procedure during init
 

#ifndef TWI_DEFAULT_CONFIG_CLR_BUS_INIT
#define TWI_DEFAULT_CONFIG_CLR_BUS_INIT 0
#endif

// <q> TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT  - Enables bus holding after uninit
 

#ifndef TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT
#define TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0
#endif

// <o> TWI_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef TWI_DEFAULT_CONFIG_IRQ_PRIORITY
#define TWI_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <e> TWI0_ENABLED - Enable TWI0 instance
//==========================================================
#ifndef TWI0_ENABLED
#define TWI0_ENABLED 0
#endif
#if  TWI0_ENABLED
// <q> TWI0_USE_EASY_DMA  - Use EasyDMA (if present)
 

#ifndef TWI0_USE_EASY_DMA
#define TWI0_USE_EASY_DMA 0
#endif

#endif //TWI0_ENABLED
// </e>

// <e> TWI1_ENABLED - Enable TWI1 instance
//==========================================================
#ifndef TWI1_ENABLED
#define TWI1_ENABLED 0
#endif
#if  TWI1_ENABLED
// <q> TWI1_USE_EASY_DMA  - Use EasyDMA (if present)
 

#ifndef TWI1_USE_EASY_DMA
#define TWI1_USE_EASY_DMA 0
#endif

#endif //TWI1_ENABLED
// </e>

#endif //TWI_ENABLED
// </e>

// <e> UART_ENABLED - nrf_drv_uart - UART/UARTE peripheral driver
//==========================================================
#ifndef UART_ENABLED
#define UART_ENABLED 1
#endif
#if  UART_ENABLED
// <o> UART_DEFAULT_CONFIG_HWFC  - Hardware Flow Control
 
// <0=> Disabled 
// <1=> Enabled 

#ifndef UART_DEFAULT_CONFIG_HWFC
#define UART_DEFAULT_CONFIG_HWFC 0
#endif

// <o> UART_DEFAULT_CONFIG_PARITY  - Parity
 
// <0=> Excluded 
// <14=> Included 

#ifndef UART_DEFAULT_CONFIG_PARITY
#define UART_DEFAULT_CONFIG_PARITY 0
#endif

// <o> UART_DEFAULT_CONFIG_BAUDRATE  - Default Baudrate
 
// <323584=> 1200 baud 
// <643072=> 2400 baud 
// <1290240=> 4800 baud 
// <2576384=> 9600 baud 
// <3862528=> 14400 baud 
// <5152768=> 19200 baud 
// <7716864=> 28800 baud 
// <10289152=> 38400 baud 
// <15400960=> 57600 baud 
// <20615168=> 76800 baud 
// <30801920=> 115200 baud 
// <61865984=> 230400 baud 
// <67108864=> 250000 baud 
// <121634816=> 460800 baud 
// <251658240=> 921600 baud 
// <268435456=> 57600 baud 

#ifndef UART_DEFAULT_CONFIG_BAUDRATE
#define UART_DEFAULT_CONFIG_BAUDRATE 30801920
#endif

// <o> UART_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef UART_DEFAULT_CONFIG_IRQ_PRIORITY
#define UART_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <q> UART0_CONFIG_USE_EASY_DMA  - Default setting for using EasyDMA
 

#ifndef UART0_CONFIG_USE_EASY_DMA
#define UART0_CONFIG_USE_EASY_DMA 1
#endif

// <q> UART_EASY_DMA_SUPPORT  - Driver supporting EasyDMA
 

#ifndef UART_EASY_DMA_SUPPORT
#define UART_EASY_DMA_SUPPORT 1
#endif

// <q> UART_LEGACY_SUPPORT  - Driver supporting Legacy mode
 

#ifndef UART_LEGACY_SUPPORT
#define UART_LEGACY_SUPPORT 1
#endif

#endif //UART_ENABLED
// </e>

// <e> WDT_ENABLED - nrf_drv_wdt - WDT peripheral driver
//==========================================================
#ifndef WDT_ENABLED
#define WDT_ENABLED 0
#endif
#if  WDT_ENABLED
// <o> WDT_CONFIG_BEHAVIOUR  - WDT behavior in CPU SLEEP or HALT mode
 
// <1=> Run in SLEEP, Pause in HALT 
// <8=> Pause in SLEEP, Run in HALT 
// <9=> Run in SLEEP and HALT 
// <0=> Pause in SLEEP and HALT 

#ifndef WDT_CONFIG_BEHAVIOUR
#define WDT_CONFIG_BEHAVIOUR 1
#endif

// <o> WDT_CONFIG_RELOAD_VALUE - Reload value  <15-4294967295> 


#ifndef WDT_CONFIG_RELOAD_VALUE
#define WDT_CONFIG_RELOAD_VALUE 2000
#endif

// <o> WDT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef WDT_CONFIG_IRQ_PRIORITY
#define WDT_CONFIG_IRQ_PRIORITY 6
#endif

#endif //WDT_ENABLED
// </e>

// </h> 
//==========================================================

// <h> nRF_Libraries 

//==========================================================
// <q> APP_FIFO_ENABLED  - app_fifo - Software FIFO implementation
 

#ifndef APP_FIFO_ENABLED
#define APP_FIFO_ENABLED 1
#endif

// <q> APP_MAILBOX_ENABLED  - app_mailbox - Thread safe mailbox
 

#ifndef APP_MAILBOX_ENABLED
#define APP_MAILBOX_ENABLED 0
#endif

// <q> APP_PWM_ENABLED  - app_pwm - PWM functionality
 

#ifndef APP_PWM_ENABLED
#define APP_PWM_ENABLED 0
#endif

// <e> APP_SCHEDULER_ENABLED - app_scheduler - Events scheduler
//==========================================================
#ifndef APP_SCHEDULER_ENABLED
#define APP_SCHEDULER_ENABLED 0
#endif
#if  APP_SCHEDULER_ENABLED
// <q> APP_SCHEDULER_WITH_PAUSE  - Enabling pause feature
 

#ifndef APP_SCHEDULER_WITH_PAUSE
#define APP_SCHEDULER_WITH_PAUSE 0
#endif

// <q> APP_SCHEDULER_WITH_PROFILER  - Enabling scheduler profiling
 

#ifndef APP_SCHEDULER_WITH_PROFILER
#define APP_SCHEDULER_WITH_PROFILER 0
#endif

#endif //APP_SCHEDULER_ENABLED
// </e>

// <e> APP_TIMER_ENABLED - app_timer - Application timer functionality
//==========================================================
#ifndef APP_TIMER_ENABLED
#define APP_TIMER_ENABLED 1
#endif
#if  APP_TIMER_ENABLED
// <q> APP_TIMER_WITH_PROFILER  - Enable app_timer profiling
 

#ifndef APP_TIMER_WITH_PROFILER
#define APP_TIMER_WITH_PROFILER 0
#endif

// <q> APP_TIMER_KEEPS_RTC_ACTIVE  - Enable RTC always on
 

// <i> If option is enabled RTC is kept running even if there is no active timers.
// <i> This option can be used when app_timer is used for timestamping.

#ifndef APP_TIMER_KEEPS_RTC_ACTIVE
#define APP_TIMER_KEEPS_RTC_ACTIVE 0
#endif

#endif //APP_TIMER_ENABLED
// </e>

// <q> APP_TWI_ENABLED  - app_twi - TWI transaction manager
 

#ifndef APP_TWI_ENABLED
#define APP_TWI_ENABLED 0
#endif

// <e> APP_UART_ENABLED - app_uart - UART driver
//==========================================================
#ifndef APP_UART_ENABLED
#define APP_UART_ENABLED 1
#endif
#if  APP_UART_ENABLED
// <o> APP_UART_DRIVER_INSTANCE  - UART instance used
 
// <0=> 0 

#ifndef APP_UART_DRIVER_INSTANCE
#define APP_UART_DRIVER_INSTANCE 0
#endif

#endif //APP_UART_ENABLED
// </e>

// <q> BUTTON_ENABLED  - app_button - buttons handling module
 

#ifndef BUTTON_ENABLED
#define BUTTON_ENABLED 1
#endif

// <q> CRC16_ENABLED  - crc16 - CRC16 calculation routines
 

#ifndef CRC16_ENABLED
#define CRC16_ENABLED 0
#endif

// <q> CRC32_ENABLED  - crc32 - CRC32 calculation routines
 

#ifndef CRC32_ENABLED
#define CRC32_ENABLED 0
#endif

// <q> ECC_ENABLED  - ecc - Elliptic Curve Cryptography Library
 

#ifndef ECC_ENABLED
#define ECC_ENABLED 0
#endif

// <e> FDS_ENABLED - fds - Flash data storage module
//==========================================================
#ifndef FDS_ENABLED
#define FDS_ENABLED 1
#endif
#if  FDS_ENABLED
// <o> FDS_OP_QUEUE_SIZE - Size of the internal queue. 
#ifndef FDS_OP_QUEUE_SIZE
#define FDS_OP_QUEUE_SIZE 4
#endif

// <o> FDS_CHUNK_QUEUE_SIZE - Determines how many @ref fds_record_chunk_t structures can be buffered at any time. 
#ifndef FDS_CHUNK_QUEUE_SIZE
#define FDS_CHUNK_QUEUE_SIZE 8
#endif

// <o> FDS_MAX_USERS - Maximum number of callbacks that can be registered. 
#ifndef FDS_MAX_USERS
#define FDS_MAX_USERS 8
#endif

// <o> FDS_VIRTUAL_PAGES - Number of virtual flash pages to use. 
// <i> One of the virtual pages is reserved by the system for garbage collection.
// <i> Therefore, the minimum is two virtual pages: one page to store data and
// <i> one page to be used by the system for garbage collection. The total amount
// <i> of flash memory that is used by FDS amounts to @ref FDS_VIRTUAL_PAGES
// <i> @ref FDS_VIRTUAL_PAGE_SIZE * 4 bytes.

#ifndef FDS_VIRTUAL_PAGES
#define FDS_VIRTUAL_PAGES 3
#endif

// <o> FDS_VIRTUAL_PAGE_SIZE  - The size of a virtual page of flash memory, expressed in number of 4-byte words.
 

// <i> By default, a virtual page is the same size as a physical page.
// <i> The size of a virtual page must be a multiple of the size of a physical page.
// <1024=> 1024 
// <2048=> 2048 

#ifndef FDS_VIRTUAL_PAGE_SIZE
#define FDS_VIRTUAL_PAGE_SIZE 1024
#endif

#endif //FDS_ENABLED
// </e>

// <e> FSTORAGE_ENABLED - fstorage - Flash storage module
//==========================================================
#ifndef FSTORAGE_ENABLED
#define FSTORAGE_ENABLED 1
#endif
#if  FSTORAGE_ENABLED
// <o> FS_QUEUE_SIZE - Configures the size of the internal queue. 
// <i> Increase this if there are many users, or if it is likely that many
// <i> operation will be queued at once without waiting for the previous operations
// <i> to complete. In general, increase the queue size if you frequently receive
// <i> @ref FS_ERR_QUEUE_FULL errors when calling @ref fs_store or @ref fs_erase.

#ifndef FS_QUEUE_SIZE
#define FS_QUEUE_SIZE 4
#endif

// <o> FS_OP_MAX_RETRIES - Number attempts to execute an operation if the SoftDevice fails. 
// <i> Increase this value if events return the @ref FS_ERR_OPERATION_TIMEOUT
// <i> error often. The SoftDevice may fail to schedule flash access due to high BLE activity.

#ifndef FS_OP_MAX_RETRIES
#define FS_OP_MAX_RETRIES 3
#endif

// <o> FS_MAX_WRITE_SIZE_WORDS - Maximum number of words to be written to flash in a single operation. 
// <i> Tweaking this value can increase the chances of the SoftDevice being
// <i> able to fit flash operations in between radio activity. This value is bound by the
// <i> maximum number of words which the SoftDevice can write to flash in a single call to
// <i> @ref sd_flash_write, which is 256 words for nRF51 ICs and 1024 words for nRF52 ICs.

#ifndef FS_MAX_WRITE_SIZE_WORDS
#define FS_MAX_WRITE_SIZE_WORDS 1024
#endif

#endif //FSTORAGE_ENABLED
// </e>

// <q> HARDFAULT_HANDLER_ENABLED  - hardfault_default - HardFault default handler for debugging and release
 

#ifndef HARDFAULT_HANDLER_ENABLED
#define HARDFAULT_HANDLER_ENABLED 0
#endif

// <e> HCI_MEM_POOL_ENABLED - hci_mem_pool - memory pool implementation used by HCI
//==========================================================
#ifndef HCI_MEM_POOL_ENABLED
#define HCI_MEM_POOL_ENABLED 0
#endif
#if  HCI_MEM_POOL_ENABLED
// <o> HCI_TX_BUF_SIZE - TX buffer size in bytes. 
#ifndef HCI_TX_BUF_SIZE
#define HCI_TX_BUF_SIZE 600
#endif

// <o> HCI_RX_BUF_SIZE - RX buffer size in bytes. 
#ifndef HCI_RX_BUF_SIZE
#define HCI_RX_BUF_SIZE 600
#endif

// <o> HCI_RX_BUF_QUEUE_SIZE - RX buffer queue size. 
#ifndef HCI_RX_BUF_QUEUE_SIZE
#define HCI_RX_BUF_QUEUE_SIZE 4
#endif

#endif //HCI_MEM_POOL_ENABLED
// </e>

// <e> HCI_SLIP_ENABLED - hci_slip - SLIP protocol implementation used by HCI
//==========================================================
#ifndef HCI_SLIP_ENABLED
#define HCI_SLIP_ENABLED 0
#endif
#if  HCI_SLIP_ENABLED
// <o> HCI_UART_BAUDRATE  - Default Baudrate
 
// <323584=> 1200 baud 
// <643072=> 2400 baud 
// <1290240=> 4800 baud 
// <2576384=> 9600 baud 
// <3862528=> 14400 baud 
// <5152768=> 19200 baud 
// <7716864=> 28800 baud 
// <10289152=> 38400 baud 
// <15400960=> 57600 baud 
// <20615168=> 76800 baud 
// <30801920=> 115200 baud 
// <61865984=> 230400 baud 
// <67108864=> 250000 baud 
// <121634816=> 460800 baud 
// <251658240=> 921600 baud 
// <268435456=> 57600 baud 

#ifndef HCI_UART_BAUDRATE
#define HCI_UART_BAUDRATE 30801920
#endif

// <o> HCI_UART_FLOW_CONTROL  - Hardware Flow Control
 
// <0=> Disabled 
// <1=> Enabled 

#ifndef HCI_UART_FLOW_CONTROL
#define HCI_UART_FLOW_CONTROL 0
#endif

// <o> HCI_UART_RX_PIN - UART RX pin 
#ifndef HCI_UART_RX_PIN
#define HCI_UART_RX_PIN 8
#endif

// <o> HCI_UART_TX_PIN - UART TX pin 
#ifndef HCI_UART_TX_PIN
#define HCI_UART_TX_PIN 6
#endif

// <o> HCI_UART_RTS_PIN - UART RTS pin 
#ifndef HCI_UART_RTS_PIN
#define HCI_UART_RTS_PIN 5
#endif

// <o> HCI_UART_CTS_PIN - UART CTS pin 
#ifndef HCI_UART_CTS_PIN
#define HCI_UART_CTS_PIN 7
#endif

#endif //HCI_SLIP_ENABLED
// </e>

// <e> HCI_TRANSPORT_ENABLED - hci_transport - HCI transport
//==========================================================
#ifndef HCI_TRANSPORT_ENABLED
#define HCI_TRANSPORT_ENABLED 0
#endif
#if  HCI_TRANSPORT_ENABLED
// <o> HCI_MAX_PACKET_SIZE_IN_BITS - Maximum size of a single application packet in bits. 
#ifndef HCI_MAX_PACKET_SIZE_IN_BITS
#define HCI_MAX_PACKET_SIZE_IN_BITS 8000
#endif

#endif //HCI_TRANSPORT_ENABLED
// </e>

// <q> LED_SOFTBLINK_ENABLED  - led_softblink - led_softblink module
 

#ifndef LED_SOFTBLINK_ENABLED
#define LED_SOFTBLINK_ENABLED 0
#endif

// <q> LOW_POWER_PWM_ENABLED  - low_power_pwm - low_power_pwm module
 

#ifndef LOW_POWER_PWM_ENABLED
#define LOW_POWER_PWM_ENABLED 0
#endif

// <e> MEM_MANAGER_ENABLED - mem_manager - Dynamic memory allocator
//==========================================================
#ifndef MEM_MANAGER_ENABLED
#define MEM_MANAGER_ENABLED 0
#endif
#if  MEM_MANAGER_ENABLED
// <o> MEMORY_MANAGER_SMALL_BLOCK_COUNT - Size of each memory blocks identified as 'small' block.  <0-255> 


#ifndef MEMORY_MANAGER_SMALL_BLOCK_COUNT
#define MEMORY_MANAGER_SMALL_BLOCK_COUNT 1
#endif

// <o> MEMORY_MANAGER_SMALL_BLOCK_SIZE -  Size of each memory blocks identified as 'small' block. 
// <i>  Size of each memory blocks identified as 'small' block. Memory block are recommended to be word-sized.

#ifndef MEMORY_MANAGER_SMALL_BLOCK_SIZE
#define MEMORY_MANAGER_SMALL_BLOCK_SIZE 32
#endif

// <o> MEMORY_MANAGER_MEDIUM_BLOCK_COUNT - Size of each memory blocks identified as 'medium' block.  <0-255> 


#ifndef MEMORY_MANAGER_MEDIUM_BLOCK_COUNT
#define MEMORY_MANAGER_MEDIUM_BLOCK_COUNT 0
#endif

// <o> MEMORY_MANAGER_MEDIUM_BLOCK_SIZE -  Size of each memory blocks identified as 'medium' block. 
// <i>  Size of each memory blocks identified as 'medium' block. Memory block are recommended to be word-sized.

#ifndef MEMORY_MANAGER_MEDIUM_BLOCK_SIZE
#define MEMORY_MANAGER_MEDIUM_BLOCK_SIZE 256
#endif

// <o> MEMORY_MANAGER_LARGE_BLOCK_COUNT - Size of each memory blocks identified as 'large' block.  <0-255> 


#ifndef MEMORY_MANAGER_LARGE_BLOCK_COUNT
#define MEMORY_MANAGER_LARGE_BLOCK_COUNT 0
#endif

// <o> MEMORY_MANAGER_LARGE_BLOCK_SIZE -  Size of each memory blocks identified as 'large' block. 
// <i>  Size of each memory blocks identified as 'large' block. Memory block are recommended to be word-sized.

#ifndef MEMORY_MANAGER_LARGE_BLOCK_SIZE
#define MEMORY_MANAGER_LARGE_BLOCK_SIZE 256
#endif

// <q> MEM_MANAGER_ENABLE_LOGS  - Enable debug trace in the module.
 

#ifndef MEM_MANAGER_ENABLE_LOGS
#define MEM_MANAGER_ENABLE_LOGS 0
#endif

// <q> MEM_MANAGER_DISABLE_API_PARAM_CHECK  - Disable API parameter checks in the module.
 

#ifndef MEM_MANAGER_DISABLE_API_PARAM_CHECK
#define MEM_MANAGER_DISABLE_API_PARAM_CHECK 0
#endif

#endif //MEM_MANAGER_ENABLED
// </e>

// <e> NRF_CSENSE_ENABLED - nrf_csense - nrf_csense module
//==========================================================
#ifndef NRF_CSENSE_ENABLED
#define NRF_CSENSE_ENABLED 0
#endif
#if  NRF_CSENSE_ENABLED
// <o> NRF_CSENSE_PAD_HYSTERESIS - Minimal value of change to decide that pad was touched. 
#ifndef NRF_CSENSE_PAD_HYSTERESIS
#define NRF_CSENSE_PAD_HYSTERESIS 15
#endif

// <o> NRF_CSENSE_PAD_DEVIATION - Minimal value measured on pad to take its value while calculating step. 
#ifndef NRF_CSENSE_PAD_DEVIATION
#define NRF_CSENSE_PAD_DEVIATION 70
#endif

// <o> NRF_CSENSE_MIN_PAD_VALUE - Minimum normalized value on pad to take its value into account. 
#ifndef NRF_CSENSE_MIN_PAD_VALUE
#define NRF_CSENSE_MIN_PAD_VALUE 20
#endif

// <o> NRF_CSENSE_MAX_PADS_NUMBER - Maximum number of pads used for one instance. 
#ifndef NRF_CSENSE_MAX_PADS_NUMBER
#define NRF_CSENSE_MAX_PADS_NUMBER 20
#endif

// <o> NRF_CSENSE_MAX_VALUE - Maximum normalized value got from measurement. 
#ifndef NRF_CSENSE_MAX_VALUE
#define NRF_CSENSE_MAX_VALUE 1000
#endif

// <o> NRF_CSENSE_OUTPUT_PIN - Output pin used by lower module. 
// <i> This is only used when running on NRF51.

#ifndef NRF_CSENSE_OUTPUT_PIN
#define NRF_CSENSE_OUTPUT_PIN 30
#endif

#endif //NRF_CSENSE_ENABLED
// </e>

// <e> NRF_DRV_CSENSE_ENABLED - nrf_drv_csense - Capacitive sensor module
//==========================================================
#ifndef NRF_DRV_CSENSE_ENABLED
#define NRF_DRV_CSENSE_ENABLED 0
#endif
#if  NRF_DRV_CSENSE_ENABLED
// <o> TIMER0_FOR_CSENSE - First TIMER instance used by the driver (except nRF51) 
#ifndef TIMER0_FOR_CSENSE
#define TIMER0_FOR_CSENSE 1
#endif

// <o> TIMER1_FOR_CSENSE - Second TIMER instance used by the driver (except nRF51) 
#ifndef TIMER1_FOR_CSENSE
#define TIMER1_FOR_CSENSE 2
#endif

// <o> MEASUREMENT_PERIOD - Single measurement period. 
// <i> Time of single measurement can be calculated as T = (1/2)*MEASUREMENT_PERIOD*(1/f_OSC) where f_OSC = I_SOURCE / (2C*(VUP-VDOWN) ). I_SOURCE, VUP and VDOWN are values used to initialize COMP and C is capacitance of used pad.

#ifndef MEASUREMENT_PERIOD
#define MEASUREMENT_PERIOD 20
#endif

#endif //NRF_DRV_CSENSE_ENABLED
// </e>

// <q> RETARGET_ENABLED  - retarget - Retargeting stdio functions
 

#ifndef RETARGET_ENABLED
#define RETARGET_ENABLED 1
#endif

// <q> SLIP_ENABLED  - slip - SLIP encoding decoding
 

#ifndef SLIP_ENABLED
#define SLIP_ENABLED 0
#endif

// </h> 
//==========================================================

// <h> nRF_Log 

//==========================================================
// <e> NRF_LOG_ENABLED - nrf_log - Logging
//==========================================================
#ifndef NRF_LOG_ENABLED
#define NRF_LOG_ENABLED 0
#endif
#if  NRF_LOG_ENABLED
// <e> NRF_LOG_USES_COLORS - If enabled then ANSI escape code for colors is prefixed to every string
//==========================================================
#ifndef NRF_LOG_USES_COLORS
#define NRF_LOG_USES_COLORS 0
#endif
#if  NRF_LOG_USES_COLORS
// <o> NRF_LOG_COLOR_DEFAULT  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NRF_LOG_COLOR_DEFAULT
#define NRF_LOG_COLOR_DEFAULT 0
#endif

// <o> NRF_LOG_ERROR_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NRF_LOG_ERROR_COLOR
#define NRF_LOG_ERROR_COLOR 0
#endif

// <o> NRF_LOG_WARNING_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NRF_LOG_WARNING_COLOR
#define NRF_LOG_WARNING_COLOR 0
#endif

#endif //NRF_LOG_USES_COLORS
// </e>

// <o> NRF_LOG_DEFAULT_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NRF_LOG_DEFAULT_LEVEL
#define NRF_LOG_DEFAULT_LEVEL 3
#endif

// <e> NRF_LOG_DEFERRED - Enable deffered logger.

// <i> Log data is buffered and can be processed in idle.
//==========================================================
#ifndef NRF_LOG_DEFERRED
#define NRF_LOG_DEFERRED 1
#endif
#if  NRF_LOG_DEFERRED
// <o> NRF_LOG_DEFERRED_BUFSIZE - Size of the buffer for logs in words. 
// <i> Must be power of 2

#ifndef NRF_LOG_DEFERRED_BUFSIZE
#define NRF_LOG_DEFERRED_BUFSIZE 256
#endif

#endif //NRF_LOG_DEFERRED
// </e>

// <q> NRF_LOG_USES_TIMESTAMP  - Enable timestamping
 

// <i> Function for getting the timestamp is provided by the user

#ifndef NRF_LOG_USES_TIMESTAMP
#define NRF_LOG_USES_TIMESTAMP 0
#endif

#endif //NRF_LOG_ENABLED
// </e>

// </h> 
//==========================================================

// <<< end of configuration section >>>
#endif //SDK_CONFIG_H


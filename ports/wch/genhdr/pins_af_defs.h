#define STATIC_AF_TIM10_BKIN_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E2_obj)")  & strcmp( #pin_obj , "((&pin_E2_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_A23(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E2_obj)")  & strcmp( #pin_obj , "((&pin_E2_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_EVENTOUT_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A10_obj)")  & strcmp( #pin_obj , "((&pin_A10_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A11_obj)")  & strcmp( #pin_obj , "((&pin_A11_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A13_obj)")  & strcmp( #pin_obj , "((&pin_A13_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A14_obj)")  & strcmp( #pin_obj , "((&pin_A14_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A15_obj)")  & strcmp( #pin_obj , "((&pin_A15_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A4_obj)")  & strcmp( #pin_obj , "((&pin_A4_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A5_obj)")  & strcmp( #pin_obj , "((&pin_A5_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A6_obj)")  & strcmp( #pin_obj , "((&pin_A6_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A8_obj)")  & strcmp( #pin_obj , "((&pin_A8_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_A9_obj)")  & strcmp( #pin_obj , "((&pin_A9_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B10_obj)")  & strcmp( #pin_obj , "((&pin_B10_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B11_obj)")  & strcmp( #pin_obj , "((&pin_B11_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B14_obj)")  & strcmp( #pin_obj , "((&pin_B14_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B15_obj)")  & strcmp( #pin_obj , "((&pin_B15_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B2_obj)")  & strcmp( #pin_obj , "((&pin_B2_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B3_obj)")  & strcmp( #pin_obj , "((&pin_B3_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B4_obj)")  & strcmp( #pin_obj , "((&pin_B4_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B7_obj)")  & strcmp( #pin_obj , "((&pin_B7_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C0_obj)")  & strcmp( #pin_obj , "((&pin_C0_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C10_obj)")  & strcmp( #pin_obj , "((&pin_C10_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C11_obj)")  & strcmp( #pin_obj , "((&pin_C11_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C12_obj)")  & strcmp( #pin_obj , "((&pin_C12_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C13_obj)")  & strcmp( #pin_obj , "((&pin_C13_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C14_obj)")  & strcmp( #pin_obj , "((&pin_C14_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C15_obj)")  & strcmp( #pin_obj , "((&pin_C15_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C1_obj)")  & strcmp( #pin_obj , "((&pin_C1_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C2_obj)")  & strcmp( #pin_obj , "((&pin_C2_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C3_obj)")  & strcmp( #pin_obj , "((&pin_C3_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C4_obj)")  & strcmp( #pin_obj , "((&pin_C4_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C5_obj)")  & strcmp( #pin_obj , "((&pin_C5_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C6_obj)")  & strcmp( #pin_obj , "((&pin_C6_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C7_obj)")  & strcmp( #pin_obj , "((&pin_C7_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C8_obj)")  & strcmp( #pin_obj , "((&pin_C8_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_C9_obj)")  & strcmp( #pin_obj , "((&pin_C9_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D0_obj)")  & strcmp( #pin_obj , "((&pin_D0_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D0_obj)")  & strcmp( #pin_obj , "((&pin_D0_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D10_obj)")  & strcmp( #pin_obj , "((&pin_D10_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D11_obj)")  & strcmp( #pin_obj , "((&pin_D11_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D12_obj)")  & strcmp( #pin_obj , "((&pin_D12_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D13_obj)")  & strcmp( #pin_obj , "((&pin_D13_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D14_obj)")  & strcmp( #pin_obj , "((&pin_D14_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D15_obj)")  & strcmp( #pin_obj , "((&pin_D15_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D1_obj)")  & strcmp( #pin_obj , "((&pin_D1_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D1_obj)")  & strcmp( #pin_obj , "((&pin_D1_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D2_obj)")  & strcmp( #pin_obj , "((&pin_D2_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D3_obj)")  & strcmp( #pin_obj , "((&pin_D3_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D4_obj)")  & strcmp( #pin_obj , "((&pin_D4_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D5_obj)")  & strcmp( #pin_obj , "((&pin_D5_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D6_obj)")  & strcmp( #pin_obj , "((&pin_D6_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D7_obj)")  & strcmp( #pin_obj , "((&pin_D7_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D8_obj)")  & strcmp( #pin_obj , "((&pin_D8_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_D9_obj)")  & strcmp( #pin_obj , "((&pin_D9_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E0_obj)")  & strcmp( #pin_obj , "((&pin_E0_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E10_obj)")  & strcmp( #pin_obj , "((&pin_E10_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E11_obj)")  & strcmp( #pin_obj , "((&pin_E11_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E12_obj)")  & strcmp( #pin_obj , "((&pin_E12_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E13_obj)")  & strcmp( #pin_obj , "((&pin_E13_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E14_obj)")  & strcmp( #pin_obj , "((&pin_E14_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E15_obj)")  & strcmp( #pin_obj , "((&pin_E15_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E1_obj)")  & strcmp( #pin_obj , "((&pin_E1_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E2_obj)")  & strcmp( #pin_obj , "((&pin_E2_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E3_obj)")  & strcmp( #pin_obj , "((&pin_E3_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E4_obj)")  & strcmp( #pin_obj , "((&pin_E4_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E5_obj)")  & strcmp( #pin_obj , "((&pin_E5_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E6_obj)")  & strcmp( #pin_obj , "((&pin_E6_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E7_obj)")  & strcmp( #pin_obj , "((&pin_E7_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E8_obj)")  & strcmp( #pin_obj , "((&pin_E8_obj))")) == 0) ? (15) : \
    ((strcmp( #pin_obj , "(&pin_E9_obj)")  & strcmp( #pin_obj , "((&pin_E9_obj))")) == 0) ? (15) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH1N_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E3_obj)")  & strcmp( #pin_obj , "((&pin_E3_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_A19(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E3_obj)")  & strcmp( #pin_obj , "((&pin_E3_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH2N_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E4_obj)")  & strcmp( #pin_obj , "((&pin_E4_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_A20(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E4_obj)")  & strcmp( #pin_obj , "((&pin_E4_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH3N_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E5_obj)")  & strcmp( #pin_obj , "((&pin_E5_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_A21(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E5_obj)")  & strcmp( #pin_obj , "((&pin_E5_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_A22(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E6_obj)")  & strcmp( #pin_obj , "((&pin_E6_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TAMPER-RTC_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C13_obj)")  & strcmp( #pin_obj , "((&pin_C13_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH4_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C13_obj)")  & strcmp( #pin_obj , "((&pin_C13_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_OSC32_IN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C14_obj)")  & strcmp( #pin_obj , "((&pin_C14_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH4_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C14_obj)")  & strcmp( #pin_obj , "((&pin_C14_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_OSC32_OUT(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C15_obj)")  & strcmp( #pin_obj , "((&pin_C15_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH4_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C15_obj)")  & strcmp( #pin_obj , "((&pin_C15_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_OSC_IN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D0_obj)")  & strcmp( #pin_obj , "((&pin_D0_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_OSC_OUT(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D1_obj)")  & strcmp( #pin_obj , "((&pin_D1_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH1N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C0_obj)")  & strcmp( #pin_obj , "((&pin_C0_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART6_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C0_obj)")  & strcmp( #pin_obj , "((&pin_C0_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_RXC(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C0_obj)")  & strcmp( #pin_obj , "((&pin_C0_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH2N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C1_obj)")  & strcmp( #pin_obj , "((&pin_C1_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART6_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C1_obj)")  & strcmp( #pin_obj , "((&pin_C1_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_MDC(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C1_obj)")  & strcmp( #pin_obj , "((&pin_C1_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_MDC (pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C1_obj)")  & strcmp( #pin_obj , "((&pin_C1_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ ETH_RGMII_RXCTL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C1_obj)")  & strcmp( #pin_obj , "((&pin_C1_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH3N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C2_obj)")  & strcmp( #pin_obj , "((&pin_C2_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART7_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C2_obj)")  & strcmp( #pin_obj , "((&pin_C2_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_TXD2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C2_obj)")  & strcmp( #pin_obj , "((&pin_C2_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_RXD0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C2_obj)")  & strcmp( #pin_obj , "((&pin_C2_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C3_obj)")  & strcmp( #pin_obj , "((&pin_C3_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART7_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C3_obj)")  & strcmp( #pin_obj , "((&pin_C3_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_TX_CLK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C3_obj)")  & strcmp( #pin_obj , "((&pin_C3_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_RXD1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C3_obj)")  & strcmp( #pin_obj , "((&pin_C3_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_WKUP_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH1_ETR(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH1_ETR_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM5_CH1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_ETR(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_ETR_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_CTS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_CRS_WKUP(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_RXD2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A0_obj)")  & strcmp( #pin_obj , "((&pin_A0_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH2_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM5_CH2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_BKIN_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_RTS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_RX_CLK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_REF_CLK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_RXD3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A1_obj)")  & strcmp( #pin_obj , "((&pin_A1_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH3_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM5_CH3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH1_ETR(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH1_ETR_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_MDIO(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_MDIO(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_GTXC(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A2_obj)")  & strcmp( #pin_obj , "((&pin_A2_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH4_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM5_CH4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH2_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_COL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_TXEN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A3_obj)")  & strcmp( #pin_obj , "((&pin_A3_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A4_obj)")  & strcmp( #pin_obj , "((&pin_A4_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH3_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A4_obj)")  & strcmp( #pin_obj , "((&pin_A4_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI1_NSS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A15_obj)")  & strcmp( #pin_obj , "((&pin_A15_obj))")) == 0) ? (5) : \
    ((strcmp( #pin_obj , "(&pin_A4_obj)")  & strcmp( #pin_obj , "((&pin_A4_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI3_NSS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A15_obj)")  & strcmp( #pin_obj , "((&pin_A15_obj))")) == 0) ? (6) : \
    ((strcmp( #pin_obj , "(&pin_A4_obj)")  & strcmp( #pin_obj , "((&pin_A4_obj))")) == 0) ? (6) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2S3_WS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A15_obj)")  & strcmp( #pin_obj , "((&pin_A15_obj))")) == 0) ? (6) : \
    ((strcmp( #pin_obj , "(&pin_A4_obj)")  & strcmp( #pin_obj , "((&pin_A4_obj))")) == 0) ? (6) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_CK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A4_obj)")  & strcmp( #pin_obj , "((&pin_A4_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH1N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A5_obj)")  & strcmp( #pin_obj , "((&pin_A5_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI1_SCK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A5_obj)")  & strcmp( #pin_obj , "((&pin_A5_obj))")) == 0) ? (5) : \
    ((strcmp( #pin_obj , "(&pin_B3_obj)")  & strcmp( #pin_obj , "((&pin_B3_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_CTS_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A5_obj)")  & strcmp( #pin_obj , "((&pin_A5_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_CK_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A5_obj)")  & strcmp( #pin_obj , "((&pin_A5_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_VSYNC(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A5_obj)")  & strcmp( #pin_obj , "((&pin_A5_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_BKIN_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A6_obj)")  & strcmp( #pin_obj , "((&pin_A6_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A6_obj)")  & strcmp( #pin_obj , "((&pin_A6_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_BKIN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A6_obj)")  & strcmp( #pin_obj , "((&pin_A6_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH2N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A6_obj)")  & strcmp( #pin_obj , "((&pin_A6_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI1_MISO(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A6_obj)")  & strcmp( #pin_obj , "((&pin_A6_obj))")) == 0) ? (5) : \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_TX_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A6_obj)")  & strcmp( #pin_obj , "((&pin_A6_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART7_TX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A6_obj)")  & strcmp( #pin_obj , "((&pin_A6_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH1N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH1N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH3N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI1_MOSI(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (5) : \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_RX_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART7_RX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_CRS_DV(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (11) : \
    ((strcmp( #pin_obj , "(&pin_D8_obj)")  & strcmp( #pin_obj , "((&pin_D8_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_TXD0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A7_obj)")  & strcmp( #pin_obj , "((&pin_A7_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C4_obj)")  & strcmp( #pin_obj , "((&pin_C4_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_CTS_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C4_obj)")  & strcmp( #pin_obj , "((&pin_C4_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART8_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C4_obj)")  & strcmp( #pin_obj , "((&pin_C4_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_RXD0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C4_obj)")  & strcmp( #pin_obj , "((&pin_C4_obj))")) == 0) ? (11) : \
    ((strcmp( #pin_obj , "(&pin_D9_obj)")  & strcmp( #pin_obj , "((&pin_D9_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_RXD0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C4_obj)")  & strcmp( #pin_obj , "((&pin_C4_obj))")) == 0) ? (11) : \
    ((strcmp( #pin_obj , "(&pin_D9_obj)")  & strcmp( #pin_obj , "((&pin_D9_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_TXD1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C4_obj)")  & strcmp( #pin_obj , "((&pin_C4_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_BKIN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C5_obj)")  & strcmp( #pin_obj , "((&pin_C5_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_RTS_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C5_obj)")  & strcmp( #pin_obj , "((&pin_C5_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART8_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C5_obj)")  & strcmp( #pin_obj , "((&pin_C5_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_RXD1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C5_obj)")  & strcmp( #pin_obj , "((&pin_C5_obj))")) == 0) ? (11) : \
    ((strcmp( #pin_obj , "(&pin_D10_obj)")  & strcmp( #pin_obj , "((&pin_D10_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_RXD1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C5_obj)")  & strcmp( #pin_obj , "((&pin_C5_obj))")) == 0) ? (11) : \
    ((strcmp( #pin_obj , "(&pin_D10_obj)")  & strcmp( #pin_obj , "((&pin_D10_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_TXD2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C5_obj)")  & strcmp( #pin_obj , "((&pin_C5_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH2N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH3_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH2N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH1N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART4_TX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_RXD2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (11) : \
    ((strcmp( #pin_obj , "(&pin_D11_obj)")  & strcmp( #pin_obj , "((&pin_D11_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_TXD3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B0_obj)")  & strcmp( #pin_obj , "((&pin_B0_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH3N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH4_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH3N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH2N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART4_RX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_RXD3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (11) : \
    ((strcmp( #pin_obj , "(&pin_D12_obj)")  & strcmp( #pin_obj , "((&pin_D12_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RGMII_125IN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B1_obj)")  & strcmp( #pin_obj , "((&pin_B1_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_BOOT1_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B2_obj)")  & strcmp( #pin_obj , "((&pin_B2_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH3N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B2_obj)")  & strcmp( #pin_obj , "((&pin_B2_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_ETR_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E7_obj)")  & strcmp( #pin_obj , "((&pin_E7_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART5_TX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E7_obj)")  & strcmp( #pin_obj , "((&pin_E7_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E7_obj)")  & strcmp( #pin_obj , "((&pin_E7_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH1N_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E8_obj)")  & strcmp( #pin_obj , "((&pin_E8_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART5_RX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E8_obj)")  & strcmp( #pin_obj , "((&pin_E8_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D5(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E8_obj)")  & strcmp( #pin_obj , "((&pin_E8_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH1_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E9_obj)")  & strcmp( #pin_obj , "((&pin_E9_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART6_TX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E9_obj)")  & strcmp( #pin_obj , "((&pin_E9_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D6(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E9_obj)")  & strcmp( #pin_obj , "((&pin_E9_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH2N_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E10_obj)")  & strcmp( #pin_obj , "((&pin_E10_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART6_RX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E10_obj)")  & strcmp( #pin_obj , "((&pin_E10_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D7(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E10_obj)")  & strcmp( #pin_obj , "((&pin_E10_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH2_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E11_obj)")  & strcmp( #pin_obj , "((&pin_E11_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART7_TX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E11_obj)")  & strcmp( #pin_obj , "((&pin_E11_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D8(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E11_obj)")  & strcmp( #pin_obj , "((&pin_E11_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH3N_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E12_obj)")  & strcmp( #pin_obj , "((&pin_E12_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART7_RX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E12_obj)")  & strcmp( #pin_obj , "((&pin_E12_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D9(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E12_obj)")  & strcmp( #pin_obj , "((&pin_E12_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH3_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E13_obj)")  & strcmp( #pin_obj , "((&pin_E13_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART8_TX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E13_obj)")  & strcmp( #pin_obj , "((&pin_E13_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D10(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E13_obj)")  & strcmp( #pin_obj , "((&pin_E13_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH4_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E14_obj)")  & strcmp( #pin_obj , "((&pin_E14_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART8_RX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E14_obj)")  & strcmp( #pin_obj , "((&pin_E14_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D11(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E14_obj)")  & strcmp( #pin_obj , "((&pin_E14_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_BKIN_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E15_obj)")  & strcmp( #pin_obj , "((&pin_E15_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D12(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E15_obj)")  & strcmp( #pin_obj , "((&pin_E15_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH3_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B10_obj)")  & strcmp( #pin_obj , "((&pin_B10_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH3_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B10_obj)")  & strcmp( #pin_obj , "((&pin_B10_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_BKIN_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B10_obj)")  & strcmp( #pin_obj , "((&pin_B10_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2C2_SCL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B10_obj)")  & strcmp( #pin_obj , "((&pin_B10_obj))")) == 0) ? (4) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B10_obj)")  & strcmp( #pin_obj , "((&pin_B10_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_RX_ER(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B10_obj)")  & strcmp( #pin_obj , "((&pin_B10_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH4_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B11_obj)")  & strcmp( #pin_obj , "((&pin_B11_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH4_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B11_obj)")  & strcmp( #pin_obj , "((&pin_B11_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_ETR_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B11_obj)")  & strcmp( #pin_obj , "((&pin_B11_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2C2_SDA(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B11_obj)")  & strcmp( #pin_obj , "((&pin_B11_obj))")) == 0) ? (4) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B11_obj)")  & strcmp( #pin_obj , "((&pin_B11_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_TX_EN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B11_obj)")  & strcmp( #pin_obj , "((&pin_B11_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_TX_EN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B11_obj)")  & strcmp( #pin_obj , "((&pin_B11_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_BKIN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2C2_SMBA(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (4) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI2_NSS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2S2_WS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_CK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_CAN2_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (9) : \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (9) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_TXD0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_TXD0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B12_obj)")  & strcmp( #pin_obj , "((&pin_B12_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH1N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI2_SCK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2S2_CK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_CTS_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_CTS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_CAN2_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (9) : \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (9) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_TXD1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_TXD1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B13_obj)")  & strcmp( #pin_obj , "((&pin_B13_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH2N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B14_obj)")  & strcmp( #pin_obj , "((&pin_B14_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI2_MISO(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B14_obj)")  & strcmp( #pin_obj , "((&pin_B14_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_RTS_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B14_obj)")  & strcmp( #pin_obj , "((&pin_B14_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_RTS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B14_obj)")  & strcmp( #pin_obj , "((&pin_B14_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH3N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B15_obj)")  & strcmp( #pin_obj , "((&pin_B15_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI2_MOSI(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B15_obj)")  & strcmp( #pin_obj , "((&pin_B15_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2S2_SD(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B15_obj)")  & strcmp( #pin_obj , "((&pin_B15_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_TX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B15_obj)")  & strcmp( #pin_obj , "((&pin_B15_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH1N_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D8_obj)")  & strcmp( #pin_obj , "((&pin_D8_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_TX_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D8_obj)")  & strcmp( #pin_obj , "((&pin_D8_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_RX_DV(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D8_obj)")  & strcmp( #pin_obj , "((&pin_D8_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D13(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D8_obj)")  & strcmp( #pin_obj , "((&pin_D8_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH1_ETR_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D9_obj)")  & strcmp( #pin_obj , "((&pin_D9_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_RX_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D9_obj)")  & strcmp( #pin_obj , "((&pin_D9_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D14(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D9_obj)")  & strcmp( #pin_obj , "((&pin_D9_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH2N_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D10_obj)")  & strcmp( #pin_obj , "((&pin_D10_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_CK_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D10_obj)")  & strcmp( #pin_obj , "((&pin_D10_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D15(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D10_obj)")  & strcmp( #pin_obj , "((&pin_D10_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH2_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D11_obj)")  & strcmp( #pin_obj , "((&pin_D11_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_CTS_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D11_obj)")  & strcmp( #pin_obj , "((&pin_D11_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_A16(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D11_obj)")  & strcmp( #pin_obj , "((&pin_D11_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM4_CH1_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D12_obj)")  & strcmp( #pin_obj , "((&pin_D12_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH3N_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D12_obj)")  & strcmp( #pin_obj , "((&pin_D12_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_RTS_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D12_obj)")  & strcmp( #pin_obj , "((&pin_D12_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_A17(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D12_obj)")  & strcmp( #pin_obj , "((&pin_D12_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM4_CH2_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D13_obj)")  & strcmp( #pin_obj , "((&pin_D13_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH3_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D13_obj)")  & strcmp( #pin_obj , "((&pin_D13_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_A18(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D13_obj)")  & strcmp( #pin_obj , "((&pin_D13_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM4_CH3_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D14_obj)")  & strcmp( #pin_obj , "((&pin_D14_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_BKIN_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D14_obj)")  & strcmp( #pin_obj , "((&pin_D14_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D14_obj)")  & strcmp( #pin_obj , "((&pin_D14_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM4_CH4_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D15_obj)")  & strcmp( #pin_obj , "((&pin_D15_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM9_CH4_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D15_obj)")  & strcmp( #pin_obj , "((&pin_D15_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D15_obj)")  & strcmp( #pin_obj , "((&pin_D15_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH1_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C6_obj)")  & strcmp( #pin_obj , "((&pin_C6_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C6_obj)")  & strcmp( #pin_obj , "((&pin_C6_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2S2_MCK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C6_obj)")  & strcmp( #pin_obj , "((&pin_C6_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RXP(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C6_obj)")  & strcmp( #pin_obj , "((&pin_C6_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_D6(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C6_obj)")  & strcmp( #pin_obj , "((&pin_C6_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH2_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C7_obj)")  & strcmp( #pin_obj , "((&pin_C7_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C7_obj)")  & strcmp( #pin_obj , "((&pin_C7_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2S3_MCK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C7_obj)")  & strcmp( #pin_obj , "((&pin_C7_obj))")) == 0) ? (5) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RXN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C7_obj)")  & strcmp( #pin_obj , "((&pin_C7_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_D7(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C7_obj)")  & strcmp( #pin_obj , "((&pin_C7_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH3_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C8_obj)")  & strcmp( #pin_obj , "((&pin_C8_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C8_obj)")  & strcmp( #pin_obj , "((&pin_C8_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_TXP(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C8_obj)")  & strcmp( #pin_obj , "((&pin_C8_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_D0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C8_obj)")  & strcmp( #pin_obj , "((&pin_C8_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C8_obj)")  & strcmp( #pin_obj , "((&pin_C8_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH4_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C9_obj)")  & strcmp( #pin_obj , "((&pin_C9_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C9_obj)")  & strcmp( #pin_obj , "((&pin_C9_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_TXN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C9_obj)")  & strcmp( #pin_obj , "((&pin_C9_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_D1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C9_obj)")  & strcmp( #pin_obj , "((&pin_C9_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C9_obj)")  & strcmp( #pin_obj , "((&pin_C9_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_MCO_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A8_obj)")  & strcmp( #pin_obj , "((&pin_A8_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A8_obj)")  & strcmp( #pin_obj , "((&pin_A8_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH1_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A8_obj)")  & strcmp( #pin_obj , "((&pin_A8_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_CK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A8_obj)")  & strcmp( #pin_obj , "((&pin_A8_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_CK_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A8_obj)")  & strcmp( #pin_obj , "((&pin_A8_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_RX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A8_obj)")  & strcmp( #pin_obj , "((&pin_A8_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A9_obj)")  & strcmp( #pin_obj , "((&pin_A9_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH2_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A9_obj)")  & strcmp( #pin_obj , "((&pin_A9_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A9_obj)")  & strcmp( #pin_obj , "((&pin_A9_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_RTS_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A9_obj)")  & strcmp( #pin_obj , "((&pin_A9_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_OTG_FS_VBUS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A9_obj)")  & strcmp( #pin_obj , "((&pin_A9_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A9_obj)")  & strcmp( #pin_obj , "((&pin_A9_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A10_obj)")  & strcmp( #pin_obj , "((&pin_A10_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH3_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A10_obj)")  & strcmp( #pin_obj , "((&pin_A10_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A10_obj)")  & strcmp( #pin_obj , "((&pin_A10_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_CK_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A10_obj)")  & strcmp( #pin_obj , "((&pin_A10_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_OTG_FS_ID(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A10_obj)")  & strcmp( #pin_obj , "((&pin_A10_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A10_obj)")  & strcmp( #pin_obj , "((&pin_A10_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH4_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A11_obj)")  & strcmp( #pin_obj , "((&pin_A11_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_CH4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A11_obj)")  & strcmp( #pin_obj , "((&pin_A11_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_CTS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A11_obj)")  & strcmp( #pin_obj , "((&pin_A11_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_CTS_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A11_obj)")  & strcmp( #pin_obj , "((&pin_A11_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_CAN1_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A11_obj)")  & strcmp( #pin_obj , "((&pin_A11_obj))")) == 0) ? (9) : \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (9) : \
    ((strcmp( #pin_obj , "(&pin_D0_obj)")  & strcmp( #pin_obj , "((&pin_D0_obj))")) == 0) ? (9) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USBDM_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A11_obj)")  & strcmp( #pin_obj , "((&pin_A11_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_OTG_FS_DM(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A11_obj)")  & strcmp( #pin_obj , "((&pin_A11_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_ETR_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM1_ETR(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH1N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_RTS(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_RTS_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_CAN1_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (9) : \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (9) : \
    ((strcmp( #pin_obj , "(&pin_D1_obj)")  & strcmp( #pin_obj , "((&pin_D1_obj))")) == 0) ? (9) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USBDP_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_OTG_FS_DP(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A12_obj)")  & strcmp( #pin_obj , "((&pin_A12_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SWDIO_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A13_obj)")  & strcmp( #pin_obj , "((&pin_A13_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH2N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A13_obj)")  & strcmp( #pin_obj , "((&pin_A13_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SWCLK_NULL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A14_obj)")  & strcmp( #pin_obj , "((&pin_A14_obj))")) == 0) ? (0) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH2N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A14_obj)")  & strcmp( #pin_obj , "((&pin_A14_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH3N(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A14_obj)")  & strcmp( #pin_obj , "((&pin_A14_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART8_TX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A14_obj)")  & strcmp( #pin_obj , "((&pin_A14_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH1_ETR_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A15_obj)")  & strcmp( #pin_obj , "((&pin_A15_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH1_ETR_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A15_obj)")  & strcmp( #pin_obj , "((&pin_A15_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH3N_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A15_obj)")  & strcmp( #pin_obj , "((&pin_A15_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART8_RX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_A15_obj)")  & strcmp( #pin_obj , "((&pin_A15_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_ETR(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C10_obj)")  & strcmp( #pin_obj , "((&pin_C10_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI3_SCK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B3_obj)")  & strcmp( #pin_obj , "((&pin_B3_obj))")) == 0) ? (6) : \
    ((strcmp( #pin_obj , "(&pin_C10_obj)")  & strcmp( #pin_obj , "((&pin_C10_obj))")) == 0) ? (6) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2S3_CK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B3_obj)")  & strcmp( #pin_obj , "((&pin_B3_obj))")) == 0) ? (6) : \
    ((strcmp( #pin_obj , "(&pin_C10_obj)")  & strcmp( #pin_obj , "((&pin_C10_obj))")) == 0) ? (6) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_TX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C10_obj)")  & strcmp( #pin_obj , "((&pin_C10_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART4_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C10_obj)")  & strcmp( #pin_obj , "((&pin_C10_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_D2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C10_obj)")  & strcmp( #pin_obj , "((&pin_C10_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D8(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C10_obj)")  & strcmp( #pin_obj , "((&pin_C10_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C11_obj)")  & strcmp( #pin_obj , "((&pin_C11_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI3_MISO(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B4_obj)")  & strcmp( #pin_obj , "((&pin_B4_obj))")) == 0) ? (6) : \
    ((strcmp( #pin_obj , "(&pin_C11_obj)")  & strcmp( #pin_obj , "((&pin_C11_obj))")) == 0) ? (6) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_RX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C11_obj)")  & strcmp( #pin_obj , "((&pin_C11_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART4_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C11_obj)")  & strcmp( #pin_obj , "((&pin_C11_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_D3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C11_obj)")  & strcmp( #pin_obj , "((&pin_C11_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C11_obj)")  & strcmp( #pin_obj , "((&pin_C11_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_BKIN(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C12_obj)")  & strcmp( #pin_obj , "((&pin_C12_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SPI3_MOSI(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (6) : \
    ((strcmp( #pin_obj , "(&pin_C12_obj)")  & strcmp( #pin_obj , "((&pin_C12_obj))")) == 0) ? (6) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2S3_SD(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (6) : \
    ((strcmp( #pin_obj , "(&pin_C12_obj)")  & strcmp( #pin_obj , "((&pin_C12_obj))")) == 0) ? (6) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART3_CK_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C12_obj)")  & strcmp( #pin_obj , "((&pin_C12_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART5_TX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C12_obj)")  & strcmp( #pin_obj , "((&pin_C12_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_CK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C12_obj)")  & strcmp( #pin_obj , "((&pin_C12_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D9(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_C12_obj)")  & strcmp( #pin_obj , "((&pin_C12_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_ETR_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D0_obj)")  & strcmp( #pin_obj , "((&pin_D0_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D0_obj)")  & strcmp( #pin_obj , "((&pin_D0_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH1_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D1_obj)")  & strcmp( #pin_obj , "((&pin_D1_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_D3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D1_obj)")  & strcmp( #pin_obj , "((&pin_D1_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_ETR(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D2_obj)")  & strcmp( #pin_obj , "((&pin_D2_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_ETR_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D2_obj)")  & strcmp( #pin_obj , "((&pin_D2_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_ETR_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D2_obj)")  & strcmp( #pin_obj , "((&pin_D2_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART5_RX(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D2_obj)")  & strcmp( #pin_obj , "((&pin_D2_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_CMD(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D2_obj)")  & strcmp( #pin_obj , "((&pin_D2_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D11(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D2_obj)")  & strcmp( #pin_obj , "((&pin_D2_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH2_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D3_obj)")  & strcmp( #pin_obj , "((&pin_D3_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_CTS_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D3_obj)")  & strcmp( #pin_obj , "((&pin_D3_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_CLK(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D3_obj)")  & strcmp( #pin_obj , "((&pin_D3_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_RTS_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D4_obj)")  & strcmp( #pin_obj , "((&pin_D4_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_NOE(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D4_obj)")  & strcmp( #pin_obj , "((&pin_D4_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH3_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D5_obj)")  & strcmp( #pin_obj , "((&pin_D5_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_TX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D5_obj)")  & strcmp( #pin_obj , "((&pin_D5_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_NWE(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D5_obj)")  & strcmp( #pin_obj , "((&pin_D5_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_RX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D6_obj)")  & strcmp( #pin_obj , "((&pin_D6_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_NWAIT(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D6_obj)")  & strcmp( #pin_obj , "((&pin_D6_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D10(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D6_obj)")  & strcmp( #pin_obj , "((&pin_D6_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH4_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D7_obj)")  & strcmp( #pin_obj , "((&pin_D7_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART2_CK_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D7_obj)")  & strcmp( #pin_obj , "((&pin_D7_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_NE1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D7_obj)")  & strcmp( #pin_obj , "((&pin_D7_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_NCE2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_D7_obj)")  & strcmp( #pin_obj , "((&pin_D7_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH2_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B3_obj)")  & strcmp( #pin_obj , "((&pin_B3_obj))")) == 0) ? (1) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH1_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B3_obj)")  & strcmp( #pin_obj , "((&pin_B3_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM2_CH2_3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B3_obj)")  & strcmp( #pin_obj , "((&pin_B3_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH1_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B4_obj)")  & strcmp( #pin_obj , "((&pin_B4_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH2_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B4_obj)")  & strcmp( #pin_obj , "((&pin_B4_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART5_TX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B4_obj)")  & strcmp( #pin_obj , "((&pin_B4_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM3_CH2_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH3_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2C1_SMBA(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (4) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART5_RX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_PPS_OUT(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_RMII_PPS_OUT(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B5_obj)")  & strcmp( #pin_obj , "((&pin_B5_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM4_CH1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH1_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2C1_SCL(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (4) : \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (4) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_TX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USBHS_DM(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USBHD_DM(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D5(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B6_obj)")  & strcmp( #pin_obj , "((&pin_B6_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM4_CH2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B7_obj)")  & strcmp( #pin_obj , "((&pin_B7_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH2_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B7_obj)")  & strcmp( #pin_obj , "((&pin_B7_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_I2C1_SDA(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B7_obj)")  & strcmp( #pin_obj , "((&pin_B7_obj))")) == 0) ? (4) : \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (4) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USART1_RX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B7_obj)")  & strcmp( #pin_obj , "((&pin_B7_obj))")) == 0) ? (7) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USBHS_DP(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B7_obj)")  & strcmp( #pin_obj , "((&pin_B7_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_USBHD_DP(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B7_obj)")  & strcmp( #pin_obj , "((&pin_B7_obj))")) == 0) ? (10) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_NADV(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B7_obj)")  & strcmp( #pin_obj , "((&pin_B7_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM4_CH3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_CH3_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART6_TX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_ETH_MII_TXD3(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (11) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_D4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D6(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B8_obj)")  & strcmp( #pin_obj , "((&pin_B8_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM4_CH4(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (2) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM8_BKIN_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_TIM10_CH2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (3) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART6_RX_1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_SDIO_D5(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_DVP_D7(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_B9_obj)")  & strcmp( #pin_obj , "((&pin_B9_obj))")) == 0) ? (13) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART4_TX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E0_obj)")  & strcmp( #pin_obj , "((&pin_E0_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_NBL0(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E0_obj)")  & strcmp( #pin_obj , "((&pin_E0_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_UART4_RX_2(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E1_obj)")  & strcmp( #pin_obj , "((&pin_E1_obj))")) == 0) ? (8) : \
    (0xffffffffffffffffULL))

#define STATIC_AF_FSMC_NBL1(pin_obj) ( \
    ((strcmp( #pin_obj , "(&pin_E1_obj)")  & strcmp( #pin_obj , "((&pin_E1_obj))")) == 0) ? (12) : \
    (0xffffffffffffffffULL))


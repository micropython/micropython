#include STM32_HAL_H

void NUCLEO_H743ZI_board_early_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_GPIOG_CLK_ENABLE();

    // Turn off the USB switch
    GPIO_InitStructure.Pin = GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
}

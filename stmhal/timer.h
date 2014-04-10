// Periodically, the state of the buffer "UserTxBuffer" is checked.
// The period depends on USBD_CDC_POLLING_INTERVAL
// The value is in ms. The max is 65 and the min is 1.
#define USBD_CDC_POLLING_INTERVAL (10)

extern TIM_HandleTypeDef TIM3_Handle;
extern TIM_HandleTypeDef TIM5_Handle;

void timer_tim3_init(void);
void timer_tim5_init(void);

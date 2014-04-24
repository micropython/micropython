// Periodically, the state of the buffer "UserTxBuffer" is checked.
// The period depends on USBD_CDC_POLLING_INTERVAL
// The value is in ms. The max is 65 and the min is 1.
#define USBD_CDC_POLLING_INTERVAL (10)

extern TIM_HandleTypeDef TIM3_Handle;
extern TIM_HandleTypeDef TIM5_Handle;
extern TIM_HandleTypeDef TIM6_Handle;

extern const mp_obj_type_t pyb_timer_type;

void timer_init0(void);
void timer_tim3_init(void);
void timer_tim5_init(void);
void timer_tim6_init(uint freq);

void timer_irq_handler(uint tim_id);

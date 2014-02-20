// Vectors 0-15 are for regular pins
// Vectors 16-22 are for internal sources.
//
// Use the following constants for the internal sources:

#define EXTI_PVD_OUTPUT         (16)
#define EXTI_RTC_ALARM          (17)
#define EXTI_USB_OTG_FS_WAKEUP  (18)
#define EXTI_ETH_WAKEUP         (19)
#define EXTI_USB_OTG_HS_WAKEUP  (20)
#define EXTI_RTC_TIMESTAMP      (21)
#define EXTI_RTC_WAKEUP         (22)

#define EXTI_NUM_VECTORS  23

void exti_init_early(void);
void exti_init(mp_obj_t mod);

uint exti_register(mp_obj_t pin_obj, mp_obj_t mode_obj, mp_obj_t trigger_obj, mp_obj_t callback_obj, mp_obj_t param_obj);

void exti_enable(uint line);
void exti_disable(uint line);
void exti_swint(uint line);

typedef struct {
  mp_obj_t callback;
  mp_obj_t param;
} exti_t;


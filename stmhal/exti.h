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

#define EXTI_NUM_VECTORS        (23)

#define EXTI_MODE_INTERRUPT     (offsetof(EXTI_TypeDef, IMR))
#define EXTI_MODE_EVENT         (offsetof(EXTI_TypeDef, EMR))

#define EXTI_TRIGGER_RISING         (offsetof(EXTI_TypeDef, RTSR))
#define EXTI_TRIGGER_FALLING        (offsetof(EXTI_TypeDef, FTSR))
#define EXTI_TRIGGER_RISING_FALLING (EXTI_TRIGGER_RISING + EXTI_TRIGGER_FALLING)  // just different from RISING or FALLING

void exti_init(void);

uint exti_register(mp_obj_t pin_obj, mp_obj_t mode_obj, mp_obj_t trigger_obj, mp_obj_t callback_obj, void *param);

void exti_enable(uint line);
void exti_disable(uint line);
void exti_swint(uint line);

void Handle_EXTI_Irq(uint32_t line);

typedef struct {
  mp_obj_t callback;
  void *param;
} exti_t;

extern const mp_obj_type_t exti_obj_type;

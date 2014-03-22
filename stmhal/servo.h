extern TIM_HandleTypeDef servo_TIM2_Handle;

void servo_init(void);
void servo_timer_irq_callback(void);

MP_DECLARE_CONST_FUN_OBJ(pyb_servo_set_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_pwm_set_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_Servo_obj);

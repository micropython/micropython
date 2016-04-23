#include "spark_wiring_servo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ATTACHED_SERVO_NUM 13 // Duo only layout 13 PWM pins
#define ASSERT_PIN(pin) if(pin>20)

static Servo _servo[MAX_ATTACHED_SERVO_NUM+1];

static uint8_t pin_to_object_idx[20] = {
    0, // D0
    1, // D1
    2, // D2
    3, // D3
    4, // D4
    MAX_ATTACHED_SERVO_NUM,
    MAX_ATTACHED_SERVO_NUM,
    MAX_ATTACHED_SERVO_NUM,
    MAX_ATTACHED_SERVO_NUM,
    MAX_ATTACHED_SERVO_NUM,
    5, // D8
    6, // D9
    MAX_ATTACHED_SERVO_NUM,
    MAX_ATTACHED_SERVO_NUM,
    7, // D12
    8, // D13
    9, // D14
    10, // D15
    11, // D16
    12, // D17
};

bool servo_attach(uint16_t pin)
{
    ASSERT_PIN(pin){return false;}
    return _servo[ pin_to_object_idx[pin] ].attach(pin);
}

bool servo_attached(uint16_t pin)
{
    ASSERT_PIN(pin){return false;}
    return _servo[ pin_to_object_idx[pin] ].attached();
}

int servo_attachedPin(uint16_t pin)
{
    ASSERT_PIN(pin){return -1;}
    return _servo[ pin_to_object_idx[pin] ].attachedPin();
}

bool servo_detach(uint16_t pin)
{
    ASSERT_PIN(pin){return false;}
    return _servo[ pin_to_object_idx[pin] ].detach();
}

void servo_write(uint16_t pin, int angle)
{
    ASSERT_PIN(pin){return;}
    _servo[ pin_to_object_idx[pin] ].write(angle);
}

int servo_read(uint16_t pin)
{
    ASSERT_PIN(pin){return -1;}
    return _servo[ pin_to_object_idx[pin] ].read();
}

void servo_writeMicroseconds(uint16_t pin, uint16_t pulseWidth)
{
    ASSERT_PIN(pin){return;}
    _servo[ pin_to_object_idx[pin] ].writeMicroseconds(pulseWidth);
}

uint16_t servo_readMicroseconds(uint16_t pin)
{
    ASSERT_PIN(pin){return 0;}
    return _servo[ pin_to_object_idx[pin] ].readMicroseconds();
}

void servo_setTrim(uint16_t pin, int trim)
{
    ASSERT_PIN(pin){return;}
    _servo[ pin_to_object_idx[pin] ].setTrim(trim);
}


#ifdef __cplusplus
}
#endif

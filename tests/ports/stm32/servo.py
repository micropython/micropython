from pyb import Servo

servo = Servo(1)
print(servo)

servo.angle(0)
servo.angle(10, 100)

servo.speed(-10)
servo.speed(10, 100)

servo.pulse_width(1500)
print(servo.pulse_width())

servo.calibration(630, 2410, 1490, 2460, 2190)
print(servo.calibration())

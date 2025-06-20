import pyb
#from machine import PWM

class PWM:
    def __init__(self, ch=0,freq=1000,dtc_per_cent=50):

        if ch > 1 or ch < 0 or freq < 0 or dtc_per_cent > 100 or dtc_per_cent < 0:
            self.__ch = -1
            self.__tim_nr = -1
            self.__freq = -1
            self.__dtc_per_cent = -1
            self.__timer = None
            self.__tim_ch = None
            print("Not a possible channel")
        if ch == 0:
            self.__tim_nr = 11
            self.__timer = pyb.Timer(11, freq=freq)
            self.__tim_ch = self.__timer.channel(1, pyb.Timer.PWM, pin=pyb.Pin.board.PWM0, pulse_width_percent=dtc_per_cent)
        elif ch == 1:
            self.__tim_nr = 2
            self.__timer = pyb.Timer(2, freq=freq)
            self.__tim_ch = self.__timer.channel(1, pyb.Timer.PWM, pin=pyb.Pin.board.PWM1, pulse_width_percent=dtc_per_cent)

        self.__ch = ch
        self.__freq = freq
        self.__dtc_per_cent = dtc_per_cent

        return

    def get_pulse_width_percent(self):
        if self.__ch == -1:
            print("Not a valid PWM")
            return
        return self.__tim_ch.pulse_width_percent()

    def set_pulse_width_percent(self, dtc_per_cent):
        self.__timer.deinit()

        self.__dtc_per_cent = dtc_per_cent

        self.__tim_ch.pulse_width_percent(dtc_per_cent)
        self.__timer.init(freq=self.__freq)

    def get_frequency(self):
        if self.__ch == -1:
            print("Not a valid PWM")
            return
        return self.__freq

    def set_frequency(self, freq):
        self.__timer.deinit()
        self.__freq = freq

        self.__timer.init(freq=self.__freq)


#pwm0 = PWM(0,50,75)
#pwm1 = PWM(1,100000,25)




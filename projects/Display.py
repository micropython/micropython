from observer import Subject, Observer


class Display(Observer):

    _oled = None

    def __init__(self, oled, np=0):
        self._oled = oled
        self._np = np
        self.ao = self.analog_observer(self)
        self.rpm_pixels = 69
        self.cs = self.current_strip(45, 24, np)

    def np_current(self, cur):
        n = round(cur * 4)

        if n > 0:
            for i in range(n):
                self.cs[i] = (0, 0, 100)

        else:
            n = n * -1
            length = len(self.cs) - 1
            for i in range(n):
                self.cs[length - i] = (100, 0, 0)
         
    def np_rpm(self, rpm):
        n = int(rpm/20)
        if n > 0:
            for i in range(n):
                self._np[i] = (0, 100, 0)
        else:
            n = n * -1
            for i in range(n):
                self._np[44 - i] = (100, 0, 0)

    def clear(self):
            # clear
        for i in range(self.rpm_pixels):
            self._np[i] = (0, 0, 0)
       
    def __setitem__(self, key, item):
        _item = str(item)
        if key == "vin" and item:
            self._oled.text("vin:" + _item, 0, 0, 1)
        elif key == "rpm" and item:
            self._oled.text("rpm:" + _item, 0, 12, 1)
            self.np_rpm(item)
        elif key == "avg_input_current" and item:
            self._oled.text("ic:" + _item, 0, 24, 1)        
        elif key == "avg_motor_current" and item:
            self._oled.text("mc:" + _item, 60, 24, 1)
            self.np_current(item)

    def update(self, subject: Subject) -> None:
        self._oled.fill(0)
        self.clear()
        self["vin"] = subject.response.v_in
        self["rpm"] = int(subject.response.rpm/11)  # erpm/poles
        self["avg_input_current"] = subject.response.avg_input_current
        self["avg_motor_current"] = subject.response.avg_motor_current
        self._np.write()
        # self._oled.text("time: " + str(subject.response.time_ms), 0, 36, 1)
        # self._oled.show()
        
    class current_strip:

        def __init__(self, offset, n, strip): 
            self._strip = strip
            self._offset = offset
            self.n = n

        def __setitem__(self, index, value):
            self._strip[self._offset + index] = value

        def __len__(self):
            return self.n
        
    class analog_observer(Observer):

        def __init__(self, parent):
            self.parent = parent

        def update(self, subject: Subject) -> None:
            self.parent._oled.text("th:" + str(int(subject._values[0]/1000)) +
                                   " br:" + str(int(subject._values[1]/1000)), 0, 48, 1)    

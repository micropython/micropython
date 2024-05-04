from observer import Subject, Observer
from lcd_i2c import LCD
from ssd1306 import SSD1306_I2C
from neopixel import NeoPixel


class VESCDisplay:

    def Current(self, icurrent, mcurrent):
        pass

    def rpm(self, rpm):
        pass

    def voltage(self, voltage):
        pass

    def throttle(self, th):
        pass

    def brake(self, br):
        pass

    def show(self):
        pass

    def clear(self):
        pass

# _oled.text(text, 0, 48, 1)
# FrameBuffer.fill_rect(x, y, w, h, c)


class DisplayOLED(VESCDisplay):
    def __init__(self, oled: SSD1306_I2C) -> None:
        self._oled = oled
        super().__init__()

    def rpm(self, rpm):
        self._oled.fill_rect(0, 12, 128, 8, 0)
        self._oled.text("rpm:" + rpm, 0, 12, 1)
        return super().rpm(rpm)

    def Current(self, icurrent, mcurrent):
        self._oled.fill_rect(60, 12, 128, 8, 0)
        self._oled.text("mc:" + icurrent, 60, 24, 1)
        return super().Current(icurrent)
    
    def voltage(self, voltage):
        self._oled.fill_rect(0, 0, 128, 8, 0)
        self._oled.text("vin:" + voltage, 0, 0, 1)
        return super().voltage(voltage)
    
    def throttle(self, th):
        self._oled.fill_rect(0, 48, 64, 8, 0)
        self._oled.text("th:" + str(th), 0, 48, 1)
        return super().throttle(th)
    
    def brake(self, br):
        self._oled.fill_rect(64, 48, 64, 8, 0)
        self._oled.text("br:" + str(br), 64, 48, 1)
        return super().brake(br)
    
    def show(self):
        self._oled.show()
        return super().show()
    

class Display4LineLCD(VESCDisplay):

    def __init__(self, lcd: LCD) -> None:
        self._lcd = lcd
        self.cs = [' '] * 20
        self.active = 100 
        self._lcd.clear()
        # self._lcd.no_backlight()
        super().__init__()

    def clear(self):
        for i, val in enumerate(self.cs):
            self.cs[i] = ' '
        if self.active < 1:
            self._lcd.no_backlight()
        elif self.active > 50:
            self._lcd.backlight()

    def rpm(self, rpm):
        text = "                    "
        if rpm > 0:
            text = ">" * int(rpm/40)
            self.active = 100
        elif rpm == 0:   
            self.active -= 1
                    
        elif rpm < 0:
            rpm = rpm * -1
            text = "<" * int(rpm/40)
            self.active = 100

            # text = (len(text) - 20) * " " + text
        text = text + (20 - len(text)) * " "
        self._lcd.set_cursor(0, 0)
        self._lcd.print(text)
        return super().rpm(rpm)
    
    def Current(self, icurrent, mcurrent):
        n = round(icurrent * 4)        
        if n > 0:
            for i in range(n):
                if i < len(self.cs):
                    self.cs[i] = '>'
            self.active = 100

        elif n < 0:
            n = n * -1
            self.active = 100
            for i in range(n):
                if i < len(self.cs):
                    self.cs[i] = '<'
        else:
            self.active -= 1

        text = "".join(self.cs)
        self._lcd.set_cursor(0, 1)
        self._lcd.print(text)     
        self._lcd.set_cursor(8, 2)
        self._lcd.print("ic:" + str(icurrent))
        
    
    def throttle(self, th):
        text = "th:" + str(th) + " "
        self._lcd.set_cursor(0, 3)
        self._lcd.print(text)
        return super().throttle(th)
    
    def brake(self, br):
        text = "br:" + str(br) + "   "
        self._lcd.set_cursor(8, 3)
        self._lcd.print(text)
        return super().brake(br)
    
    def voltage(self, voltage):
        text = "vi:" + str(voltage)
        self._lcd.set_cursor(0, 2)
        self._lcd.print(text)
        return super().voltage(voltage)


class Display2LineLCD(VESCDisplay):

    def __init__(self, lcd: LCD) -> None:
        self._lcd = lcd
        self._lcd.clear()
        super().__init__()

    def Current(self, icurrent, mcurrent):
        return super().Current(icurrent, mcurrent)
    
    def rpm(self, rpm):
        text = "rpm:" + rpm + "  "
        self._lcd.set_cursor(8, 0)
        self._lcd.print(text)
        return super().rpm(rpm)

    def voltage(self, voltage):
        text = "vi:" + voltage
        self._lcd.set_cursor(0, 0)
        self._lcd.print(text)
        
    def throttle(self, th):
        text = "th:" + str(th) + "   "
        self._lcd.set_cursor(0, 1)
        self._lcd.print(text)
    
    def brake(self, br):
        text = "br:" + str(br) + "   "
        self._lcd.set_cursor(8, 1)
        self._lcd.print(text)
      

class DisplayNeopixel(VESCDisplay):

    def __init__(self, np: NeoPixel) -> None:
        self._np = np
        self.rpm_pixels = 69
        self.cs = self.current_strip(45, 24, np)
        super().__init__()

    def show(self):
        self._np.write()
        return super().show()
    
    def clear(self):
        for i in range(self.rpm_pixels):
            self._np[i] = (0, 0, 0)
        return super().clear()

    def rpm(self, rpm):
        n = int(rpm/20)
        
        if n > 0:
            for i in range(n):
                self._np[i] = (0, 100, 0)
        else:
            n = n * -1
            for i in range(n):
                self._np[44 - i] = (100, 0, 0)

        return super().rpm(rpm)
   
    def Current(self, current, mcurrent):
        n = round(current * 4)
       
        if n > 0:
            for i in range(n):
                self.cs[i] = (0, 0, 100)

        else:
            n = n * -1
            length = len(self.cs) - 1
            for i in range(n):
                self.cs[length - i] = (100, 0, 0)

    class current_strip:

        def __init__(self, offset, n, strip): 
            self._strip = strip
            self._offset = offset
            self.n = n

        def __setitem__(self, index, value):
            self._strip[self._offset + index] = value

        def __len__(self):
            return self.n


class DisplayDispatcher:

    def __init__(self, ds: VESCDisplay):
        self._ds = [ds]    
        self.vo = vesc_obsrver(self)
        self.ao = analog_observer(self)
       
    def append(self, ds: VESCDisplay):
        self._ds.append(ds)


        
'''''''''''''''
    def __setitem__(self, key, item):
        _item = str(item)
        if key == "vin" and item:
            self._ds.voltage(_item)
        elif key == "rpm":
            self.np_rpm(item)
            self._ds.rpm(_item)
        elif key == "avg_input_current" and item:
            if self._oled:
                self._oled.text("ic:" + _item, 0, 24, 1)        
        elif key == "avg_motor_current" and item:
            if self._oled:
                self._oled.text("mc:" + _item, 60, 24, 1)
            self.np_current(item)
'''''


class vesc_obsrver(Observer):

    def __init__(self, parent: DisplayDispatcher):
        self.parent = parent

    def update(self, subject: Subject) -> None:
        for ds in self.parent._ds:
            ds.clear()
            ds.voltage(subject.response.v_in)
            ds.rpm(int(subject.response.rpm/11))
            ds.Current(subject.response.avg_input_current, subject.response.avg_motor_current)
            ds.show()
        # self.parent._ds.Current(subject.response.v_in)
        # self.parent["vin"] = subject.response.v_in
        # self.parent["rpm"] = int(subject.response.rpm/11)  # erpm/poles
        # self.parent["avg_input_current"] = subject.response.avg_input_current
        # self.parent["avg_motor_current"] = subject.response.avg_motor_current
        # self.parent._ds.show()
             

class analog_observer(Observer):

    def __init__(self, parent: DisplayDispatcher):
        self.parent = parent

    def update(self, subject: Subject) -> None:
        for _ds in self.parent._ds:
            _ds.throttle(int(subject._values[0]/1000))
            _ds.brake(int(subject._values[1]/1000))


         

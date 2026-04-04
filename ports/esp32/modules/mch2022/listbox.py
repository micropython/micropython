import display, gc

_activeList = None

# Listbox UI element
class List():
    def __init__(self, x, y, w, h):
        self.fgColor = 0x000000
        self.bgColor = 0xFFFFFF
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.items = []
        self.selected = 0
        global _activeList
        _activeList = self
        self.lines = self.h // (display.getTextHeight(" ", "roboto_regular18")+6)
        self.offset = 0
        self.visible(True)
        self.enabled(True)
    
    def draw(self):
        if self._visible:
            display.drawRect(self.x, self.y, self.w, self.h, True, self.bgColor)
            display.drawRect(self.x, self.y, self.w, self.h, False, self.fgColor)
            cursor = (self.x+1,self.y+1)
            totalHeight = 0
            for i in range(len(self.items)-self.offset):
                cursor = (self.x+1,cursor[1])
                item = self.items[self.offset+i]
                lineHeight = display.getTextHeight(item, "roboto_regular18")
                
                while display.getTextWidth(item, "roboto_regular18") > self.w:
                    item = item[:-1]
                
                totalHeight += lineHeight+6
                if totalHeight >= self.h:
                    break
                color = self.fgColor
                if self.offset+i == self.selected:
                    display.drawRect(self.x, cursor[1], self.w, lineHeight+6, True, self.fgColor)
                    color = self.bgColor
                cursor = (self.x,cursor[1]+3)
                display.drawText(cursor[0]+2, cursor[1], item+"\n", color, "roboto_regular18")
                cursor = (self.x,cursor[1]+3+display.getTextHeight(item+"\n", "roboto_regular18"))
    
    def add_item(self, caption):
        if type(caption) == type(""):
            i = self.items.append(caption)
        elif type(caption) == type(b""):
            i = self.items.append(caption.decode('utf-8'))
        else:
            i = self.items.append(str(caption))
        if self._enabled:
            self.draw()
        return i
    
    def count(self):
        return len(self.items)
    
    def selected_text(self):
        return self.items[self.selected]
    
    def remove_item(self, pos):
        self.items.pop(pos)
        if self.selected >= pos and self.selected > 0:
            self.selected -= 1
        if self._enabled:
            self.draw()
        #print("Remove item from pos", pos,"selected",self.selected)
    
    def selected_index(self, setValue=None):
        if setValue:
            self.selected = setValue
            if self.selected < self.offset:
                self.offset = self.selected
            if self.selected >= self.offset+self.lines:
                self.offset = self.selected+self.lines-1
            self.draw()
        else:
            return self.selected
    
    def destroy(self):
        global _activeList
        self.items = []
        self.selected = 0
        _activeList = None
        
    def moveUp(self):
        if self.selected > 0:
            self.selected-=1
            if self.selected < self.offset:
                self.offset = self.selected
        else:
            self.selected = len(self.items)-1
            if len(self.items) >= self.lines:
                self.offset = self.selected - self.lines + 1
            else:
                self.offset = self.selected - len(self.items) + 1
        self.draw()
            
    def moveDown(self):
        if self.selected < len(self.items)-1:
            self.selected+=1
            if self.selected >= self.offset+self.lines:
                self.offset += 1
        else:
            self.selected = 0
            self.offset = 0
        self.draw()
    
    def visible(self, arg):
        self._visible = arg
        self.draw()

    def enabled(self, val):
        global _activeList, _listUpCallback, _listDownCallback
        self._enabled = val
    
    def clear(self):
        while len(self.items): #Keep the same list!
            self.items.pop()
        self.offset = 0
        self.selected = 0
        gc.collect()

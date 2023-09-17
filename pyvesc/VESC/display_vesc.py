
import tkinter as tk


class Labels:
    tk_labels =[]
    
    def __init__(self,labels):
        self.root = tk.Tk()
        self.labels=labels
        for label in labels:
            tk_label = tk.Label(self.root, text=label,font=("Arial", 25),anchor="w",justify=tk.LEFT, bg="red")
            tk_label.pack(fill=tk.X)
            self.tk_labels.append(tk_label)
        
    def __setitem__(self, i, v):
         text=str(v)+" "+self.labels[i]
         self.tk_labels[i].config(text=text)
        
    def __len__(self):
        return len(self.labels)
    

    def Update(self):
         self.root.update()
        


class Display:
    rpm=0
    avg_input_current=0
    v_in=0
    def Update(self):
        print(self.rpm)







class window(Display):

    def __init__(self):
        self.window = tk.Tk()
        self.frame = tk.Frame(master=self.window, width=150, height=150)
        self.frame.pack()

        self.labelrpm = tk.Label(master=self.frame,text="rpm",bg="red")
        self.labelrpm.place(x=0, y=0)

        self.labelavg_input_current = tk.Label(master=self.frame, text="I'm at (75, 75)", bg="yellow")
        self.labelavg_input_current.place(x=75, y=25)

        self.labelv_in = tk.Label(master=self.frame, text="I'm at (0, 75)", bg="green")
        self.labelv_in.place(x=0, y=25)

    
    def Update(self):
        self.labelrpm.config(text=str(self.rpm)+" rpm" )
        self.labelavg_input_current.config(text=str(self.avg_input_current)+" amps" )
        self.labelv_in.config(text=str(self.v_in)+" volts" )
        self.window.update()

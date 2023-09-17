
from pyvesc.VESC.display_vesc import Labels
from gi.repository import GLib


l = Labels(["one","two"])

def send_ping():
    print("ping")
    

print("main loop")

l.mainloop()
print("exiting")







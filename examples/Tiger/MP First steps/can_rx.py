from pyb import CAN, millis


def print_can_stat(stat_list): # stat of the can
    print()
    print("=== CAN INFOS ===")
    print("TEC Value: " + str(stat_list[0]))
    print("REC Value: " + str(stat_list[1]))
    print("Error Warning State: " + str(stat_list[2]) + " times")
    print("Error Passive State: " + str(stat_list[3]) + " times")
    print("Bus Off State: " + str(stat_list[4]) + " times")
    print("Pending TX messages: " + str(stat_list[5]))
    print("Pending RX messages in FIFO 0: " + str(stat_list[6]))
    print("Pending RX messages in FIFO 1: " + str(stat_list[7]))
    print("=== CAN INFOS ===")
    print()

def cb_fifo_0(bus, reason): #function for callback of the fifo 0
    #print(bus)
    if reason == 0:
        return

    print(" ------ FIFO INTERRUPT -----")
    if reason == 1:
        print('Reason: full')
    if reason == 2:
        print('Reason: overflow')
    print(" ------ END FIFO INTERRUPT -----")


bt0 = 0x10 #bt0 from tiger example
bt1 = 0x45 #bt1 from tiger example
sjw = bt0 >> 6 #sjw from our C code
ts1 = bt1 & 0x0F #ts1 from our C code
ts2 = ((bt1 & 0x70) >> 4) #ts2 from our C code

#baudrate and sample point used based in baudrate and bt0 & bt1 of our Tiger CAN example
can = CAN("CAN", CAN.NORMAL, prescaler=82,sjw=sjw, bs1=ts1,bs2=ts2,auto_restart=True, baudrate=50000,sample_point=65)
can.setfilter(bank=0,mode=CAN.MASK16,fifo=0,params=(0x50A0, 0xFFFF, 0x0155, 0xFFFF))#only works with the dirct id, the 0x50A0 makes no difference
can.rxcallback(0,cb_fifo_0)#callback for the fifo changing

buf = bytearray(8)#array to  reiceve the data
lst = [0, 0, 0, 0, memoryview(buf)]#memoryview allocs buf in heap so it does not need to allocate each loop
t0 = millis()#initial time
ts = millis()#final time

while ts - t0 < 5000:#if elapsed time is less than 5 seconds
    ts = millis()#last time
    try:
        if can.any(0):#if there is data in fifo
            can.recv(0,lst)#receove the sata in the list
            print("ID: " + str(lst[0]))#prints the id of the receiver
            print("Message: " + str(buf.decode('utf-8')))#prints the messsage
    except KeyboardInterrupt:#in case of a keyboard interrup
        print_can_stat(can.info())#prints the can state
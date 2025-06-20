
from pyb import CAN, millis


def print_can_stat(stat_list):
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


#tiger_basic_clk = 50_000_000
bt0 = 0x10#bt0 from tiger example
bt1 = 0x45#bt1 from tiger example
#brp = ((((bt0 & 0x3F) + 1 ) * 5) - 1)
sjw = bt0 >> 6 #sjw from our C code
ts1 = bt1 & 0x0F #ts1 from our C code
ts2 = ((bt1 & 0x70) >> 4) #ts2 from our C code

#tiger_python_clk = freq()[2] # clock frequency

#prescaler = (( tiger_python_clk * (brp+1) ) / tiger_basic_clk) #calculates the ratio of the MicroPython clock and Tiger Clock

#Using bs1 and bs2 it did not work, so I had to use Baudrate and Sample point to emulate the same rewsults of the tiger example

#baudrate and sample point used based in baudrate and bt0 & bt1 of our Tiger CAN example
can = CAN("CAN", CAN.NORMAL, prescaler=82,sjw=sjw, bs1=ts1,bs2=ts2,auto_restart=True, baudrate=50000,sample_point=65)


data = 'the quick brown fox jumps over the lazy dog' #data to be sent
i_msg = 0 #nibble of the mesage to be sent
t_id  = 0x155 << 5 #id of the message


t0 = millis()#initial time
ts = millis()#final time

while ts - t0 < 5000:#if elapsed time is less than 5 seconds

    if i_msg >= (len(data) - 8): #if the position is bigger then the len
        i_msg = 0
    msg = data[i_msg:i_msg+8]  #nibble to be sent
    can.send(msg, t_id, timeout=200) #send msg wit t_id timeout 200 ms
    i_msg = i_msg + 1 # increments the nibble
    ts = millis() #last time
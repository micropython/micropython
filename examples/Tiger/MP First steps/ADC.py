from machine import ADC, Pin

adc_number = 12 #Number of adcs presetn in the MCU

adcs = [] #list of adcs
adc_prefix = "AN" #prefix of the name of the pins

for i in range (adc_number): #loop through the adcs
    if i < 10: #if the number is lower than 10 a '0' is needed to be added to the name
        adc_name = adc_prefix + str('0') + str(i) # Name is, for example AN + 0 + 3 = AN03
    else:
        adc_name = adc_prefix + str(i) #if the number is 10 or bigger the name is AN + 11 = AN11

    adc = ADC(Pin(adc_name)) #creates the ADC object for that pin
    adcs.append(adc) #appens on the adc list

val_u16 = 0 #val of the samples

for i in range(adc_number): #runs through the ADC list
    for j in range (1000): #reads 1000 samples
        val_u16 += (adcs[i].read_u16()/0xFFFF) #takes a 16 bit sample
    val_u16 = (val_u16/1000)*5.0 #calculates the average of the sample sum

    print(("ADC " + str(i) + " value from 16 bytes measurement: " + str(val_u16) + "V"))
    val_u16 = 0 # resets the counter

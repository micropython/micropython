Example Description

This example describes how to use ADC.


1.Prepare a DC power supply to provide a adjustable voltage.

2.Connect anode to HDK board A3, and cathode to GND
  
3.Run the main function.

4.Will see result like below

AD1:00008049 = 1644 mv, AD2:00002a75 = 17 mv, AD3:00002a94 = 20 mv


NOTE: 
	1. For 8195AM EVB, A0 and A1 are hardware connected. A2 is also available.
	   For 8711AM EVB, A0 and A1 are not available. Only A2 is avaliable.
	2. ADC need calibration to get correct voltage value by modifing OFFSET and GAIN_DIV.




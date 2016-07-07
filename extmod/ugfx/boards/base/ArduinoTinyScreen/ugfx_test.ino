#include <Wire.h>
#include <SPI.h>
#include <gfx.h>

void setup() {
  coord_t		width, height;
  coord_t		i, j;

  pinMode(13, OUTPUT);

  // Initialize and clear the display
  gfxInit();

#if 1
  // Get the screen size
  width = gdispGetWidth();
  height = gdispGetHeight();

  // Code Here
  gdispDrawBox(10, 10, width / 2, height / 2, Yellow);
  gdispFillArea(width / 2, height / 2, width / 2 - 10, height / 2 - 10, Blue);
  gdispDrawLine(5, 30, width - 50, height - 40, Red);

  for (i = 5, j = 0; i < width && j < height; i += 7, j += i / 20)
    gdispDrawPixel(i, j, White);
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);
  //gfxSleepMilliseconds(500);
  digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(500);
  //gfxSleepMilliseconds(500);
}


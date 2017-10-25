#include "Adafruit_FreeTouch.h"

Adafruit_FreeTouch qt_1 = Adafruit_FreeTouch(A0, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch qt_2 = Adafruit_FreeTouch(A1, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch qt_3 = Adafruit_FreeTouch(A2, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch qt_4 = Adafruit_FreeTouch(A3, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

void setup() {
  Serial.begin(115200);
  
  while (!Serial);
  Serial.println("FreeTouch test");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  if (! qt_1.begin())  
    Serial.println("Failed to begin qt on pin A0");
  if (! qt_2.begin())  
    Serial.println("Failed to begin qt on pin A1");
  if (! qt_3.begin())  
    Serial.println("Failed to begin qt on pin A2");
  if (! qt_4.begin())  
    Serial.println("Failed to begin qt on pin A3");

}



void loop() {
  int counter, result = 0;
  
  // DIY
  Serial.println("\n*************************************");

  counter = millis();
  result = qt_1.measure(); 
  Serial.print("QT 1: "); Serial.print(result);
  Serial.print(" (");  Serial.print(millis() - counter); Serial.println(" ms)");

  counter = millis();
  result = qt_2.measure(); 
  Serial.print("QT 2: "); Serial.print(result);
  Serial.print(" (");  Serial.print(millis() - counter); Serial.println(" ms)");

  counter = millis();
  result = qt_3.measure(); 
  Serial.print("QT 3: "); Serial.print(result);
  Serial.print(" (");  Serial.print(millis() - counter); Serial.println(" ms)");
  
  counter = millis();
  result = qt_4.measure(); 
  Serial.print("QT 4: "); Serial.print(result);
  Serial.print(" (");  Serial.print(millis() - counter); Serial.println(" ms)");
  
  delay(200);
}
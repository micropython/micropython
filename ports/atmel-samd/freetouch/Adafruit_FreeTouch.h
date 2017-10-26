#ifndef ADAFRUIT_FREETOUCH_H
#define ADAFRUIT_FREETOUCH_H

#include <Arduino.h>
#include "adafruit_ptc.h"

class Adafruit_FreeTouch {
 public:
  Adafruit_FreeTouch(int p = 0, oversample_t f = OVERSAMPLE_4, series_resistor_t r = RESISTOR_0, freq_mode_t fh = FREQ_MODE_NONE);
  bool begin(void);

  uint16_t measure(void);
  uint16_t measureRaw(void);

 private:
  void ptcInitSettings(void);
  void ptcConfigIOpin(void);
  uint16_t startPtcAcquire(void);

  int  getYLine(void);
  void selectYLine(void);
  void setOversampling(oversample_t lvl);
  void setSeriesResistor(series_resistor_t res);
  void setFreqHopping(freq_mode_t fh, freq_hop_t hops = FREQ_HOP_1);
  void setCompCap(uint16_t cc);
  void setIntCap(uint8_t ic);

  void snapshotRegsAndPrint(uint32_t base, uint8_t numregs);
  void printHex(uint8_t h, boolean newline);
  void printPTCregs(uint32_t base, uint8_t *regs, uint8_t num);

 private:
    int pin;           // arduino pin #
    struct adafruit_ptc_config config;
};

#endif

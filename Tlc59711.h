//
// Arduino library for controlling TI's TLC59711
//
// 21 Feb 2016 by Ulrich Stern
//
// open source (see LICENSE file)
//
// notes:
// * the library's Wiki on GitHub has a comparison with the Adafruit_TLC59711
//  library and various other info
//

#ifndef TLC59711_H
#define TLC59711_H

#include <Arduino.h>

class Tlc59711 {

public:
  // object default state: (matches reset())
  // - all grayscale (PWM) values 0
  // - all brightness values 127 (maximum)
  Tlc59711(uint16_t numTlc, uint8_t clkPin = 13, uint8_t dataPin = 11);

  ~Tlc59711();

  // select data transfer mode
  // * beginFast():
  //  - use hardware support (SPI library)
  //  - default: 10 MHz SPI clock (TLC59711 datasheet value)
  // * beginSlow():
  //  - use software (shiftOut(), which is digitalWrite()-based)
  // * if this library is NOT used on an Arduino Uno or with 10 MHz,
  //  postXferDelayMicros possibly needs to be adjusted; see write()
  //  implementation and the library's GitHub Wiki for details
  void beginFast(bool bufferXfer = true, uint32_t spiClock = 10000000,
    unsigned int postXferDelayMicros = 4);
  void beginSlow(unsigned int postXferDelayMicros = 200,
    bool interrupts = false);
  // control of TMGRST bit
  // * separate from begin() functions to reduce their parameters
  // * side effect: resets all brightness values to maximum (127)
  void setTmgrst(bool val = true);

  // the following functions affect the state of this object only (the data
  // is transferred to the chip when write() is called)
  // * index order follows the datasheet
  //   (note: chip 1 is the one directly connected to Arduino)
  //  - channel: 0:R0 (chip 1), 1:G0, ..., 12:R0 (chip 2), ...
  //  - RGB/LED: 0:R0,G0,B0 (chip 1), ..., 4:R0,G0,B0 (chip 2), ...
  //  - tlcIdx: 0:chip 1, 1:chip 2, ...
  // * functions without index set values for all chips and LEDs
  // * brightness values (bcr, bcg, bcb) must be 0-127
  void setChannel(uint16_t idx, uint16_t val);
  void setRGB(uint16_t idx, uint16_t r, uint16_t g, uint16_t b);
  void setRGB(uint16_t r = 0, uint16_t g = 0, uint16_t b = 0);
  void setLED(uint16_t idx, uint16_t r, uint16_t g, uint16_t b) {
    setRGB(idx, r, g, b);
  }
    // setLED() is convenience function to allow faster testing/porting of
    // code using Adafruit_TLC59711; the index order for multiple chips is
    // different here, however, and chip 1 has the smallest indices
  void setBrightness(uint16_t tlcIdx, uint8_t bcr, uint8_t bcg, uint8_t bcb);
  void setBrightness(uint8_t bcr = 127, uint8_t bcg = 127, uint8_t bcb = 127);
  void reset() {
    setRGB();
    setBrightness();
  }

  // transfer data to TLC59711; will work only after a call to beginFast() or
  // beginSlow()
  void write();

  // stop using this library
  void end();

private:
  const uint16_t numTlc, bufferSz;
  const uint8_t clkPin, dataPin;
  uint16_t *buffer, *buffer2;
  bool beginCalled;
  uint8_t fc;
  bool useSpi_, bufferXfer_, noInterrupts;
  unsigned int postXferDelayMicros_;

  void begin(bool useSpi, unsigned int postXferDelayMicros);
  void xferSpi();
  void xferSpi16();
  void xferShiftOut();
};

#endif


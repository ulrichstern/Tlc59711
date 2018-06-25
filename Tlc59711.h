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

#ifndef TLC59711_H_
#define TLC59711_H_

#include <Arduino.h>

class Tlc59711 {
 public:
  // object default state: (matches reset())
  // - all grayscale (PWM) values 0
  // - all brightness values 127 (maximum)
  explicit Tlc59711(uint16_t numTlc, uint8_t clkPin = 13, uint8_t dataPin = 11);

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
  uint16_t getChannel(uint16_t idx);
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
  // const uint16_t idx_lookup;
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generate lookup table at compile time
// based on stackoverflow questions
// Is it possible to create and initialize an array of values
// using template metaprogramming?
// https://stackoverflow.com/a/2228298/574981
// https://stackoverflow.com/a/37447199/574981

// template <uint16_t idx>
// struct idx_lookup_generator {
//     idx_lookup_generator<idx - 1> rest;
//     static const uint16_t x = (14 * (idx / 12) + idx % 12);
//     // static const uint16_t x = idx * idx;
//     constexpr uint16_t operator[](uint16_t const &i) const {
//         return (i == idx ?  x : rest[i]);
//     }
//     constexpr uint16_t size() const {
//         return idx;
//     }
// };
//
// template <>
// struct idx_lookup_generator<0> {
//     static const uint16_t x = 0;
//     constexpr uint16_t operator[](uint16_t const &i) const { return x; }
//     constexpr uint16_t size() const { return 1; }
// };

// template <int idx>
// struct idx_lookup_generator {
//     idx_lookup_generator<idx - 1> rest;
//     int x;
//     idx_lookup_generator() : x(14 * (idx / 12) + idx % 12) {}
// };
//
// template <>
// struct idx_lookup_generator<1> {
//     int x;
//     idx_lookup_generator() : x(0) {}
// };

// maximum elements is 56173 (as this translates to 65535)
// const idx_lookup_generator<96> idx_lookup_table;
const uint16_t idx_lookup_table[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123,
    126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
    140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151,
    154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165,
    168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193,
    196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235,
    238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
    252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263,
    266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277,
    280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291
};


#endif  // TLC59711_H_

//
// "Latch pulse" test for Adafruit_TLC59711 library
//
// 12 Mar 2016 by Ulrich Stern
//
// open source (see LICENSE file)
//
// notes:
// * for the Tlc59711 library, this test is part of Tlc59711Test
// * for this test
//  - the red LEDs should be constant on
//  - the green and blue LEDs should blink (1s on, 1s off)
// * the test fails as follows
//  - the red LEDs change (every second) between 2 intensities
//  - the blue LEDs do not turn off fully
// * reason: see GitHub Wiki of the Tlc59711 library
//

#include <Adafruit_TLC59711.h>

const int NUM_TLC = 1;

Adafruit_TLC59711 tlc(NUM_TLC);
long writesPerSecond;

void setup() {
  tlc.begin();
  unsigned long us = micros();
  tlc.write();
  writesPerSecond = 1000000L / (micros() - us);
}

void loop() {
  uint16_t tv = 0x96c0;
    // MSBs: 0b100101 (0x25), then 0b10110 (OUTTMG, ...)
  for (int j=0; j<2; j++) {
    for (int i=0; i<4*NUM_TLC; i++)
      tlc.setLED(i, tv, tv*j, tv*j);
    for (long i=0, n=writesPerSecond; i<n; i++)
      tlc.write();
  }
}


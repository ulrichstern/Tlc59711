//
// "Hello World" for Tlc59711 library
//
// 27 Feb 2016 by Ulrich Stern
//
// open source (see LICENSE file)
//

#include <Tlc59711.h>

const int NUM_TLC = 2;

Tlc59711 tlc(NUM_TLC);

void setup() {
  tlc.beginFast();
}

void loop() {
  for (int i=0; i<2; i++) {
    tlc.setRGB(65535*i, 6554*i, 655*i);
    tlc.write();
    delay(200);
  }
}


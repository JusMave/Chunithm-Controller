/*********************************************************
This is a library for the MPR121 12-channel Capacitive touch sensor

Designed specifically to work with the MPR121 Breakout in the Adafruit shop 
  ----> https://www.adafruit.com/products/

These sensors use I2C communicate, at least 2 pins are required 
to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution
**********************************************************/

#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "Keyboard.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void setup() {
  SerialUSB.begin(9600);

  while (!SerialUSB) { // needed to keep leonardo/micro from starting too fast!00000
    delay(10);
  }
  
  SerialUSB.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    SerialUSB.println("MPR121 not found, check wiring?");
    while (1);
  }
  SerialUSB.println("MPR121 found!");
  Keyboard.begin();
}

void loop() {
  // Get the currently touched pads
  currtouched = cap.touched();
  
  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      SerialUSB.print(i); SerialUSB.println(" touched");
      Keyboard.press('A');
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      SerialUSB.print(i); SerialUSB.println(" released");
      Keyboard.release('A');
    }
  }

  // reset our state
  lasttouched = currtouched;

  // comment out this line for detailed data from the sensor!
  return;
  
  // debugging info, what
  SerialUSB.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); SerialUSB.println(cap.touched(), HEX);
  SerialUSB.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
    SerialUSB.print(cap.filteredData(i)); SerialUSB.print("\t");
  }
  SerialUSB.println();
  SerialUSB.print("Base: ");
  for (uint8_t i=0; i<12; i++) {
    SerialUSB.print(cap.baselineData(i)); SerialUSB.print("\t");
  }
  SerialUSB.println();
  
  // put a delay so it isn't overwhelming
  delay(100);
}

#include "HID-Project.h"

const int pinLed = LED_BUILTIN;
const int pinButton = 2;
int teststring;

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  // Sends a clean report to the host. This is important on any Arduino type.
  Keyboard.begin();
  Keyboard.begin();
  Keyboard.begin();
}
//
void loop() 
{
  
  // Hold a lot of keys at the same time

    //digitalWrite(pinLed, HIGH);
    //ABCDEFABCDEFABCDEFABCDEFABCDEF
    // ADo not press to many at once or some OS will have problems.
    // Note that the resulting pressed order might differ,
    // because all keys are pressed at the same time.
    //ABABABABABABAB
    //Keyboard.press('A');
    //Keyboard.press('B');
    /*
    Keyboard.press('C');
    Keyboard.press('D');
    Keyboard.press('E');
    Keyboard.press('F');
    Keyboard.press('G');
    Keyboard.press('H');
    */
    Keyboard.releaseAll();
   
    /*0000
    SerialUSB.println("XXXXX");
    NKROKeyboard.press('B');
    NKROKeyboard.release('B');
    SerialUSB.println("CCCC");
    */
    /*
    NKROKeyboard.press('0');
    NKROKeyboard.releaseAll();
    */
    delay(4000);
    // Release all keys and hit enter
    /*
    NKROKeyboard.releaseAll();
    NKROKeyboard.println();
    */
    // Simple debounce
    delay(300);
}

/*
 *
 * Encoder test program.
 * 
 * Plug ChA from encoder into PIN 2
 * Plug ChB from encoder into PIN 3
 * Plug Idx from encoder into PIN 4 (Optional)
 * Plug GND and 5V from Encoder into Arduino
 * 
 * Optional
 * 
 * LED on PIN 8 will show state of ChA (Green LED)
 * LED on PIN 10 will show state of ChB (Blue LED)
 * LED on PIN 12 will show state of Idx (Red LED)
 * 
 */

// Constants in the range of 0 to 1023 corresponding to
// the maximum and minimum values used in computing
// the analogWrite() when controlling a Victor PWM.
// (we want the pulse to be high in for 1.0 to 2.0 milliseconds
// where 1.5 milliseconds tells the victor to stop)
const int victorInMin = 251;
const int victorInMax = 494;
const int victorInStop = (victorInMin + victorInMax) / 2;

// constants won't change. They're used here to
// set pin numbers:

// LED output pins for each channel of encoder
const int ledA = 8;
const int ledB = 10;
const int ledI = 12;

// Input PINS for data from encoder
const int CHA_PIN = 2;
const int CHB_PIN = 3;
const int IDX_PIN = 4;
const int REPORT_PIN = 7;

// Used to control speed controller
const int MOTOR0_PIN = 5;
// User can use to set speed control
const int SPEED0_PIN = 0;

// Relay (and two digital outputs)
const int FWD_PIN = 11;
const int REV_PIN = 13;

String padLeading(String sval, int places) {
  int slen = sval.length();
  
  const int maxPlaces = 20;
  places = min(places, maxPlaces);
  
  if (slen < places) {
    // 20 space characters
    const char* spaces = "                    ";
    String pad(&spaces[maxPlaces - (places - slen)]);
    sval = pad + sval;
  }
  return sval;
}
  
String formatFixed(unsigned long val, int places) {
  String valStr(val);
  return padLeading(valStr, places);
}

// WARNING: Because we played with the timers for PWM control
// we need to correct the time returned by micros() (we would
// be better off to move the PWM output line to a pin not
// associated with the timer used for micros()/millis().

unsigned long getTime() {
  unsigned long now = micros();
  now <<= 3;
  return now;
}


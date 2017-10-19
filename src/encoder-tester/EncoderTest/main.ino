// Used for timing
unsigned long now = 0;

// Set up our encoder/digital inputs
Encoder enc(CHA_PIN, CHB_PIN, now);
const DigitalInput& chA = enc.getChA();
const DigitalInput& chB = enc.getChB();
DigitalInput idx(IDX_PIN, now);
DigitalInput reportButton(REPORT_PIN, now);

// Relay control pins and a counter used
// to cycle between states
int relayFwd = FWD_PIN;
int relayRev = REV_PIN;
int relayIdx = 0;

// Used when the PWM output is enable to contol a motor
int enableSpeedCtl = 0;
int motor = MOTOR0_PIN;
int speed = 0;

// Used to track how many encoder counts happend in one
// revolution.
long encCountsLast = 0;
long countsPerRev = 0;

//
// Updates the output state of the relay to allow
// us to cycle from off, to forward, to both, to reverse.
//
void updateRelay() { 
  // Make the digital out sequence something we
  // can feed into the encoder
  const int fwdVals[] = { 0, 1, 1, 0 };
  const int revVals[] = { 0, 0, 1, 1 };
  
  digitalWrite(relayFwd, fwdVals[relayIdx]);
  digitalWrite(relayRev, revVals[relayIdx]);
  
  relayIdx = (relayIdx + 1) % 4;
}

//
// Zeros out accumulated counts and times - typically
// done after printing a report.
//
void zero() {
  now = getTime();
  enc.zero(now);
  idx.zero(now);
  reportButton.zero(now);
  countsPerRev = 0;
  encCountsLast = enc.getCount();

  relayIdx = 0;  
  updateRelay();
      
  enableSpeedCtl = false;
  analogWrite(motor, 0);
}

//
// This is a helper method to maximize the range of user input allowing
// the user to make full use of the potentiometer's range when adjusting speed.
//

unsigned analogToPwm(unsigned ain, unsigned fullRev, unsigned stop, unsigned fullFwd) {
  const unsigned maxIn = 1024;
  const unsigned zeroZone = maxIn / 20;
  const unsigned midPt = maxIn / 2;
  const unsigned revInMin = 0;
  const unsigned revInMax = midPt - zeroZone;
  const unsigned revInRange = revInMax - revInMin;
  const unsigned fwdInMin = midPt + zeroZone;
  const unsigned fwdInMax = maxIn;
  const unsigned fwdInRange = fwdInMax - fwdInMin;

  unsigned pwmOut = stop;
  
  if (ain <= revInMax) {
    const unsigned ofs = max(ain, revInMin);
    pwmOut = fullRev + (ofs * (stop - fullRev) / revInRange);
  } else if (ain >= fwdInMin) {
    const unsigned ofs = min(ain, maxIn) - fwdInMin;
    pwmOut = stop + (ofs * (fullFwd - stop) / fwdInRange);
  }

  return pwmOut / 4;
  
}

//
// Dump information about the state of the sensors to
// the console (this has more information than you will
// see just looking at the LEDs on the board).
//
void dumpReport() {
  Serial.print("Enc: ");
  if (countsPerRev != 0) {
    Serial.print(enc.toString());
    Serial.print("  CPR: ");
    Serial.println(abs(countsPerRev));
  } else {
    Serial.println(enc.toString());  
  }
  Serial.print("ChA: ");
  Serial.println(chA.toString());

  Serial.print("ChB: ");
  Serial.println(chB.toString());

  Serial.print("Idx: ");
  Serial.println(idx.toString());
  
  Serial.print("Rep: ");
  Serial.println(reportButton.toString());
  
  Serial.print("Spd: Raw:");
  Serial.print(speed);
  Serial.print(" volts:");
  Serial.print(speed * (5.0 / 1024));
  Serial.println();

  Serial.println();
}

//
// Check to see if we need to respond to any button presses
// or adjust PWM outputs based on user input.
//
// * Button quick press toggles speed control on/off.
// * Button quick press also iterates through relay states.
// * Button long press displays report and zeros values.
//
// Analog reads are apparently slow on an Arduino, so you
// really want to skip the PWM control code when testing
// an encoder.
//
// Likewise, serial writes are extremely slow. So, we zero
// counts after printing a report.
// 
void checkUserInput() {  
  int repChanged = reportButton.getChanged();
  int repPressed = !reportButton.getState();
  
  if (enableSpeedCtl) {
    speed = analogRead(SPEED0_PIN);
    //analogWrite(motor, speed / 4);

    // Map to range supported by Victor
    int pwmOut = analogToPwm(speed, victorInMin, victorInStop, victorInMax);
    analogWrite(motor, pwmOut);
  }

  // See if user button was just released
  if (repChanged && !repPressed) {
    unsigned long dur = reportButton.getPriorDuration();

    if (dur >= 1000000) {
      // Button was held down for at least 1 second,
      // then zero out values and cut off speed control
      dumpReport();
      zero();
    } else if (dur > 10000) {
      // Button was held down for at least 0.01 seconds,
      // change state of relay and whether we adjust speed
      updateRelay();
      enableSpeedCtl = !enableSpeedCtl;
    }
  }
}

void setup() {
  // Initialize LEDs to current state of encoders
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(ledI, OUTPUT);
  
  pinMode(motor, OUTPUT);

  // From: https://arduino-info.wikispaces.com/Arduino-PWM-Frequency
  // For Arduino Uno, Nano, Micro Magician, Mini Driver, Lilly Pad and any other board using ATmega 8, 168 or 328**
  //---------------------------------------------- Set PWM frequency for D5 & D6 ------------------------------- 
  // NOTE: This timer setting messes up the millis() and micros() methods
  //TCCR0B = TCCR0B & B11111000 | B00000001;    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
  //TCCR0B = TCCR0B & B11111000 | B00000010;    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
  //  TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz (The DEFAULT)
  TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
  //TCCR0B = TCCR0B & B11111000 | B00000101;    // set timer 0 divisor to  1024 for PWM frequency of    61.04 Hz

  pinMode(relayFwd, OUTPUT);
  pinMode(relayRev, OUTPUT);

  digitalWrite(ledA, chA.getState());
  digitalWrite(ledB, chB.getState());
  digitalWrite(ledI, idx.getState());

  Serial.begin(38400);
  Serial.println("Initialization complete, user input button now active");
  zero();
}

void loop() {
  // Get current time
  now = getTime();

  // Update digital input readings
  enc.update(now);
  idx.update(now);
  reportButton.update(now);

  if (chA.getChanged()) {
    digitalWrite(ledA, chA.getState());
  }

  if (chB.getChanged()) {
    digitalWrite(ledB, chB.getState());
  }

  if (idx.getChanged()) {
    digitalWrite(ledI, idx.getState());
    if (idx.getState() && (abs(idx.getCount()) > 2)) {
      countsPerRev = enc.getCount() - encCountsLast;
      encCountsLast = enc.getCount();
    }
  }

  checkUserInput();
}


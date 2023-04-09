// This Arduino example demonstrates bidirectional operation of a
// 28BYJ-48, using a VMA401 - ULN2003 interface board to drive the stepper.
// The 28BYJ-48 motor is a 4-phase, 8-beat motor, geared down by
// a factor of 68. One bipolar winding is on motor pins 1 & 3 and
// the other on motor pins 2 & 4. The step angle is 5.625/64 and the
// operating Frequency is 100pps. Current draw is 92mA.
////////////////////////////////////////////////

//declare variables for the motor pins
uint8_t motorPin1 = 5;                                                               // Blue - 28BYJ48 pin 1
uint8_t motorPin2 = 6;                                                               // Pink - 28BYJ48 pin 2
uint8_t motorPin3 = 7;                                                              // Yellow - 28BYJ48 pin 3
uint8_t motorPin4 = 8;                                                              // Orange - 28BYJ48 pin 4
                                                                                     // Red - 28BYJ48 pin 5 (VCC)
int motorSpeed = 3600;                                                               // 800 - 4800 defaultvariable to set stepper speed, ***higher is slower***
int lookup[8] = { B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001 };  // used for stepping the motor


// keep the states of A and B inputs
int curStateA = 1;
int prevStateA = 1;
int curStateB = 1;

int stepSize = 1;
int countsPerHalfRev = 265;  // was 512 number of steps for full revolution
int ledPin = 13;


//////////////////////////////////////////////////////////////////////////////
// Digital encoder
int8_t pinA = 2;
int8_t pinB = 3;
int8_t pinSW = 4;
int pos = 0;

// initialize everything
void setup() {
  // setup digital encoder
  pinMode(pinSW, INPUT_PULLUP);  // saves need for external pullup resistors
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);

  //declare the motor pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  Serial.begin(9600);

  initMotor(); //do this to run motor from end to end once to calibrate without using end stops as it won't hurt it.
  prevStateA = digitalRead(pinA);
  Serial.println("Ready!");
}

// main prog loop
void loop() {
  curStateA = digitalRead(pinA);
  curStateB = digitalRead(pinB);

  if (curStateA != prevStateA) {
    if (curStateB != curStateA) {
      pos--;
      clockwise();
    } else {
      pos++;
      anticlockwise();
    }

    if (pos < 0) { pos = 0;  }
    if (pos > countsPerHalfRev) { pos = countsPerHalfRev; }
    Serial.println(pos);
  
  }
  prevStateA = curStateA;
}


// run the motor from end to end and back. It is a stepper so it will not harm it and no limit switches are needed
// if it does not do 180 degrees then the value of countsPerHalfRev is wrong, refer to documentation for your specific motor
// you can reverse antoclocwise and clockwise statements to ensure the cap is closed
void initMotor() {

  Serial.println("Init ACW");
  for (int i = 0; i <= countsPerHalfRev; i++) {
    anticlockwise();
  }

  Serial.println("Init CW");
  for (int i = countsPerHalfRev; i > -5; i--) {
    clockwise();
  }
  pos = 0;
}

// close the cap to prevent dust getting on vanes
void shutdown() {
  for (int i = 0; i < countsPerHalfRev; i++) {
    anticlockwise();
  }
}

// move the motor
//set pins to ULN2003 high in sequence from 1 to 4
//delay "motorSpeed" between each pin setting (to determine speed)
void anticlockwise() {
  for (int i = 0; i < 8; i++) {
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

void clockwise() {
  for (int i = 7; i >= 0; i--) {
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

// drive the outputs sequentially as desired
void setOutput(int out) {
  digitalWrite(motorPin1, bitRead(lookup[out], 0));
  digitalWrite(motorPin2, bitRead(lookup[out], 1));
  digitalWrite(motorPin3, bitRead(lookup[out], 2));
  digitalWrite(motorPin4, bitRead(lookup[out], 3));
}

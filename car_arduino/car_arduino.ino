// Self Driving Car - Arduino Code
// Theresa Cruz, Jessica Dudoff, James Lee, and Ben Reese
// ECPE 196 - Fall 2014

// Included Libraries
#include <Servo.h>
#define thresholdVal 100


// constants
const unsigned breakThresh = 1510;
const unsigned breakVal = 1000;
const unsigned neutralVal = 1480;
const unsigned bumpThresh = 1580;
const unsigned bumpVal = 1650;
const int IRThresh = 80;

// Pin Assignments
const int led = 13;
const int leftIRSensor = 3;
const int compStandard = A0;
const int speedSensor = A1;
const int turnServoPin = 9;
const int camServoPin = 10;
const int speedServoPin = 9;


// Variables
Servo turnServo;
Servo speedServo;
Servo camServo;
String command = "";
boolean commandComplete = false;
boolean turn = true;
char comm = ' ';
int inX = 0;
int currentMicro = 1500;
int currentAngle = 90;
int rightValue = 0;
int leftValue = 0;
int compVal = 0;
int speedVal = 0;
unsigned long lastHigh = 0;
unsigned long lastLow = 0;
float currentSpeed = 0;



void setup() {
  Serial.begin(9600);
  command.reserve(200); 
  turnServo.attach(turnServoPin);
  camServo.attach(camServoPin);
  speedServo.attach(speedServoPin,1000,2000);
  
  pinMode(led,OUTPUT);
  analogRead(leftIRSensor);
  analogRead(compStandard);
  analogRead(speedVal);
  speedServo.writeMicroseconds(neutralVal);
  currentMicro = neutralVal;

  
}

void stopCar() {
   Serial.print(currentMicro);
  if (currentMicro>=breakThresh)
      {
        speedServo.writeMicroseconds(breakVal);
        currentMicro = breakVal;
      }
      else
      {
        speedServo.writeMicroseconds(neutralVal);
        currentMicro = neutralVal;
      }  
}

void FcarSpeed(int newSpeed){
  if (newSpeed<neutralVal){
    Serial.print("E\n");
  }
  else if (currentMicro <bumpThresh && newSpeed<=bumpThresh){
    speedServo.writeMicroseconds(bumpVal);
    delay(50);
    speedServo.writeMicroseconds(newSpeed);
    currentMicro = newSpeed;
  }
  else {
    speedServo.writeMicroseconds(newSpeed);
    currentMicro = newSpeed;
  }
  
}

void RcarSpeed(int newSpeed){
  if (newSpeed>neutralVal){
    Serial.print("E\n");
  }
  else if (currentMicro > neutralVal){
    speedServo.writeMicroseconds(breakVal);
    delay(1000);
    speedServo.writeMicroseconds(neutralVal);
    delay(100);
    speedServo.writeMicroseconds(newSpeed);
    currentMicro = newSpeed;
  }
  else if (currentMicro <= neutralVal){
    speedServo.writeMicroseconds(newSpeed);
    currentMicro = newSpeed;
  }
  else {
    Serial.print("E\n");
  }
}




void loop() {
  // For now, assuming that I have taken in the UART message and 
  // decoded it into a char for the command, and an int or double for
  // the parameter

  // Assumptions - name of the char will be comm
  // name of the parameter will be inX
  leftValue = analogRead(leftIRSensor);
  if (leftValue>thresholdVal)
  {
    Serial.println("O");
    stopCar();
    delay(500);
    while(leftValue>IRThresh){
      leftValue = analogRead(leftIRSensor);
    }
    while(Serial.read() != -1);
    Serial.print("G");
  }
  else if (commandComplete){
    comm = command[0];
    command = command.substring(1);
    inX = command.toInt();
    switch (comm) {
    case 'S':  // Stop Car         
      stopCar();
      break;
    case 'F':  // Car Forward Command
      FcarSpeed(inX);
      break;
    case 'B':  // Car Backward Command
      RcarSpeed(inX);
      break;
    case 'R':
      // Set direction servo and camera servo to 
      //parameter in the right direction
      turnServo.write(90-inX);
      camServo.write(90-inX);
      break;

    case 'L':
      // Set direction servo and camera servo to 
      //parameter in the left direction
      turnServo.write(90+inX);
      camServo.write(90+inX);
      break;
    case 'A':
      break;
      // Acknowledge
    case 'E':
      // Error - therefore stop immediately!!
      stopCar();
      break;
    }
    command = "";
    commandComplete = false;
  }
}


// This command is simply to find anything in the Serial buffer and read it as a command
void serialEvent() {
  while (Serial.available()) {
    char inChar = char(Serial.read());
    command += inChar;
    if (inChar== '\n') commandComplete=true;
  }
}



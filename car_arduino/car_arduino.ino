// Self Driving Car - Arduino Code
// Theresa Cruz, Jessica Dudoff, James Lee, and Ben Reese
// ECPE 196 - Fall 2014

// Included Libraries
#include <Servo.h>
#define thresholdVal 100

// Pins and Variables

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

// Pin Assignments
int led = 13;
int rightIRSensor = 2;
int leftIRSensor = 3;
int compStandard = A0;
int speedSensor = A1;




void setup() {
  Serial.begin(9600);
  command.reserve(200); 
  turnServo.attach(9);
  camServo.attach(10);
  speedServo.attach(11,1000,2000);
  
  pinMode(led,OUTPUT);
  analogRead(rightIRSensor);
  analogRead(leftIRSensor);
  analogRead(compStandard);
  analogRead(speedVal);
  speedServo.writeMicroseconds(1480);
  currentMicro = 1480;

  
}

void stopCar() {
   Serial.print(currentMicro);
  if (currentMicro>=1510)
      {
        speedServo.writeMicroseconds(1000);
        currentMicro = 1000;
      }
      else
      {
        speedServo.writeMicroseconds(1480);
        currentMicro = 1480;
      }  
}

void FcarSpeed(int newSpeed){
  if (newSpeed<1480){
    Serial.print("E\n");
  }
  else if (currentMicro <1580 && newSpeed<=1580){
    speedServo.writeMicroseconds(1580);
    delay(50);
    speedServo.writeMicroseconds(newSpeed);
    currentMicro = newSpeed;
  }
  else if (currentMicro>=1580 && newSpeed>1580){
    speedServo.writeMicroseconds(newSpeed);
    currentMicro = newSpeed;
  }
  else
  {
    speedServo.writeMicroseconds(newSpeed);
    currentMicro = newSpeed;
  }
  
}

void RcarSpeed(int newSpeed){
  if (newSpeed>1480){
    Serial.print("E\n");
  }
  else if (currentMicro > 1480){
    speedServo.writeMicroseconds(1200);
    delay(1000);
    speedServo.writeMicroseconds(1480);
    delay(100);
    speedServo.writeMicroseconds(newSpeed);
    currentMicro = newSpeed;
  }
  else if (currentMicro <= 1480){
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
//  rightValue = analogRead(rightIRSensor);
  leftValue = analogRead(leftIRSensor);
//  if (rightValue>thresholdVal || leftValue>thresholdVal)
  if (leftValue>thresholdVal)
  {
    Serial.println("O");
    stopCar();
    delay(500);
    while(leftValue>80){
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
/*    if (ACSR & (1<<ACO))    // check status of comparator output flag; if set, then check the speed
    {
      if (led==HIGH)
      {
        currentSpeed = calcSpeed(lastHigh,lastLow);
      }
      else
      {
        currentSpeed = calcSpeed(lastLow,lastHigh);
      }
    }
  }
*/
}
}

// Command to determine speed based on 1 revolution between beginTime and endTime
float calcSpeed(long beginTime, long endTime)
{
  long passedTime = endTime-beginTime;
  passedTime = passedTime/1000;
  return 1/(passedTime);
  
}


// This command is simply to find anything in the Serial buffer and read it as a command
void serialEvent() {
  while (Serial.available()) {
    char inChar = char(Serial.read());
    command += inChar;
    if (inChar== '\n'){
      commandComplete=true;
    }
  }
}

// Interrupt - Store the last time there was a high or low. 
// Helps to determine the speed of the car
/*ISR(ANALOG_COMP_vect) {
  if (digitalRead(led)==HIGH){
    digitalWrite(led,LOW);
    lastHigh = millis();
  }
  else
  {
    digitalWrite(led,HIGH);
    lastLow = millis();
  }
}*/


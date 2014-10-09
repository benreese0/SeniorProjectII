// Self Driving Car - Arduino Code
// Theresa Cruz, Jessica Dudoff, James Lee, and Ben Reese
// ECPE 196 - Fall 2014

// Included Libraries
#include <Servo.h>
#define thresholdVal 60

// Pins and Variables

Servo turnServo;
Servo speedServo;
String command = "";
boolean commandComplete = false;
boolean turn = true;
char comm = ' ';
int inX = 0;
int currentMicro = 1500;
int currentAngle = 90;
int led = 13;
int rightIRSensor = 2;
int rightValue = 0;

void setup() {
  Serial.begin(9600);
  command.reserve(200); 
  turnServo.attach(9);
  speedServo.attach(11,1000,2000);
  pinMode(led,OUTPUT);
  analogRead(rightIRSensor);
  speedServo.writeMicroseconds(1475);
  currentMicro = 1475;
}

void stopCar() {
  Serial.println(currentMicro);
  if (currentMicro>1500)
      {
        speedServo.writeMicroseconds(1000);
        currentMicro = 1000;
      }
      else
      {
        speedServo.writeMicroseconds(1475);
        currentMicro = 1475;
      }

  
}



void loop() {
  // For now, assuming that I have taken in the UART message and 
  // decoded it into a char for the command, and an int or double for
  // the parameter

  // Assumptions - name of the char will be comm
  // name of the parameter will be inX
  rightValue = analogRead(rightIRSensor);
  if (rightValue>thresholdVal)
  {
    stopCar();
    while(rightValue>40){
    rightValue = analogRead(rightIRSensor);
    Serial.println(rightValue);

  }
  }
  else if (commandComplete){
    Serial.println(command);
    comm = command[0];
    command = command.substring(1);
    inX = command.toInt();
    Serial.println(comm);
    Serial.println(inX);
    switch (comm) {
    case 'S':
      //
      stopCar();
      

      break;
    case 'F':
      /*
              if (inX==1)
       {
       Serial.println("Changing speed to 1");
       speedServo.writeMicroseconds(1600);
       // Set speed to parameter
       }
       else if (inX==2)
       {
       speedServo.writeMicroseconds(1700);
       }
       */
      speedServo.writeMicroseconds(inX);
      currentMicro = inX;
      break;
    case 'R':
      // Set direction servo and camera servo to 
      //parameter in the right direction
      turnServo.write(90-inX);
      break;

    case 'L':
      // Set direction servo and camera servo to 
      //parameter in the left direction
      turnServo.write(90+inX);
      break;

    case 'B':
      // Stop Car, then set reverse speed to given parameter
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
  if (digitalRead(led)==HIGH)
  {
    digitalWrite(led,LOW);
  }
  else
  {
    digitalWrite(led,HIGH);
  }

}

void serialEvent() {
  while (Serial.available()) {
    char inChar = char(Serial.read());
    command += inChar;
    if (inChar== '\n'){
      commandComplete=true;
    }
  }
}




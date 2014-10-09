// Self Driving Car - Arduino Code
// Theresa Cruz, Jessica Dudoff, James Lee, and Ben Reese
// ECPE 196 - Fall 2014

// Included Libraries
#include <Servo.h>


// Pins and Variables

Servo turnServo;
Servo speedServo;
String command = "";
boolean commandComplete = false;
char comm = ' ';
int inX = 0;
int currentMicro = 1500;
int currentAngle = 90;
int led = 13;

void setup() {
  Serial.begin(9600);
  command.reserve(200); 
  turnServo.attach(9);
  speedServo.attach(11);
  pinMode(led,OUTPUT);
  speedServo.writeMicroseconds(1000);
  delay(1000);
  speedServo.writeMicroseconds(3000);
  delay(1000);
  speedServo.writeMicroseconds(1500);
  delay(1000);
}

void setSpeed(int speed)
{
  // speed is from 0 to 100 where 0 is off and 100 is max speed
  // the following maps speed values of 0-100 to angles from 0-180
  
  int angle = map(speed, 0, 100, 0, 180);
  speedServo.write(angle);
  
}

void arm() {
  setSpeed(30);
  delay(2000);
  setSpeed(90);
  delay(2000);
  setSpeed(30);
  delay(2000);
}

  

void loop() {
// For now, assuming that I have taken in the UART message and 
// decoded it into a char for the command, and an int or double for
// the parameter

// Assumptions - name of the char will be comm
// name of the parameter will be inX
     if (commandComplete){
         Serial.println(command);
         comm = command[0];
         command = command.substring(1);
         inX = command.toInt();
         Serial.println(comm);
         Serial.println(inX);
          switch (comm) {
          case 'S':
              //
              if (currentMicro<1500)
              {
                speedServo.writeMicroseconds(2000);
                currentMicro = 2000;
              }
              else
              {
                speedServo.writeMicroseconds(1500);
                currentMicro = 1500;
              }
              
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
              if (currentMicro<1500)
              {
                speedServo.writeMicroseconds(2000);
                currentMicro = 2000;
              }
              else
              {
                speedServo.writeMicroseconds(1500);
                currentMicro = 1500;
              }
              
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



#include <Servo.h>

Servo servoMotor;

void setup() {
  Serial.begin(9600);
  servoMotor.attach(2);
}

void loop() {
  // put your main code here, to run repeatedly:
  servoMotor.write(45);
  delay(1000);
  servoMotor.write(135);
  delay(1000);
}

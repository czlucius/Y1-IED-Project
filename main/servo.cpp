#include "servo.h"
#include <Arduino.h>
void servoInit(Servo &servo, int pin, int angle) {
  servo.attach(7);
//  servo.write(angle);
}


void moveToAngle(Servo &servo, int angle) {
  servo.write(angle);
}

void moveGraduallyToAngle(Servo &servo, int finalAngle) {
  for (int angle = 10; angle < finalAngle; angle++)
  {
    servo.write(angle);
    delay(15);
  }
}

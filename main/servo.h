#include <Servo.h>
void servoInit(Servo &servo, int pin, int angle) ;
void moveToAngle(Servo &servo, int angle);
void moveGraduallyToAngle(Servo &servo, int finalAngle);

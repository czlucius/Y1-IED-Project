
#include <LiquidCrystal_I2C.h>

#include "motors.h"
#include <Arduino.h>

#define BLACK 1
#define WHITE 0



Motors::Motors(int in1 = 9, int in2 = 6, int in3 = 5, int in4 = 3) {
  pinMode(in2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(in3, OUTPUT);
  this->in1 = in1;
  this->in2 = in2;
  this->in3 = in3;
  this->in4 = in4;
}



void Motors::forward(int speed) {
  digitalWrite(in1, LOW);
  analogWrite(in2, speed);
  digitalWrite(in3, LOW);
  analogWrite(in4, speed);
}

void Motors::control(int left, int right) {
  digitalWrite(in1, LOW);
  analogWrite(in2, left);
  digitalWrite(in3, LOW);
  analogWrite(in4, right);
}





void Motors::brake() {
  Serial.println("Brake");
  // Standby mode
  digitalWrite(in1, LOW);
  analogWrite(in2, 0);
  digitalWrite(in3, LOW);
  analogWrite(in4, 0);
}



void displayTet(String text) {
  //  lcUd.clear();
  //  Serial.println("LCD is displaying: " + text);
  //  lcUd.setCursor(0, 0); // position cursor at character 0, line 0
  //  if (text.length() > 16) {
  //    lcUd.print(text.substring(0, 16));
  //    lcUd.setCursor(0, 1);
  //    lcUd.print(text.substring(16));
  //
  //  } else {
  //    lcUd.print(text);
  //  }
}


String lineTrackNow(Motors motors, int ir1 = 14, int ir2 = 15) {
  int leftIR = digitalRead(ir1); // Reading and storing IR sensor 1 digital signal value
  int rightIR = digitalRead(ir2); // Reading and storing IR sensor 2 digital signal value

  Serial.print("IR Value");
  Serial.println(String(leftIR) + " " + String(rightIR));

  // IR2 is right, IR1 is left.
  // White is 0, Black is 1
  if (leftIR == BLACK && rightIR == BLACK) {
    // Break point. We exit the program.
    Serial.println("All black");
    return "BB";
  } else if (leftIR == WHITE && rightIR == BLACK) {
    Serial.println("WB");
    // It has swerved to the left. Move it right.
    motors.control(150, 0);//100);
    return "WB";


  } else if (leftIR == BLACK && rightIR == WHITE) {
    Serial.println("BW");
    // It has swerved to the right. Move it left.
    motors.control(0, 150);
    return "BW";
  } else {
    Serial.println("WW");
    displayTet("WW");


    // The robot is out of the line. We shall just make it go forward.
    motors.control(100, 100); // move straight
    return "WW";
  }
}

#include "libs/esp8266.h"
#include "wifi2.h"
#include "secrets.h"
#include "motors.h"
#include "sensors.h"
#include "servo.h"
#include "DHT.h"
#define DEBUG true
#define ESP_DEBUG true


#define HIT_A 4
#define HIT_B 2
#define RAIN_SENSOR A2
#define DHT_PIN 11
#define MOISTURE_PIN A3

#include <Wire.h>
#include <SoftwareSerial.h> // Software Serial library
#include <LiquidCrystal_I2C.h>
#include <Servo.h>



SoftwareSerial esSerial(12, 13);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Motors motors;
Servo servo;
SensorAnalog rainSense(RAIN_SENSOR, INPUT_PULLUP);
SensorAnalog moistureSense(MOISTURE_PIN, INPUT_PULLUP);
DHT *dhtPoint = new DHT();
DHTTemp *tempSense = nullptr;
DHTHumid *humidSense = nullptr;


volatile bool toRun = false;
volatile unsigned long timeSinceLastHitA = -700;

volatile bool waterAllPlants = false; // If this is false, we water dry plants only. Else we water all plants.
volatile unsigned long timeSinceLastHitB = -700;

int timeSinceDataSent = -700;


void displayText(String text) {
  lcd.clear();
  Serial.println("LCD is displaying: " + text);
  lcd.setCursor(0, 0); // position cursor at character 0, line 0
  if (text.length() > 16) {
    lcd.print(text.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(text.substring(16));

  } else {
    lcd.print(text);
  }
}

void lcdInit() {
  lcd.init(); // initialise the LCD
  lcd.setCursor(0, 0); //position cursor at top row left hand corner
  lcd.begin(16, 2); // 16 characters, 2 lines
  lcd.backlight(); // turn backlight on
  lcd.setCursor(0, 0); // position cursor at character 0, line 0
  lcd.print("Hello! ");
}


void hitAInterrupt() {
  if ((millis() - timeSinceLastHitA) >= 1500) {
    int targetA = digitalRead(HIT_A);
    if (targetA == LOW) {
      // Target A is hit. We stop execution.
      toRun = !toRun;
      if (toRun) {
        displayText("Running ...");
      } else {
        displayText("Program stopped");
      }
      timeSinceLastHitA = millis();
    }

  }

}

void hitBInterrupt() {
  if ((millis() - timeSinceLastHitB) >= 1500) {
    int targetB = digitalRead(HIT_B);
    if (targetB == LOW) {
      // Target B is hit. Water all plants
      waterAllPlants = !waterAllPlants;
      if (waterAllPlants) {
        displayText("Watering all");
      } else {
        displayText("Watering dry");
      }
      timeSinceLastHitB = millis();

    }
  }
}

void jumpStartToSpeed(Motors motors, int left, int right) {
  motors.control(140, 140);
  delay(200);
  motors.control(left, right);
}


bool measureMoisture() {
  moveGraduallyToAngle(servo, 180);
  int moisture = moistureSense.read();
  delay(100);
  moveGraduallyToAngle(servo, 20);
  return moisture < 670;
}






void setup()
{
  Serial.begin(9600);

  lcdInit();
  dhtPoint->setup(DHT_PIN);

  tempSense = new DHTTemp(dhtPoint);
  humidSense = new DHTHumid(dhtPoint);


  //  attachInterrupt(digitalPinToInterrupt(HIT_A), hitAInterrupt, CHANGE);
  //  attachInterrupt(digitalPinToInterrupt(HIT_B), hitBInterrupt, CHANGE);
  Serial.println("Connecting to Wi-Fi, please wait for a moment...");
  displayText("Connecting...");
  wifiSetup();
  displayText("Connected");

  delay(2000);


  servoInit(servo, 7, 10);
  servo.attach(7);
  servo.write(10);
  delay(2000);
  Serial.println("Finished setting up");
}


// Setup:
/*
   Get the sensors to work:
   - DHT11
   - Rain sensor (A2 Input)
   - Moisture sensor (A3 input)
   - SG90 servo * 2
   - LCD
   - Target sensor * 2
*/



void loop()
{
  //  Serial.println(digitalRead(HIT_A));
  //  Serial.println("Hi!" + String(toRun) + " " + String(millis()) +" " + timeSinceLastHitA);
  hitAInterrupt();
  //  hitBInterrupt();
  //
  //  if ((millis() - timeSinceDataSent) >= 10000) {
  //
  //    //    sendThingSpeakSensors(rainSense, "field1");
  //    sendThingSpeakSensors(*tempSense, "field2");
  //    sendThingSpeakSensors(*humidSense, "field3");
  //    timeSinceDataSent = millis();
  //
  //  }

  // Target sensor board
  if (toRun) {

    String breakPoint = lineTrackNow(motors, A0, A1);
    if (breakPoint == "BB") {
      motors.brake();
      displayText("Break Point");
      bool moist = measureMoisture();
      if (moist) {
        displayText("Moist");
      } else {
        displayText("Not moist");
      }

      sendThingSpeakSensors(rainSense, "field1");
      sendThingSpeakSensors(*tempSense, "field2");
      sendThingSpeakSensors(*humidSense, "field3");
      timeSinceDataSent = millis();




      delay(3000);
      displayText("Running ...");

    }
  } else {
    motors.brake();
  }





}

// As we are using SoftwareSerial for Wi-Fi, we cannot use the interrupt ISR (PCINT2_vect) and have to check in every loop.

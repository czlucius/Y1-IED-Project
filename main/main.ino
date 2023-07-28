// These over here are not third party libraries.
// They are header files (and C++ files) that I created so that my code will have better Seperation of Concerns
// https://en.wikipedia.org/wiki/Separation_of_concerns
#include "wifi2.h"
#include "secrets.h"
#include "motors.h"
#include "sensors.h"
#include "servo.h"


#define DEBUG true
#define ESP_DEBUG true


#define HIT_A 4
#define HIT_B 2
#define RAIN_SENSOR A2
#define DHT_PIN 8
#define MOISTURE_PIN A3

#define MOIST_SERVO 7
#define WATER_SERVO 10

#include "esp8266.h"
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);
Motors motors;
Servo servo;
Servo waterServo;
SensorAnalog rainSense(RAIN_SENSOR, INPUT_PULLUP);
SensorAnalog moistureSense(MOISTURE_PIN, INPUT_PULLUP);
DHT *dhtPoint = new DHT();
DHTTemp *tempSense = nullptr;
DHTHumid *humidSense = nullptr;


volatile bool toRun = false;
volatile unsigned long timeSinceLastHitA = -700;

volatile bool seedMode = false;  // If this is false, we water dry plants only. Else we water all plants.
volatile unsigned long timeSinceLastHitB = -700;

int timeSinceDataSent = -700;

volatile int bpCount = 0;  // we stop aft 3
int maxBp = 3;



void displayText(String text) {
  lcd.clear();
  Serial.println("LCD is displaying: " + text);
  lcd.setCursor(0, 0);  // position cursor at character 0, line 0
  if (text.length() > 16) {
    lcd.print(text.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(text.substring(16));

  } else {
    lcd.print(text);
  }
}



void displayTextMultiline(String line1, String line2) {
  lcd.clear();
  Serial.println("LCD is displaying: " + line1 + "\n" + line2);
  lcd.setCursor(0, 0);  // position cursor at character 0, line 0
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void lcdInit() {
  lcd.init();           // initialise the LCD
  lcd.setCursor(0, 0);  //position cursor at top row left hand corner
  lcd.begin(16, 2);     // 16 characters, 2 lines
  lcd.backlight();      // turn backlight on
  lcd.setCursor(0, 0);  // position cursor at character 0, line 0
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
        motors.forward(140);
        delay(400);
      } else {
        displayText("Program stopped");
      }
      bpCount = 0;

      timeSinceLastHitA = millis();
    }
  }
}

void hitBInterrupt() {
  //  Serial.println("246245");
  if ((millis() - timeSinceLastHitB) >= 1500) {
    int targetB = digitalRead(HIT_B);
    //    Serial.println(targetB);

    if (targetB == LOW) {
      // Target B is hit. Enable seed mode.
      seedMode = !seedMode;
      if (seedMode) {
        displayText("Seed mode");
      } else {
        displayText("Watering mode");
      }
      timeSinceLastHitB = millis();
    }
  }
}




bool measureMoisture() {
  moveGraduallyToAngle(servo, 180);
  //  servo.write(180);
  int moisture = moistureSense.read();
  Serial.println("Moisture " + String(moisture));
  delay(100);
  servo.write(10);
  return moisture < 670;
}

void dispense() {
  waterServo.write(150);
  delay(2000);
  waterServo.write(0);
  delay(500);
  servo.write(10);
}






void setup() {
  Serial.begin(9600);

  pinMode(HIT_A, INPUT_PULLUP);
  pinMode(HIT_B, INPUT_PULLUP);

  lcdInit();
  dhtPoint->setup(DHT_PIN);

  tempSense = new DHTTemp(dhtPoint);
  humidSense = new DHTHumid(dhtPoint);
  Serial.println("Temp " + String(tempSense->read()) + " Humid " + String(humidSense->read()));


  //  attachInterrupt(digitalPinToInterrupt(HIT_A), hitAInterrupt, CHANGE);
  // Only B can listen out for interrupt routines as its an external interrupt with no S/W Serial interference. TODO check if this affects Servo.
  //  attachInterrupt(digitalPinToInterrupt(2), hitBInterrupt, CHANGE);
  Serial.println("Connecting to Wi-Fi, please wait for a moment...");
  displayText("Connecting...");
  wifiSetup();
  displayText("Connected");



  // String getWithRes(char *host, int port, char *path)
  String sres = httpGetWRes("168.138.177.217", 28508, "/data/seedMode");
  Serial.println("sres is " + sres);
  if (sres != "") {
    seedMode = (sres == "1");
    if (seedMode) {
      displayText("Seed mode");
    } else {
      displayText("Watering mode");
    }
    delay(1000);
  }
  String bpres = httpGetWRes("168.138.177.217", 28508, "/data/maxBreaks");
  if (bpres != "" || bpres == '0') {
    maxBp = bpres.toInt();
    displayText(String(maxBp) + " break points");
    delay(1000);
  }

  delay(2000);
  digitalWrite(11, 0);  // activate servo
  servo.attach(MOIST_SERVO);
  servo.write(10);
  waterServo.attach(WATER_SERVO);
  waterServo.write(0);
  //  servoInit(servo, MOIST_SERVO, 10);
  //  servoInit(waterServo, WATER_SERVO, 0);
  delay(2000);






  Serial.println("Finished setting up");
  displayTextMultiline("Hello!", "Hit A to start");
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



void loop() {
  hitAInterrupt();
  hitBInterrupt();

  // Target sensor board
  if (toRun && bpCount < maxBp) {

    String breakPoint = lineTrackNow(motors, A0, A1);
    char *rain = nullptr;
    char *moisture = nullptr;
    char *humid = nullptr;
    if (breakPoint == "BB") {
      motors.brake();
      displayText("Break Point");

      bpCount++;  // Increase count of break points, we stop @ 3.

      // Check for seeding mode
      if (!seedMode) {



        bool moist = measureMoisture();
        digitalWrite(11, 1);  // disable servo

        if (moist) {
          displayText("Moist");
          rain = "1";
        } else {
          displayText("Not moist");
          rain = "0";
        }
        delay(1400);
        float rainVal = rainSense.read();
        if (rainVal < 670.0) {
          displayText("There is rain");
          moisture = "1";
        } else {
          displayText("No rain");
          moisture = "0";
        }
        delay(1400);

        float tempVal = tempSense->read();
        String tempDisp = String(tempVal) + " deg C";
        displayTextMultiline("Temperature", tempDisp);
        //      sendThingSpeakSensors(*tempSense, "field2");
        char temp[String(tempVal).length() + 1];
        dtostrf(tempVal, 2, 0, temp);

        delay(1400);

        float humidVal = humidSense->read();
        String humidDisp = String(humidVal) + "%";
        displayTextMultiline("Humidity", humidDisp);
        char humid[String(humidVal).length() + 1];
        dtostrf(humidVal, 2, 0, humid);

        // Send all data at once
        sendTS4(rain, temp, humid, moisture);


        timeSinceDataSent = millis();

        if (!moist && rainVal >= 670.0) {
          dispense();
        }

        delay(3000);
      } else {
        dispense();
      }

      if (bpCount < maxBp) {
        displayText("Running ...");
      } else {

        displayText("Finished.");
      }
      motors.forward(130);
      delay(400);
      motors.brake();
    }
  } else {
    motors.brake();
  }
}

// As we are using SoftwareSerial for Wi-Fi, we cannot use the interrupt ISR (PCINT2_vect) and have to check the value for Hit A and Hit B (and IR sensor) in every loop.
// This is a known problem of the Arduino platform and cannot be worked around.
// https://forum.arduino.cc/t/softwareserial-read-text-0x0-multiple-definition-of-__vector_3/520573

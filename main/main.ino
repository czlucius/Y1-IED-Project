
#include "wifi.h"
#include "secrets.h"
#define DEBUG true


void setup()
{
  Serial.begin(9600);
  wifiSetup();

}

void loop()
{
  /* Here, I'm using the function random(range) to send a random value to the
    ThingSpeak API. You can change this value to any sensor data
    so that the API will show the sensor data
  */

  sendThingSpeakTCP("field1", "34");
  delay(1000);

  //
  //  sendVal = random(100); // Send a random number between 1 and 1000
  //  String sendData = "GET /update?api_key=" + myAPI + "&" + myFIELD + "=" + String(sendVal);
  //  espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections
  //  espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
  //  espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG);
  //  espSerial.find('>');
  //  espSerial.println(sendData);
  //  Serial.print("Value to be sent: ");
  //  Serial.println(sendVal);
  //
  //  espData("AT+CIPCLOSE=0", 1000, DEBUG);
  //  delay(1000);
}

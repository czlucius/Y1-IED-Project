#define ESP_DEBUG true
#include "secrets.h"
#include <SoftwareSerial.h>       //Software Serial library

#include <Arduino.h>
SoftwareSerial espSerial(12, 13);   //Pin 12 and 13 act as RX and TX. Connect them to TX and RX of ESP8266


String espData(String command, const int timeout, bool debug);

SoftwareSerial wifiSetup() {
  espSerial.begin(9600);

  espData("AT+RST", 1000, ESP_DEBUG);                      //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000, ESP_DEBUG);                 //Set the ESP mode as station mode
  espData("AT+CWJAP=\"" + String(WLAN_SSID) + "\",\"" + WLAN_PW + "\"", 1000, ESP_DEBUG); //Connect to WiFi network
  delay(1600);
  return espSerial;
}


void sendThingSpeakTCP(String field, String value, String host = "api.thingspeak.com", int port = 80, String queryAppend = "") {
  String sendData = "GET /update?api_key=" + String(API_KEY) + "&" + field + "=" + value + queryAppend;
  espData("AT+CIPMUX=1", 1000, ESP_DEBUG);       //Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\"" + host + "\"," + String(port), 1000, ESP_DEBUG);
  espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, ESP_DEBUG);
  espSerial.find('>');
  espSerial.println(sendData);
  Serial.print("Value to be sent: ");
  Serial.println(value);

  espData("AT+CIPCLOSE=0", 1000, ESP_DEBUG);
}






















void tcpGet(String field, String value, String host = "api.thingspeak.com", int port = 80, String queryAppend = "") {
  String sendData = "GET /";
  espData("AT+CIPMUX=1", 1000, ESP_DEBUG);       //Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\"" + host + "\"," + String(port), 1000, ESP_DEBUG);
  espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, ESP_DEBUG);
  espSerial.find('>');
  espSerial.println(sendData);
  Serial.print("Value to be sent: ");
  Serial.println(value);

  espData("AT+CIPCLOSE=0", 1000, ESP_DEBUG);
}


String espData(String command, const int timeout, bool debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");

  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}


String espDataNoLn(String command, const int timeout, bool debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");

  String response = "";
  espSerial.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}


void httpGet(String url) {
//  String req = "AT+HTTPCLIENT=1,0,\"" + url + "\",\"168.138.177.217:28508\",\"/data/power\",1";
//String req = "AT+HTTPCLIENT=2,0,\"http://168.138.177.217:28508/data/power\",\"168.138.177.217\",\"/data/power\",1";
  String req = "AT+HTTPCLIENT=2,0,\"http://httpbin.org/get\",\"httpbin.org\",\"/get\",1";
  espData(req, 20000, ESP_DEBUG);
}



//// Archived functions, which won't be available due to headers
//void httpGet(String host, String path, String defFieldName, String defFieldVal, String port = "80", String customAppend = "") {
//  String sendData = "GET " + path + "?" + defFieldName + "=" + defFieldVal + "&" + customAppend;
//  Serial.println(sendData);
//  espData("AT+CIPMUX=1", 1000, ESP_DEBUG);       //Allow multiple connections
//  espData("AT+CIPSTART=0,\"HTTP\",\"" + host + "\"," + port, 1000, ESP_DEBUG);
//  espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, ESP_DEBUG);
//  espSerial.find('>');
//  espSerial.println(sendData);
//
//  espData("AT+CIPCLOSE=0", 1000, ESP_DEBUG);
//}

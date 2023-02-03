#include "secrets.h"
#include "libs/esp8266.h"
#include "sensors.h" // as it declares pragma once, we need to include it again

#include <Arduino.h>
const char *SSID = WLAN_SSID;
const char *PW = WLAN_PW;

void wifiSetup(int countUp = 0) {
  char *ipAddress, ap[31];

  WiFi.reset(WIFI_RESET_HARD);
  WiFi.begin(9600);
  auto state = WiFi.join(SSID, PW);
  Serial.println("State of connection is " + String(state));
  if (state == WIFI_ERR_OK || state == WIFI_ERR_CONNECT) {
    ipAddress = WiFi.ip(WIFI_MODE_STA);
    Serial.print(F("\n\rIP address:"));
    Serial.print(ipAddress);
    Serial.print(':');
    if (WiFi.isConnect(ap))
      Serial.println(ap);
    else
      Serial.println(F("not found"));
  } else
    while (1);
}

void httpGet(char *host, int port, char *path) {
  if (WiFi.connect((char *)host, port) == WIFI_ERR_CONNECT) {
    String sample = "GET  HTTP/1.1\r\n\r\n";
    

    char result[sample.length() + strlen(path) + 1] = {};
    strcat(result, "GET ");
    strcat(result, path);
    strcat(result, " HTTP/1.1\r\n\r\n");
    Serial.println("Result is " + String(result));
    

    if (WiFi.send((const uint8_t *)result) == WIFI_ERR_OK) {
      int16_t c;
      uint16_t len = WiFi.listen(10000UL);
      while (len)
        if ((c = WiFi.read()) > 0) {
          Serial.write((char)c);
          len--;
        }
    } else
      Serial.println(F("Send fail"));

    WiFi.close();

  } else
    Serial.println(F("TCP connect fail"));



}


void plainGet(char *host, int port, char *path) {
  if (WiFi.connect((char *)host, port) == WIFI_ERR_CONNECT) {
    String sample = "GET \r\n";
    

    char result[sample.length() + strlen(path) + 1] = {};
    strcat(result, "GET ");
    strcat(result, path);
    strcat(result, " \r\n");
    Serial.println("Result is " + String(result));
    

    if (WiFi.send((const uint8_t *)result) == WIFI_ERR_OK) {
      int16_t c;
      uint16_t len = WiFi.listen(10000UL);
      while (len)
        if ((c = WiFi.read()) > 0) {
          Serial.write((char)c);
          len--;
        }
    } else
      Serial.println(F("Send fail"));

    WiFi.close();

  } else
    Serial.println(F("TCP connect fail"));



}


void sendThingSpeakTCP(char *field, char *value, char *queryAppend = "") {
  String sample = "/update?api_key=&=";
  char result[sample.length() + String(API_KEY).length() + strlen(field) + strlen(value) + strlen(queryAppend) + 1] = "/update?api_key=";
  strcat(result, API_KEY);
  strcat(result, "&");
  strcat(result, field);
  strcat(result, "=");
  strcat(result, value);
  strcat(result, queryAppend);
  plainGet((char *)"api.thingspeak.com", 80, result); 

//  const int port = 80;
//  String reqStr = "GET /update?api_key=" + String(API_KEY) + "&" + field + "=" + value + queryAppend;
//  char reqArr[reqStr.length()];
//  if (WiFi.connect((char *)"api.thingspeak.com", port) == WIFI_ERR_CONNECT) {
//    if (WiFi.send((const uint8_t *)reqArr) == WIFI_ERR_OK) {
//      int16_t c;
//      uint16_t len = WiFi.listen(10000UL);
//      while (len)
//        if ((c = WiFi.read()) > 0) {
//          Serial.write((char)c);
//          len--;
//        }
//    } else
//      Serial.println(F("Send fail"));
//
//    WiFi.close();
//  } else {
//    Serial.println(F("TCP connect fail"));
//  }
//

//  WiFi.disconnect();
}

void sendThingSpeakSensors(Sensor &sensor, char *field) {
  float value = sensor.read();
  char buf[String(value).length() + 1];
  dtostrf(value, 4,2,buf);
  sendThingSpeakTCP(field, buf);
}

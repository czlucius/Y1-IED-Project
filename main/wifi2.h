// Wi-Fi helper functions
#include "sensors.h"
#include "esp8266.h"


void wifiSetup(int = 0);
void httpGet(char *host, int port, char *path);
void sendThingSpeakTCP(char *field, char *value, char *queryAppend = "");
void sendThingSpeakSensors(Sensor &sensor, char *field);
void sendTS4(char *value1, char *value2, char *value3, char *value4);
String httpGetWRes(char *host, int port, char *path) ;

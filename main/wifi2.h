// Wi-Fi helper functions
#include "sensors.h"

void wifiSetup(int = 0);
void httpGet(char *host, int port, char *path);
void sendThingSpeakTCP(char *field, char *value, char *queryAppend = "");
void sendThingSpeakSensors(Sensor &sensor, char *field);

String espData(String command, const int timeout, bool debug);

void wifiSetup();
void sendThingSpeakTCP(String field, String value, String host = "api.thingspeak.com", int port = 80, String queryAppend = "");

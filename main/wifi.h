String espData(String command, const int timeout, bool debug);

String espDataNoLn(String command, const int timeout, bool debug);

void wifiSetup();
void sendThingSpeakTCP(String field, String value, String host = "api.thingspeak.com", int port = 80, String queryAppend = "");
void httpGet(String url);

void tcpGet(String field, String value, String host = "api.thingspeak.com", int port = 80, String queryAppend = "") ;

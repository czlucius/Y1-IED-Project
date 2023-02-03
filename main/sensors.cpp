#include "sensors.h"


SensorAnalog::SensorAnalog(int pin, int pm = INPUT_PULLUP): Sensor() {
  this->pin = pin;
  pinMode(pin, pm);
}

float SensorAnalog::read() {
  return (float) analogRead(pin);
}
bool SensorAnalog::readDigital() {
  return ((bool)digitalRead(pin));
}

DHTTemp::DHTTemp(DHT *dhtPoint): Sensor() {
  this->dhtPoint = dhtPoint;

}


float DHTTemp::read() {

  return dhtPoint->getTemperature();
}


bool DHTTemp::readDigital() {
  // We have to provide some functionality, or else we will be violating the Liskov Substitution Principle.

  return dhtPoint->getTemperature() > 25.0;
}





DHTHumid::DHTHumid(DHT *dhtPoint): Sensor() {
  this->dhtPoint = dhtPoint;
  this->pin = 9;

}


float DHTHumid::read() {

  return dhtPoint->getHumidity();
}
//float DHTHumid::rt() {
//
//  return dhtPoint->getTemperature();
//}

bool DHTHumid::readDigital() {

  // We have to provide some functionality, or else we will be violating the Liskov Substitution Principle.
  return dhtPoint->getHumidity() > 50.0;
}

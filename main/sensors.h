#pragma once

#include <Arduino.h>
#include "DHT.h"

class Sensor {
  protected:
    int pin;
  public:
    Sensor() {}; 

    virtual float read() {} ;
    virtual bool readDigital() {};

};




class SensorAnalog: public Sensor {
  public:
    SensorAnalog(int pin, int pm = INPUT_PULLUP);

    float read();
    bool readDigital();
};


class DHTTemp: public Sensor {
  private:
    DHT *dhtPoint;
  public:
    DHTTemp(DHT *dhtPoint);

    float read();
    bool readDigital();
};


class DHTHumid: public Sensor {
  private:
    DHT *dhtPoint;
  public:
    DHTHumid(DHT *dhtPoint);

    float read();
    bool readDigital();
};

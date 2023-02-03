//#include <Arduino.h>

class Motors {
  private:
    int in1, in2, in3, in4;

  public:
    Motors(int = 9, int = 6, int = 5, int = 3);
    void control(int, int);
    void forward(int);
    void brake();
};



String lineTrackNow(Motors motor, int=14, int=15);

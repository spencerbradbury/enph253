#if !defined(MOTOR_H)
#define MOTOR_H

#include <Arduino.h>

#define MOTOR_CLOCK_FREQ 100 // might have to change to 50

class Motor
{
private:
    PinName forwardPin;
    PinName backwardPin;
    int speed;
    int defaultSpeed;

public:
    Motor(PinName forwardPin, PinName backwardPin, int defaultSpeed);

    void modulateSpeed(int value);

    void setSpeed(int speed);

    void setDefaultSpeed(int speed);

    void stop();

    void activeStop();

    void start();
};
#endif // MOTOR_H

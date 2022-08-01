/*
Servos work by using a PWM like signal on the scale of 50Hz/20ms period.
Most servos only utilize the first 1ms to 2ms band of this period with a neutral postion of 1.5ms.
Some can extend beyond (the small servo can go from 800ms to 2200ms)
*/

#if !defined(BETTERSERVO_H)
#define BETTERSERVO_H

#include <Arduino.h>

class BetterServo
{
private:
    PinName servoPin;
public:
    BetterServo(PinName servoPin);

    void write(int angle);
}


#endif // BETTERSERVO_H



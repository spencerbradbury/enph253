#if !defined(ENCODER_H)
#define ENCODER_H

#include <Arduino.h>

class Encoder
{
private:
    uint8_t pin1;
    PinName pin2;
    int count;
    int speed;
    uint32_t lastTime;

    void handleInterrupt();

public:
    Encoder(uint8_t pin1, PinName pin2);

    int getCount();

    int getDistance();

    void resetCount();

    void setCount(int);
};
#endif // ENCODER_H

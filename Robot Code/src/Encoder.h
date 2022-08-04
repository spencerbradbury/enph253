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

    Encoder(uint8_t pin1);

    int getCount();

    int getDistance();

    void resetCount();

    void setCount(int);

    void setDirection();
};
#endif // ENCODER_H

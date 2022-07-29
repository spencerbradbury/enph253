#if !defined(ENCODER_H)
#define ENCODER_H

#include <Arduino.h>

class Encoder
{
private:
    uint8_t pin1;
    uint8_t pin2;
    int direction;
    int count;
    int speed;

public:
    Encoder(uint8_t pin1, uint8_t pin2);

    Encoder(uint8_t pin1);

    int getCount();

    int getDistance();

    void resetCount();

    void setCount();

    void setDirection();
};
#endif // ENCODER_H

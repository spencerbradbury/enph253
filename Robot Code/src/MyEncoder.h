#if !defined(MYENCODER_H)
#define MYENCODER_H

#include <Arduino.h>

class MyEncoder
{
private:
    uint8_t pin1;
    uint8_t pin2;
    int direction;
    int count;
    int speed;

public:
    MyEncoder(uint8_t pin1, uint8_t pin2);

    MyEncoder(uint8_t pin1);

    int getCount();

    int getDistance();

    void resetCount();

    void setCount();

    void setDirection();
};
#endif // MYENCODER_H
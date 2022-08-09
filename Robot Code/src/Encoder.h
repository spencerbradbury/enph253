#if !defined(ENCODER_H)
#define ENCODER_H

#include <Arduino.h>

#define COUNT_PER_ROTATION 1441        
#define WHEEL_DIAMTER 111              // mm or 11.1cm
#define DISTANCE_PER_COUNT PI * WHEEL_DIAMTER / COUNT_PER_ROTATION // mm
#define ANGLE_PER_COUNT (DISTANCE_PER_COUNT/114.3) * 180 / PI

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

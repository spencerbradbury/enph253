#if !defined(ENCODER_H)
#define ENCODER_H

#include <Arduino.h>

/*Spin Test Results
- 700 = 90 almost exactly
- 225 ~= 33
- 400 = 51
- 335 = 48

good enough to just use the same numbers based of left encoder to turn right    



- 220 Left = ~40 degrees
- 900 LEft = ~120 deg
- 1200 Left = ~150 deg

*/

#define COUNT_PER_ROTATION 1441.0      
#define WHEEL_DIAMTER 111.0             // mm or 11.1cm
#define DISTANCE_PER_COUNT PI * WHEEL_DIAMTER / COUNT_PER_ROTATION // mm 0.241996
#define ANGLE_PER_COUNT (DISTANCE_PER_COUNT/114.3) * 180.0 / PI      // 0.12131

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

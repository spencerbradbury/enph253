#if !defined(MYMOTOR_H)
#define MYMOTOR_H

#include <Arduino.h>

class MyMotor {
    private:
        PinName forwardPin;
        PinName backwardPin;
        int speed;
        int defaultSpeed;

    public:
        MyMotor(PinName forwardPin, PinName backwardPin, int defaultSpeed);

        void modulateSpeed(int value);

        void setSpeed(int speed);

        void stop();

        void start();
};
#endif //MYMOTOR_H
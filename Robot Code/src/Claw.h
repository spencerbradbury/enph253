#if !defined(CLAW_H)
#define CLAW_H

#include <Arduino.h>
#include <BetterServo.h>

class Claw
{
private:
    int armUp;
    int armDown;
    int clawOpen;
    int clawClosed;
    BetterServo armServo;
    BetterServo clawServo;
    uint8_t ultraTrig;
    uint8_t ultraEcho;

    void moveServo(BetterServo servo, int start, int end);

public:
    Claw(PinName arm_servo, PinName claw_servo, uint8_t ultrasonic_trigger, uint8_t ultrasonic_echo, int claw_open, int claw_close, int arm_up, int arm_down);

    void start();

    int getDistance();

    void pickUp();
};

#endif // CLAW_H

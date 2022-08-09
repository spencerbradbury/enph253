#if !defined(CLAW_H)
#define CLAW_H

#include <Arduino.h>
#include <BetterServo.h>

#define ULTRASONIC_TIMEOUT 5000UL // microseconds
#define SERVO_STEP_DELAY 15

class Claw
{
private:
    bool seeMagnet;

    int armUp;
    int armDown;
    int armVertical;
    int clawOpen;
    int clawClosed;
    int clawNeutral;
    BetterServo armServo;
    BetterServo clawServo;
    uint8_t ultraTrig;
    uint8_t ultraEcho;
    uint8_t hallSensor;

    void moveServo(BetterServo servo, int start, int end);

    void seeBomb();

public:
    Claw(PinName arm_servo, PinName claw_servo, uint8_t ultrasonic_trigger, uint8_t ultrasonic_echo, uint8_t hall_sensor, int claw_open, int claw_close, int claw_neutral, int arm_up, int arm_down, int arm_vertical);

    void start();

    int getDistance();

    void pickUp();
};

#endif // CLAW_H

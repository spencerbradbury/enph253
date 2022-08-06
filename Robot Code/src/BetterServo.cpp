#include "BetterServo.h"
#include <Arduino.h>

BetterServo::BetterServo() {}

BetterServo::BetterServo(PinName servoPin)
{
    this->servoPin = servoPin;
}

void BetterServo::write(int angle)
{
    int dutyCycle = map(angle, -90, 90, 600, 2400);
    pwm_start(servoPin, 50, map(dutyCycle, 0, 20000, 0, 16384), RESOLUTION_14B_COMPARE_FORMAT);
}

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
    pwm_start(servoPin, 50, dutyCycle, TimerCompareFormat_t::MICROSEC_COMPARE_FORMAT);
}

void BetterServo::writems(int dutyCycle)
{
    pwm_start(servoPin, 50, dutyCycle, TimerCompareFormat_t::MICROSEC_COMPARE_FORMAT);
}
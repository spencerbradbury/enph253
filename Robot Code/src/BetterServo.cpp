#include "BetterServo.h"
#include <Arduino.h>

BetterServo::BetterServo() {}

BetterServo::BetterServo(PinName servoPin)
{
    this->servoPin = servoPin;
}

void BetterServo::write(int angle)
{
    
    int dutyCycle = map(angle, 0, 180, 400, 2600);
    pwm_start(servoPin, 50, dutyCycle, TimerCompareFormat_t::MICROSEC_COMPARE_FORMAT);
}

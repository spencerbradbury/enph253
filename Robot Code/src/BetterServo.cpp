#include "BetterServo.h"
#include <Arduino.h>

BetterServo::BetterServo(PinName servoPin){
    this->servoPin = servoPin;
}

void BetterServo::write(int angle){
    int dutyCycle = map(angle, -90, 90, 800, 2200);
    pwm_start(servoPin, 50, map(dutyCycle, 0, 20000, 0, 1023), RESOLUTION_10B_COMPARE_FORMAT);
}

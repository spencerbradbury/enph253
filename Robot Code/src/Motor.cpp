#include "Motor.h"
#include <Arduino.h>
#define MOTOR_CLOCK_FREQ 100 // might have to change to 50

Motor::Motor(PinName forwardPin, PinName backwardPin, int defaultSpeed)
{
    this->forwardPin = forwardPin;
    this->backwardPin = backwardPin;
    this->speed = defaultSpeed;
    this->defaultSpeed = defaultSpeed;
}

void Motor::modulateSpeed(int value)
{
    this->speed = defaultSpeed + value;
    start();
}

void Motor::setSpeed(int speed)
{
    this->speed = speed;
}

void Motor::stop()
{
    pwm_start(forwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
    pwm_start(backwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
}

void Motor::activeStop()
{
    int originalSpeed = this->speed;
    setSpeed(-40);
    start();
    delay(100);
    stop();
    setSpeed(originalSpeed);
}



void Motor::start(){
    stop();
    if (this->speed > 0)
    {
        pwm_start(forwardPin, MOTOR_CLOCK_FREQ, map(this->speed, 0, 100, 0, 1023), RESOLUTION_10B_COMPARE_FORMAT);
    }
    else if (this->speed < 0)
    {
        pwm_start(backwardPin, MOTOR_CLOCK_FREQ, map(-this->speed, 0, 100, 0, 1023), RESOLUTION_10B_COMPARE_FORMAT);
    }
}

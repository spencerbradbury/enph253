#include "Motor.h"
#include <Arduino.h>


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

void Motor::setDefaultSpeed(int speed){
    this->defaultSpeed = speed;
}

void Motor::stop()
{
    pwm_start(forwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
    pwm_start(backwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
}

void Motor::activeStop()
{
    setSpeed(-speed);
    start();
    delay(100);
    stop();
    setSpeed(defaultSpeed);
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

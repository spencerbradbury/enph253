#include "Encoder.h"
#include <Arduino.h>
// #include <digital_io.h>


void Encoder::handleInterrupt()
{
    if (digitalReadFast(this->pin2))
    {
        count++;
    }
    else
    {
        count--;
    }

    // uint32_t time = millis();

    // this->speed = DISTANCE_PER_COUNT / (time - this->lastTime);
    // this->lastTime = time;
}

Encoder::Encoder(uint8_t pin1, PinName pin2)
{
    this->pin1 = pin1;
    this->pin2 = pin2;
    this->count = 0;
    this->lastTime = millis();
    attachInterrupt(digitalPinToInterrupt(pin1), std::bind(&Encoder::handleInterrupt, this), RISING);
}

int Encoder::getCount()
{
    return this->count;
}

int Encoder::getDistance()
{
    return (this->count * DISTANCE_PER_COUNT / 10.0);
}

void Encoder::resetCount()
{
    this->count = 0;
}

void Encoder::setCount(int x)
{
    this->count = x;
}
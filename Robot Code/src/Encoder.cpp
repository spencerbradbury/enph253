#include "Encoder.h"
#include <Arduino.h>

#define countPerRotation 1441

void handleInterrupt()
{
}

Encoder::Encoder(uint8_t pin1)
{
    this->pin1 = pin1;
    this->count = 0;
}

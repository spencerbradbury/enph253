#include "MyEncoder.h"
#include <Arduino.h>

#define countPerRotation 1441

void handleInterrupt()
{
}

MyEncoder::MyEncoder(uint8_t pin1)
{
    this->pin1 = pin1;
    this->count = 0;
}

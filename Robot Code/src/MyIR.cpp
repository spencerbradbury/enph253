/*
Yellow -> analog read
Blue -> select eye- high/low for each
brown -> mosfet clear - high = clear, clear before i read, add a delay if shit is wacky
*/
#include "MyIR.h"
#include <Arduino.h>
#include <utility>

#define leftEye HIGH
#define rightEye LOW

MyIR::MyIR(uint8_t readPin, uint8_t eyeSelectPin, uint8_t clearPin)
{
    this->readPin = readPin;
    this->eyeSelectPin = eyeSelectPin;
    this->clearPin = clearPin;
}

std::pair<int, int> MyIR::read()
{
    std::pair<int, int> data;

    digitalWrite(eyeSelectPin, leftEye);
    digitalWrite(clearPin, HIGH);
    digitalWrite(clearPin, LOW);
    data.first = analogRead(readPin);

    digitalWrite(eyeSelectPin, rightEye);
    digitalWrite(clearPin, HIGH);
    digitalWrite(clearPin, LOW);
    data.second = analogRead(readPin);

    return data;
}
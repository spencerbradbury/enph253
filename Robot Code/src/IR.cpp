/*
Yellow -> analog read
Blue -> select eye- high/low for each
brown -> mosfet clear - high = clear, clear before i read, add a delay if shit is wacky
*/
#include "IR.h"
#include <Arduino.h>
#include <utility>

#define leftEye HIGH
#define rightEye LOW

IR::IR(uint8_t readPin, uint8_t eyeSelectPin, uint8_t clearPin)
{
    this->readPin = readPin;
    this->eyeSelectPin = eyeSelectPin;
    this->clearPin = clearPin;
}

std::pair<int, int> IR::read()
{
    std::pair<int, int> data;

    digitalWrite(eyeSelectPin, leftEye);
    digitalWrite(clearPin, HIGH);
    // delayMicroseconds(1000);
    digitalWrite(clearPin, LOW);
    data.first = analogRead(readPin);

    digitalWrite(eyeSelectPin, rightEye);
    digitalWrite(clearPin, HIGH);
    // delayMicroseconds(1000);
    digitalWrite(clearPin, LOW);
    data.second = analogRead(readPin);

    return data;
}

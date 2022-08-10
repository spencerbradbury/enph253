/*
Yellow -> analog read
Blue -> select eye- high/low for each
brown -> mosfet clear - high = clear, clear before i read, add a delay if shit is wacky
*/
#include "IR.h"
#include <Arduino.h>
#include <utility>

IR::IR(uint8_t readPin, uint8_t eyeSelectPin, uint8_t clearPin)
{
    this->readPin = readPin;
    this->eyeSelectPin = eyeSelectPin;
    this->clearPin = clearPin;
}

std::pair<int, int> IR::read()
{
    std::pair<int, int> data;

    digitalWrite(clearPin, HIGH);
    digitalWrite(eyeSelectPin, leftEye);
    delay(22);
    digitalWrite(clearPin, LOW);
    data.second = analogRead(readPin);

    digitalWrite(clearPin, HIGH);
    digitalWrite(eyeSelectPin, rightEye);
    delay(22);
    digitalWrite(clearPin, LOW);
    data.first = analogRead(readPin);

    return data;
}

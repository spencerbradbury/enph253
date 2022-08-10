#if !defined(IR_H)
#define IR_H

#include <Arduino.h>
#include <utility>

#define leftEye LOW     
#define rightEye HIGH

class IR
{
private:
    std::pair<int, int> data;
    uint8_t readPin;
    uint8_t eyeSelectPin;
    uint8_t clearPin;

public:
    IR(uint8_t readPin, uint8_t eyeSelect, uint8_t clear);

    std::pair<int, int> read();
};

#endif // IR_H

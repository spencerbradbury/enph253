#if !defined(MYIR_H)
#define MYIR_H

#include <Arduino.h>
#include <utility>

class MyIR
{
private:
    std::pair<int, int> data;
    uint8_t readPin;
    uint8_t eyeSelectPin;
    uint8_t clearPin;

public:
    MyIR(uint8_t readPin, uint8_t eyeSelect, uint8_t clear);

    std::pair<int, int> read();
};

#endif // MYIR_H
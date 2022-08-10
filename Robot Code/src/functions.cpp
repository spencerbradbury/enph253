#include <Arduino.h>
#include "IR.h"
#include "pins.h"
#include "constants.h"
#include "main.h"
#include "PID.h"
#include "Motor.h"

extern IR IRSensors;
extern int idolCount;
extern States state;
extern PID tapePID;
extern Motor leftMotor;
extern Motor rightMotor;


// Data.first is right, second is left
int irError()
{
    std::pair<int, int> data = IRSensors.read();

    int rawError = data.first - data.second;

#if DEBUG
    display_handler.printf("right: %d \n", data.first);
    display_handler.printf("left: %d \n", data.second);
    display_handler.printf("Error: %d \n", data.first - data.second);
#endif

    if (rawError > 300)
    {
        return (-5);
    }

    if (rawError > 100)
    {
        return (-3);
    }

    if (rawError > 10)
    {
        return (-1);
    }

    if (rawError < -300)
    {
        return (5);
    }

    if (rawError < -100)
    {
        return (3);
    }

    if (rawError < -10)
    {
        return (1);
    }

    return (0);
}

int tapeError()
{
    int leftValue = analogRead(LINE_FOLLOW_LEFT);
    int rightValue = analogRead(LINE_FOLLOW_RIGHT);

    bool leftOnTape = (leftValue > REFLECTANCE_THRESHOLD);
    bool rightOnTape = (rightValue > REFLECTANCE_THRESHOLD);

#if DEBUG
    display_handler.printf("left: %d, %d\n", leftValue, leftOnTape);
    display_handler.printf("right: %d, %d\n", rightValue, rightOnTape);
#endif

    if (leftValue > EDGE_THRESHOLD || rightValue > EDGE_THRESHOLD)
    {
        if (idolCount <= 1)
        {
            state = ChickenWire;
        }
        return (tapePID.getlastErr());
    }

    if (leftOnTape && !rightOnTape)
    {
        return (1);
    }
    else if (!leftOnTape && rightOnTape)
    {
        return (-1);
    }
    else if (!leftOnTape && !rightOnTape)
    {
        if (tapePID.getlastErr() > 0)
        {
            return (3);
        }
        else if (tapePID.getlastErr() < 0)
        {
            return (-3);
        }
    }
    else
    {
        return (0);
    }
}

// Positive Value Turns Left
void modulateMotors(int value)
{
    if (value > 0)
    {
        leftMotor.modulateSpeed(-value);
    }
    else if (value < 0)
    {
        rightMotor.modulateSpeed(value);
    }
    else
    {
        rightMotor.modulateSpeed(0);
        leftMotor.modulateSpeed(0);
    }
}


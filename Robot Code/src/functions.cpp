#include <Arduino.h>
#include <utility>
#include "IR.h"
#include "pins.h"
#include "constants.h"
#include "main.h"
#include "PID.h"
#include "Motor.h"
#include "Encoder.h"

extern IR IRSensors;
extern int idolCount;
extern States state;
extern PID tapePID;
extern PID irPID;
extern Motor leftMotor;
extern Motor rightMotor;
extern Encoder leftEncoder;
extern Encoder rightEncoder;

// Data.first is right, second is left
int irError()
{
    std::pair<int, int> data = IRSensors.read();

    int rawError = data.first - data.second;

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

    if (data.first == 0 && data.second == 0)
    {
        if (irPID.getlastErr() > 0)
        {
            return (5);
        }
        else if (irPID.getlastErr() < 0)
        {
            return (-5);
        }
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

int angleToCount(int angle)
{
    return (8.3942 * angle - 56.744);
}

// Positive = counter clockwise (left), degrees (roughly)
void turn(int angle)
{
    leftMotor.stop();
    rightMotor.stop();

    if (angle == 0)
    {
        return;
    }

    if (angle > 0)
    {
        int referenceCount = rightEncoder.getCount();
        rightMotor.setSpeed(30);
        leftMotor.setSpeed(-30);
        while ((rightEncoder.getCount() - referenceCount) < angleToCount(angle))
        {
            leftMotor.start();
            rightMotor.start();
        }
    }
    else
    {
        int referenceCount = leftEncoder.getCount();
        leftMotor.setSpeed(30);
        rightMotor.setSpeed(-30);
        while ((leftEncoder.getCount() - referenceCount) < angleToCount(abs(angle)))
        {
            leftMotor.start();
            rightMotor.start();
        }
    }
    leftMotor.stop();
    rightMotor.stop();
    leftMotor.setSpeed(MOTOR_SPEED);
    rightMotor.setSpeed(MOTOR_SPEED);
}

void correctedTurn(int angle)
{
    leftMotor.stop();
    rightMotor.stop();

    if (angle == 0)
    {
        return;
    }

    if (angle > 0)
    {
        int referenceCount = rightEncoder.getCount();
        rightMotor.setSpeed(30);
        leftMotor.setSpeed(-30);
        while ((rightEncoder.getCount() - referenceCount) < angleToCount(angle))
        {
            leftMotor.start();
            rightMotor.start();
        }
        leftMotor.stop();
        rightMotor.stop();

        rightMotor.setSpeed(-20);
        leftMotor.setSpeed(20);
        while ((rightEncoder.getCount() - referenceCount) > angleToCount(angle))
        {
            leftMotor.start();
            rightMotor.start();
        }
    }
    else
    {
        int referenceCount = leftEncoder.getCount();
        leftMotor.setSpeed(30);
        rightMotor.setSpeed(-30);
        while ((leftEncoder.getCount() - referenceCount) < angleToCount(abs(angle)))
        {
            leftMotor.start();
            rightMotor.start();
        }
        leftMotor.stop();
        rightMotor.stop();

        rightMotor.setSpeed(20);
        leftMotor.setSpeed(-20);
        while ((leftEncoder.getCount() - referenceCount) > angleToCount(angle))
        {
            leftMotor.start();
            rightMotor.start();
        }
    }
    leftMotor.stop();
    rightMotor.stop();
    leftMotor.setSpeed(MOTOR_SPEED);
    rightMotor.setSpeed(MOTOR_SPEED);
}

bool turnToTape(int angle)
{
    leftMotor.stop();
    rightMotor.stop();

    if (angle == 0)
    {
        return false;
    }

    if (angle > 0)
    {
        int referenceCount = rightEncoder.getCount();
        rightMotor.setSpeed(30);
        leftMotor.setSpeed(-30);
        while ((rightEncoder.getCount() - referenceCount) < angleToCount(angle))
        {
            if ((analogRead(LINE_FOLLOW_LEFT) > REFLECTANCE_THRESHOLD) || (analogRead(LINE_FOLLOW_RIGHT) > REFLECTANCE_THRESHOLD))
            {
                rightMotor.stop();
                leftMotor.stop();
                return true;
            }
            leftMotor.start();
            rightMotor.start();
        }
    }
    else
    {
        int referenceCount = leftEncoder.getCount();
        leftMotor.setSpeed(30);
        rightMotor.setSpeed(-30);
        while ((leftEncoder.getCount() - referenceCount) < angleToCount(abs(angle)))
        {
            if ((analogRead(LINE_FOLLOW_LEFT) > REFLECTANCE_THRESHOLD) || (analogRead(LINE_FOLLOW_RIGHT) > REFLECTANCE_THRESHOLD))
            {
                rightMotor.stop();
                leftMotor.stop();
                return true;
            }
            leftMotor.start();
            rightMotor.start();
        }
    }
    leftMotor.stop();
    rightMotor.stop();
    leftMotor.setSpeed(MOTOR_SPEED);
    rightMotor.setSpeed(MOTOR_SPEED);
    return false;
}

bool turnToIR(int angle)
{
    leftMotor.stop();
    rightMotor.stop();

    if (angle == 0)
    {
        return false;
    }

    if (angle > 0)
    {
        int referenceCount = rightEncoder.getCount();
        rightMotor.setSpeed(30);
        leftMotor.setSpeed(-30);
        while ((rightEncoder.getCount() - referenceCount) < angleToCount(angle))
        {
            std::pair<int, int> data = IRSensors.read();
            if (data.first >= 10 && data.second >= 10)
            {
                rightMotor.stop();
                leftMotor.stop();
                return true;
            }
            leftMotor.start();
            rightMotor.start();
        }
    }
    else
    {
        int referenceCount = leftEncoder.getCount();
        leftMotor.setSpeed(30);
        rightMotor.setSpeed(-30);
        while ((leftEncoder.getCount() - referenceCount) < angleToCount(abs(angle)))
        {
            std::pair<int, int> data = IRSensors.read();
            if (data.first != 0 && data.second != 0)
            {
                rightMotor.stop();
                leftMotor.stop();
                return true;
            }
            leftMotor.start();
            rightMotor.start();
        }
    }
    leftMotor.stop();
    rightMotor.stop();
    leftMotor.setSpeed(MOTOR_SPEED);
    rightMotor.setSpeed(MOTOR_SPEED);
    return false;
}

// Positive = counter clockwise (left), degrees (roughly)
void turnWide(int angle)
{
    leftMotor.stop();
    rightMotor.stop();

    if (angle == 0)
    {
        return;
    }

    if (angle > 0)
    {
        int referenceCount = rightEncoder.getCount();
        rightMotor.setSpeed(45);
        while ((rightEncoder.getCount() - referenceCount) < angleToCount(angle))
        {
            rightMotor.start();
        }
    }
    else
    {
        int referenceCount = leftEncoder.getCount();
        leftMotor.setSpeed(45);
        while ((leftEncoder.getCount() - referenceCount) < angleToCount(abs(angle)))
        {
            leftMotor.start();
        }
    }
    leftMotor.stop();
    rightMotor.stop();
    leftMotor.setSpeed(MOTOR_SPEED);
    rightMotor.setSpeed(MOTOR_SPEED);
}

// Positive = forward, cm
void driveSlowly(int distance)
{
    leftMotor.stop();
    rightMotor.stop();

    if (distance == 0)
    {
        return;
    }

    if (distance > 0)
    {
        leftMotor.setSpeed(30);
        rightMotor.setSpeed(30);
        int referenceDistance = rightEncoder.getDistance();

        while ((rightEncoder.getDistance() - referenceDistance) < distance)
        {
            leftMotor.start();
            rightMotor.start();
        }
    }
    else
    {
        leftMotor.setSpeed(-30);
        rightMotor.setSpeed(-30);
        int refereenceDistance = rightEncoder.getDistance();

        while ((rightEncoder.getDistance() - refereenceDistance) > distance)
        {
            leftMotor.start();
            rightMotor.start();
        }
    }
    leftMotor.stop();
    rightMotor.stop();
    leftMotor.setSpeed(MOTOR_SPEED);
    rightMotor.setSpeed(MOTOR_SPEED);
}

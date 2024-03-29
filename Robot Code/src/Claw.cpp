#include "Claw.h"
#include <Arduino.h>
#include "BetterServo.h"

extern bool seenBombYet;

void Claw::seeBomb()
{
    this->seeMagnet = true;
    clawServo.write(clawOpen);
}

void Claw::moveServo(BetterServo servo, int start, int end)
{

    if (start < end)
    {
        for (int pos = start; pos <= end; pos += 1)
        {
            servo.write(pos);
            delay(SERVO_STEP_DELAY);
            if (seeMagnet)
            {
                break;
            }
        }
    }
    else
    {
        for (int pos = start; pos >= end; pos -= 1)
        {
            servo.write(pos);
            delay(SERVO_STEP_DELAY);
            if (seeMagnet)
            {
                break;
            }
        }
    }
}

Claw::Claw(PinName armPin, PinName clawPin, uint8_t ultrasonicTrigger, uint8_t ultrasonicEcho, uint8_t hallSensor, int clawOpen, int clawClosed, int clawNeutral, int armUp, int armDown, int armVertical)
{
    seeMagnet = false;
    BetterServo armServo(armPin);
    BetterServo clawServo(clawPin);
    this->armServo = armServo;
    this->clawServo = clawServo;
    this->ultraTrig = ultrasonicTrigger;
    this->ultraEcho = ultrasonicEcho;
    this->hallSensor = hallSensor;

    this->clawOpen = clawOpen;
    this->clawClosed = clawClosed;
    this->clawNeutral = clawNeutral;
    this->armUp = armUp;
    this->armDown = armDown;
    this->armVertical = armVertical;

    attachInterrupt(digitalPinToInterrupt(hallSensor), std::bind(&Claw::seeBomb, this), FALLING);
}

void Claw::pickUp()
{
    if (!seenBombYet)
    {
        if (!seeMagnet)
        {
            moveServo(armServo, armVertical, armVertical + 65);
        }
        if (!seeMagnet)
        {
            clawServo.write(clawOpen);
            delay(400);
            clawServo.write(clawClosed);
            delay(300);
            clawServo.write(clawOpen);
            delay(300);
        }
        if (!seeMagnet)
        {
            moveServo(armServo, armVertical + 65, armDown);
        }
        if (!seeMagnet)
        {
            clawServo.write(clawClosed);
            delay(400);
        }
        // moveServo(armServo, armDown, armUp);
        armServo.write(armVertical);
        delay(400);
        clawServo.write(clawNeutral);
    }
    else
    {
        clawServo.write(clawOpen);
        delay(300);
        moveServo(armServo, armVertical, armDown);
        clawServo.write(clawClosed);
        delay(300);
        armServo.write(armUp);
        delay(300);
        clawServo.write(clawOpen);
        delay(300);
        moveServo(armServo, armUp, armVertical);
        clawServo.write(clawNeutral);
    }
    // delay(400);
    // moveServo(armServo, armUp, armVertical);
    if (seeMagnet)
    {
        seenBombYet = true;
    }
    this->seeMagnet = false;
}

void Claw::start()
{
    armServo.write(armVertical);
    clawServo.write(clawNeutral);
}

int Claw::getDistance()
{
    int duration;
    digitalWrite(ultraTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(ultraTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultraTrig, LOW);
    // duration = pulseIn(ultraEcho, HIGH);
    duration = pulseIn(ultraEcho, HIGH, ULTRASONIC_TIMEOUT);
    return ((duration / 2) / 29.1);
}

void Claw::armIn()
{
    armServo.write(armUp);
    clawServo.write(clawNeutral);
}
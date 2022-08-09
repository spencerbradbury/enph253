#include "Claw.h"
#include <Arduino.h>
#include "BetterServo.h"

void Claw::moveServo(BetterServo servo, int start, int end)
{
  if (start < end)
  {
    for (int pos = start; pos <= end; pos += 1)
    {
      servo.write(pos);
      delay(SERVO_STEP_DELAY);
    }
  }
  else
  {
    for (int pos = start; pos >= end; pos -= 1)
    {
      servo.write(pos);
      delay(SERVO_STEP_DELAY);
    }
  }
}

Claw::Claw(PinName armPin, PinName clawPin, uint8_t ultrasonicTrigger, uint8_t ultrasonicEcho, int clawOpen, int clawClosed, int clawNeutral, int armUp, int armDown)
{
  BetterServo armServo(armPin);
  BetterServo clawServo(clawPin);
  this->armServo = armServo;
  this->clawServo = clawServo;
  this->ultraTrig = ultrasonicTrigger;
  this->ultraEcho = ultrasonicEcho;
  this->clawOpen = clawOpen;
  this->clawClosed = clawClosed;
  this->clawNeutral = clawNeutral;
  this->armUp = armUp;
  this->armDown = armDown;
}

void Claw::pickUp()
{
  clawServo.write(clawOpen);
  delay(400);
  moveServo(armServo, armUp, armDown);
  clawServo.write(clawClosed);
  delay(400);
  moveServo(armServo,armDown, armUp);
  clawServo.write(clawOpen);
  delay(400);
  clawServo.write(clawNeutral);
  delay(400);
}

void Claw::start()
{
  armServo.write(armUp);
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

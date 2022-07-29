#include "MyClaw.h"
#include <Arduino.h>
#include <Servo.h>

#define ULTRASONIC_TIMEOUT 900 // microseconds
#define SERVO_STEP_DELAY 15

void MyClaw::moveServo(Servo servo, int start, int end)
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

MyClaw::MyClaw(uint8_t armPin, uint8_t clawPin, uint8_t ultrasonicTrigger, uint8_t ultrasonicEcho, int clawOpen, int clawClose, int armUp, int armDown)
{
  Servo armServo;
  Servo clawServo;

  armServo.attach(armPin);
  clawServo.attach(clawPin);

  this->armServo = armServo;
  this->clawServo = clawServo;
  this->ultraTrig = ultrasonicTrigger;
  this->ultraEcho = ultrasonicEcho;
  this->clawOpen = clawOpen;
  this->clawClosed = clawClosed;
  this->armUp = armUp;
  this->armDown = armDown;
}

void MyClaw::pickUp()
{
  moveServo(clawServo, clawClosed, clawOpen);
  moveServo(armServo, armUp, armDown);
  moveServo(clawServo, clawOpen, clawClosed);
  moveServo(armServo, armDown, armUp);
  moveServo(clawServo, clawClosed, clawOpen);
  moveServo(clawServo, clawOpen, clawClosed);
}

void MyClaw::start()
{
  armServo.write(armUp);
  clawServo.write(clawClosed);
}

int MyClaw::getDistance()
{
  int duration;
  digitalWrite(ultraTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(ultraTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultraTrig, LOW);
  duration = pulseIn(ultraEcho, HIGH, ULTRASONIC_TIMEOUT);
  return ((duration / 2) / 29.1);
}
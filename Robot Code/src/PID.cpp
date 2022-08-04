#include "PID.h"
#include <Arduino.h>

PID::PID(int kp, int kd, int ki, int maxI)
{
    this->kp = kp;
    this->kd = kd;
    this->ki = ki;
    this->integral = 0;
    this->maxI = maxI;

    this->lastErr = 0;
}

void PID::setKD(int value)
{
    this->kd = value;
}

void PID::setKI(int value)
{
    this->ki = value;
}

void PID::setKP(int value)
{
    this->kp = value;
}

void PID::setMAXI(int value)
{
    this->maxI = value;
}

int PID::pid(int error)
{
    int p = kp * error;
    int d = kd * (error - lastErr);
    int i = ki * error + i; // integral
    i = (i > maxI) ? maxI : i;
    i = (i < -maxI) ? -maxI : i;

    lastErr = error;

    return (p + i + d);
}

int PID::getKP(){
    return(this->kp);
}

int PID::getlastErr(){
    return(this->lastErr);
}
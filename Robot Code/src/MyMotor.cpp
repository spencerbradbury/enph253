#include "MyMotor.h"
#include <Arduino.h>
#define MOTOR_CLOCK_FREQ 100

MyMotor::MyMotor(PinName forwardPin, PinName backwardPin, int defaultSpeed){
    this->forwardPin = forwardPin;
    this->backwardPin = backwardPin;
    this->speed = defaultSpeed;
    this->defaultSpeed = defaultSpeed;
}

void MyMotor::modulateSpeed(int value){
    this->speed = defaultSpeed + value;
    start();
}

void MyMotor::setSpeed(int speed){
    this->speed = speed;
}

void MyMotor::stop(){
    pwm_start(forwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
    pwm_start(backwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
}

void MyMotor::start(){
    stop();
    if (this->speed > 0)
    {
        pwm_start(forwardPin, MOTOR_CLOCK_FREQ, map(this->speed, 0, 100, 0, 1023), RESOLUTION_10B_COMPARE_FORMAT);
    }
    else if (this->speed < 0){
        pwm_start(backwardPin, MOTOR_CLOCK_FREQ, map(-this->speed, 0, 100, 0, 1023), RESOLUTION_10B_COMPARE_FORMAT);
    }          
}

// class MyMotor {
//     private:
//         PinName forwardPin;
//         PinName backwardPin;
//         int speed;
//         int defaultSpeed;
        

//     public:
//         MyMotor(PinName forwardPin, PinName backwardPin, int defaultSpeed){
//             this->forwardPin = forwardPin;
//             this->backwardPin = backwardPin;
//             this->speed = defaultSpeed;
//             this->defaultSpeed = defaultSpeed;
//         }

//         void modulateSpeed(int value){
//             this->speed = defaultSpeed + value;
//             start();
//         }

//         void setSpeed(int speed){
//             this->speed = speed;
//         }

//         void stop(){
//             pwm_start(forwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
//             pwm_start(backwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
//         }

//         void start(){
//             stop();
//             if (this->speed > 0)
//             {
//                 pwm_start(forwardPin, MOTOR_CLOCK_FREQ, map(this->speed, 0, 100, 0, 1023), RESOLUTION_10B_COMPARE_FORMAT);
//             }
//             else if (this->speed < 0){
//                 pwm_start(backwardPin, MOTOR_CLOCK_FREQ, map(-this->speed, 0, 100, 0, 1023), RESOLUTION_10B_COMPARE_FORMAT);
//             }          
//         }
// };
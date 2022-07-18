#define MOTOR_CLOCK_FREQ 100

class MyMotor {
    private:
        PinName forwardPin;
        PinName backwardPin;
        int speed;
        

    public:
        MyMotor(PinName forwardPin, PinName backwardPin, int defaultSpeed){
            this->forwardPin = forwardPin;
            this->backwardPin = backwardPin;
            this->speed = defaultSpeed;
        }

        void setSpeed(int speed){
            this->speed = speed;
            start();
        }

        void stop(){
            pwm_start(forwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
            pwm_start(backwardPin, MOTOR_CLOCK_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
        }

        void start(){
            stop();
            if (speed > 0)
            {
                pwm_start(forwardPin, MOTOR_CLOCK_FREQ, map(speed, 0, 100, 0, RESOLUTION_10B_COMPARE_FORMAT), RESOLUTION_10B_COMPARE_FORMAT);
            }
            else if (speed < 0){
                pwm_start(backwardPin, MOTOR_CLOCK_FREQ, map(-speed, 0, 100, 0, RESOLUTION_10B_COMPARE_FORMAT), RESOLUTION_10B_COMPARE_FORMAT);
            }          
        }
};
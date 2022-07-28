#define DEBUG 0

#include <Adafruit_SSD1306.h>
#include "MyMotor.h"
#include "MyEncoder.h"
#include "MyIR.h"

//SCREEN CONNECTIONS SCK -> B6, SDA -> B7
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible

#define MOTOR_LEFT_F PB_6
#define MOTOR_LEFT_B PB_7
#define MOTOR_RIGHT_F PB_8
#define MOTOR_RIGHT_B PB_9
#define LINE_FOLLOW_RIGHT PA7
#define LINE_FOLLOW_LEFT PB0
#define IR_SELECT PA11
#define IR_RESET PA12
#define IR_READ PA5

#define REFLECTANCE_THRESHOLD 200

// #define PID_MAX_INT 10

MyMotor leftMotor(MOTOR_LEFT_F, MOTOR_LEFT_B, 50);
MyMotor rightMotor(MOTOR_RIGHT_F, MOTOR_RIGHT_B, 50);
// MyEncoder leftEncoder(LEFT_ENCODER_1, LEFT_ENCODER_2);
MyIR IRSensors(IR_READ, IR_SELECT, IR_RESET);

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int tapePidErr = 0;
int tapePidLastErr = 0;
int tapeKp = 20; // 40(15 -- 17) 50(20)
int tapeKd = 8; //40(7 -- 8) 50(8)
// int ki = 0;

int IRKp = 0;
int IRKd = 0;
int IRPidErr = 0;
int IRPidLastErr = 0;
int abcdefgh = 0;


void PIDControl(int pidInput){
  // void pwm_start(PinName pin, uint32_t clock_freq, uint32_t value, TimerCompareFormat_t resolution){}= defaultPWM;
  // Defualt PWM is standard pwm value that will be modulated based on the steering requirement
  // Steering requirement is PID value, + left slows, - right slows.

  if (pidInput > 0){
    leftMotor.modulateSpeed(-pidInput);
  }
  else if (pidInput < 0 ){
    rightMotor.modulateSpeed(pidInput);
  }
  else{
    leftMotor.modulateSpeed(0);
    rightMotor.modulateSpeed(0);
  }
}

int IRPID(){
    std::pair<int, int> data = IRSensors.read();

    IRPidErr = data.first - data.second;

    int p = IRKp * IRPidErr;
    int d = IRKd * (IRPidErr - IRPidLastErr);

    IRPidLastErr = IRPidErr;

    return(p + d);
}

int tapePID(){  
  int leftValue = analogRead(LINE_FOLLOW_LEFT);
  int rightValue = analogRead(LINE_FOLLOW_RIGHT);

  if (leftValue > 600 || rightValue > 600){
    return (tapeKp * tapePidLastErr);
  }

  bool leftOnTape = (leftValue > REFLECTANCE_THRESHOLD);
  bool rightOnTape = (rightValue > REFLECTANCE_THRESHOLD);

  #if DEBUG
  display_handler.print("left: ");
  display_handler.print(analogRead(LINE_FOLLOW_LEFT));
  display_handler.print(", ");
  display_handler.println(leftOnTape);
  display_handler.print("right: ");
  display_handler.print(analogRead(LINE_FOLLOW_RIGHT));
  display_handler.print(", ");
  display_handler.println(rightOnTape);
  #endif

  if (leftOnTape && !rightOnTape){
    tapePidErr = 1;
  }
  else if (!leftOnTape && rightOnTape){
    tapePidErr = -1;
  }
  else if (!leftOnTape && !rightOnTape){
    if (tapePidLastErr > 0){
      tapePidErr = 3;
    }
    else if (tapePidLastErr < 0){
      tapePidErr = -3;
    }
  }
  else{
    tapePidErr = 0;
  }

  display_handler.print("tapePidErr: ");
  display_handler.println(tapePidErr);

  int p = tapeKp * tapePidErr; // proportional
  int d = tapeKd * (tapePidErr - tapePidLastErr); //derivative
  //Hopefully don't need integral
  // int i = ki * pidErr + i; //integral
  // i = (i > PID_MAX_INT) ? PID_MAX_INT : i;
  // i = (i < -PID_MAX_INT) ? -PID_MAX_INT : i;

  tapePidLastErr = tapePidErr;
  return(p /*+ i*/ + d);
}

void setup() {
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
  pinMode(LINE_FOLLOW_LEFT, INPUT_ANALOG);
  pinMode(LINE_FOLLOW_RIGHT, INPUT_ANALOG);
  }

void loop() {
  display_handler.clearDisplay();
  display_handler.setCursor(0,0);
  display_handler.println(abcdefgh);
  leftMotor.start();
  rightMotor.start();
  PIDControl(tapePID());
  display_handler.display();
  abcdefgh++;
  };

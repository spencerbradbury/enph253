#include <Adafruit_SSD1306.h>
#include <MyMotor.h>
#include <MyEncoder.h>

//SCREEN CONNECTIONS SCK -> B6, SDA -> B7
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible

#define MOTOR_LEFT_B PA_0
#define MOTOR_LEFT_F PA_1
#define MOTOR_RIGHT_B PA_2
#define MOTOR_RIGHT_F PA_3
#define LEFT_ENCODER_1 PA5
#define LEFT_ENCODER_2 PA6
#define LINE_FOLLOW_RIGHT PA7
#define LINE_FOLLOW_LEFT PB0

#define REFLECTANCE_THRESHOLD 200

// #define PID_MAX_INT 10

MyMotor leftMotor(MOTOR_LEFT_F, MOTOR_LEFT_B, 40);
MyMotor rightMotor(MOTOR_RIGHT_F, MOTOR_RIGHT_B, 40);
MyEncoder leftEncoder(LEFT_ENCODER_1, LEFT_ENCODER_2);

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int leftCount = 0;
int rightCount = 0;
int pidErr = 0;
int pidLastErr = 0;
int kp = 17; // 40(15 -- 17) 50(20)
int kd = 8; //40(7 -- 8) 50(8)
// int ki = 0;
int abcdefgh = 0;

void handle_left_motor(){
  leftCount++;
}

void handle_right_motor(){
  rightCount++;
}

void tapeFollow(int pidInput){
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


/*
Both on tape: x=0
left off right on: x=-1
left on right off: x = 1 
both off (left last on): x = 5
both off (right last on): x = -5
*/

int motorPID(int startCase){  
  bool leftOnTape = (analogRead(LINE_FOLLOW_LEFT) > REFLECTANCE_THRESHOLD);
  bool rightOnTape = (analogRead(LINE_FOLLOW_RIGHT) > REFLECTANCE_THRESHOLD);

  display_handler.print("left: ");
  display_handler.print(analogRead(LINE_FOLLOW_LEFT));
  display_handler.print(", ");
  display_handler.println(leftOnTape);
  display_handler.print("right: ");
  display_handler.print(analogRead(LINE_FOLLOW_RIGHT));
  display_handler.print(", ");
  display_handler.println(rightOnTape);

  if (leftOnTape && !rightOnTape){
    pidErr = 1;
  }
  else if (!leftOnTape && rightOnTape){
    pidErr = -1;
  }
  else if (!leftOnTape && !rightOnTape){
    if (pidLastErr > 0){
      pidErr = 3;
    }
    else if (pidLastErr < 0){
      pidErr = -3;
    }
  }
  else{
    pidErr = 0;
  }

  display_handler.print("pidErr: ");
  display_handler.println(pidErr);

  int p = kp * pidErr; // proportional
  int d = kd * (pidErr - pidLastErr); //derivative
  //Hopefully don't need integral
  // int i = ki * pidErr + i; //integral
  // i = (i > PID_MAX_INT) ? PID_MAX_INT : i;
  // i = (i < -PID_MAX_INT) ? -PID_MAX_INT : i;

  pidLastErr = pidErr;
  return(p /*+ i*/ + d);
}

void setup() {
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(MOTOR_LEFT_F, OUTPUT);
  // pinMode(MOTOR_LEFT_B, OUTPUT);
  // pinMode(PA2, OUTPUT); //MOTOR_RIGHT_F but bluepill is broken
  // pinMode(PA3, OUTPUT); //MOTOR_RIGHT_B but bluepill is borken
  // pinMode(LEFT_ENCODER, INPUT_PULLUP);
  // pinMode(RIGHT_ENCODER, INPUT_PULLUP);
  pinMode(LINE_FOLLOW_LEFT, INPUT_ANALOG);
  pinMode(LINE_FOLLOW_RIGHT, INPUT_ANALOG);
  // attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER), handle_left_motor, RISING);
  // attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER), handle_right_motor, RISING);
  }

void loop() {
  display_handler.clearDisplay();
  display_handler.setCursor(0,0);
  display_handler.println(abcdefgh);
  leftMotor.start();
  rightMotor.start();
  tapeFollow(motorPID(0));
  display_handler.display();
  abcdefgh++;
  };

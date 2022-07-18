#include <Adafruit_SSD1306.h>
#include <MyMotor.h>

//SCREEN CONNECTIONS SCK -> B6, SDA -> B7
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible

#define MOTOR_LEFT_F PA_0
#define MOTOR_LEFT_B PA_1
#define MOTOR_RIGHT_F PA_2
#define MOTOR_RIGHT_B PA_3
#define LEFT_ENCODER PA5
#define RIGHT_ENCODER PA6
#define LINE_FOLLOW_RIGHT PA7
#define LINE_FOLLOW_LEFT PB0

#define PID_MAX_INT 10

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int leftCount = 0;
int rightCount = 0;
int pidErr = 0;
int pidLastErr = 0;
int kp = 1;
int kd = 0;
int ki = 0;
int abcdefgh = 0;

void handle_left_motor(){
  leftCount++;
}

void handle_right_motor(){
  rightCount++;
}

void steerMotors(int defaultPWM, int pidInput){
  int leftPWM = defaultPWM;
  int rightPWM = defaultPWM;
  // void pwm_start(PinName pin, uint32_t clock_freq, uint32_t value, TimerCompareFormat_t resolution){}= defaultPWM;
  // Defualt PWM is standard pwm value that will be modulated based on the steering requirement
  // Steering requirement is PID value, + left slows, - right slows.

  if (pidInput > 0){
    leftPWM -= pidInput;
  }
  else if (pidInput < 0 ){
    rightPWM += pidInput;
  }

  display_handler.print("PID: ");
  display_handler.println(pidInput);

  pwm_start(MOTOR_LEFT_F, MOTOR_CLOCK_FREQ, leftPWM, RESOLUTION_10B_COMPARE_FORMAT);
  pwm_start(MOTOR_RIGHT_F, MOTOR_CLOCK_FREQ, rightPWM, RESOLUTION_10B_COMPARE_FORMAT);
}


/*
Both on tape: x=0
left off right on: x=-1
left on right off: x = 1 
both off (left last on): x = 5
both off (right last on): x = -5
*/

int motorPID(){  
  bool leftOnTape = (analogRead(LINE_FOLLOW_LEFT) > 300);
  bool rightOnTape = (analogRead(LINE_FOLLOW_RIGHT) > 300);

  display_handler.println(analogRead(LINE_FOLLOW_LEFT));
  display_handler.println(analogRead(LINE_FOLLOW_RIGHT));

  if (leftOnTape && !rightOnTape){
    pidErr = 1;
  }
  else if (!leftOnTape && rightOnTape){
    pidErr = -1;
  }
  else{
    pidErr = 0;
  }

  display_handler.print("pidErr: ");
  display_handler.println(pidErr);

  int p = kp * pidErr; // proportional
  int d = kd * (pidErr - pidLastErr); //derivative
  int i = ki * pidErr + i; //integral
  i = (i > PID_MAX_INT) ? PID_MAX_INT : i;
  i = (i < -PID_MAX_INT) ? -PID_MAX_INT : i;

  pidLastErr = pidErr;
  return(p + i + d);
}

void setup() {
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MOTOR_LEFT_F, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(PA2, OUTPUT); //MOTOR_RIGHT_F but bluepill is broken
  pinMode(PA3, OUTPUT); //MOTOR_RIGHT_B but bluepill is borken
  pinMode(LEFT_ENCODER, INPUT_PULLUP);
  pinMode(RIGHT_ENCODER, INPUT_PULLUP);
  pinMode(LINE_FOLLOW_LEFT, INPUT);
  pinMode(LINE_FOLLOW_RIGHT, INPUT);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER), handle_left_motor, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER), handle_right_motor, RISING);
  }

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  display_handler.clearDisplay();
  display_handler.setCursor(0,0);
  display_handler.println(abcdefgh);

  steerMotors(256, motorPID());

  display_handler.display();
  abcdefgh++;

  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);

  
};

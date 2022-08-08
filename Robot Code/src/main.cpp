#define DEBUG 1

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Motor.h"
#include "Encoder.h"
#include "IR.h"
#include "Claw.h"
#include "PID.h"
#include "BetterServo.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // This display does not have a reset pin accessible

#define MOTOR_RIGHT_F PB_6
#define MOTOR_RIGHT_B PB_7
#define MOTOR_LEFT_F PB_8
#define MOTOR_LEFT_B PB_9
#define LINE_FOLLOW_RIGHT PA3
#define LINE_FOLLOW_LEFT PA4
#define IR_SELECT PA11
#define IR_RESET PA12
#define IR_READ PA5
#define ULTRASONIC_TRIGGER PB12
#define ULTARSONIC_LEFT PB13
#define ULTRASONIC_RIGHT PB14
#define LEFT_ARM PB_1
#define LEFT_CLAW PB_0
#define RIGHT_ARM PA_7
#define RIGHT_CLAW PA_6
#define I2C_SDA PB11
#define I2C_SCL PB10
#define LEFT_ENCODER_1 PC14
#define LEFT_ENCODER_2 PB_4
#define RIGHT_ENCODER_1 PC13
#define RIGHT_ENCODER_2 PB_5

#define REFLECTANCE_THRESHOLD 200
#define MOTOR_SPEED 65
#define PID_MAX_INT MOTOR_SPEED / 3

#define LEFT_CLAW_OPEN -10
#define LEFT_CLAW_CLOSED 60
#define LEFT_CLAW_NEUTRAL 50
#define LEFT_ARM_UP 65
#define LEFT_ARM_DOWN -70
#define LEFT_ARM_VERTICAL 40

#define RIGHT_CLAW_OPEN -40
#define RIGHT_CLAW_CLOSED 35
#define RIGHT_CLAW_NEUTRAL 35
#define RIGHT_ARM_UP -65
#define RIGHT_ARM_DOWN 65
#define RIGHT_ARM_VERTICAL -40

#define LEDBUILTIN PB2

Claw leftClaw(LEFT_ARM, LEFT_CLAW, ULTRASONIC_TRIGGER, ULTARSONIC_LEFT, LEFT_CLAW_OPEN, LEFT_CLAW_CLOSED, LEFT_CLAW_NEUTRAL, LEFT_ARM_UP, LEFT_ARM_DOWN);
Claw rightClaw(RIGHT_ARM, RIGHT_CLAW, ULTRASONIC_TRIGGER, ULTRASONIC_RIGHT, RIGHT_CLAW_OPEN, RIGHT_CLAW_CLOSED, RIGHT_CLAW_NEUTRAL, RIGHT_ARM_UP, RIGHT_ARM_DOWN);
Motor leftMotor(MOTOR_LEFT_F, MOTOR_LEFT_B, MOTOR_SPEED);
Motor rightMotor(MOTOR_RIGHT_F, MOTOR_RIGHT_B, MOTOR_SPEED);
Encoder leftEncoder(LEFT_ENCODER_1, LEFT_ENCODER_2);
Encoder rightEncoder(RIGHT_ENCODER_1, RIGHT_ENCODER_2);
IR IRSensors(IR_READ, IR_SELECT, IR_RESET);
PID irPID(20, 8, 1, PID_MAX_INT);
PID tapePID(25, 10, 0, PID_MAX_INT);

#if DEBUG
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

int abcdefgh = 0;

int irError()
{
  std::pair<int, int> data = IRSensors.read();

  int rawError = data.first - data.second;

#if DEBUG
  display_handler.printf("left: %d \n", data.first);
  display_handler.printf("right: %d \n", data.second);
  display_handler.printf("Error: %d \n", data.first - data.second);
#endif

  if (rawError > 200)
  {
    return (5);
  }

  if (rawError > 100)
  {
    return (3);
  }

  if (rawError > 20)
  {
    return (1);
  }

  if (rawError < -200)
  {
    return (-5);
  }

  if (rawError < -100)
  {
    return (-3);
  }

  if (rawError < -20)
  {
    return (-1);
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

  if (leftValue > 800 || rightValue > 800)
  {
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

void setup()
{
  pinMode(LEDBUILTIN, OUTPUT);
  Wire.setSDA(PB11);
  Wire.setSCL(PB10);
#if DEBUG
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
#endif
  pinMode(LINE_FOLLOW_LEFT, INPUT_ANALOG);
  pinMode(LINE_FOLLOW_RIGHT, INPUT_ANALOG);
  pinMode(IR_SELECT, OUTPUT);
  pinMode(ULTRASONIC_TRIGGER, OUTPUT);
  pinMode(ULTRASONIC_RIGHT, INPUT);
  pinMode(ULTARSONIC_LEFT, INPUT);
  leftMotor.start();
  rightMotor.start();
  leftClaw.start();
  rightClaw.start();
  digitalWrite(LEDBUILTIN, HIGH);
  delay(2000);
  digitalWrite(LEDBUILTIN, LOW);
  delay(2000);
}

int jack = 0;

void loopwe()
{
  jack++;
  display_handler.clearDisplay();
  display_handler.setCursor(0, 0);
  display_handler.println(jack);
  display_handler.display();
  delay(100);
}

void loop()
{
#if DEBUG
  display_handler.clearDisplay();
  display_handler.setCursor(0, 0);
  display_handler.println(abcdefgh);

#endif
modulateMotors(tapePID.pid(tapeError()));
// modulateMotors(irPID.pid(irError()));

int distance = rightClaw.getDistance();
display_handler.println(distance);
if (distance > 10 && distance < 30)
{
  rightMotor.activeStop();
  leftMotor.activeStop();
  rightClaw.pickUp();
  rightMotor.start();
  leftMotor.start();
}
#if DEBUG
  display_handler.display();
  abcdefgh++;
#endif
};

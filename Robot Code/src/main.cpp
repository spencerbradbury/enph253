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

#define ULTRASONIC_TRIGGER PB12
#define ULTARSONIC_LEFT PB13
#define ULTRASONIC_RIGHT PB14
#define HALL_SENSOR PB15
#define BRIDGE_SERVO PA_8
#define FACE_SERVO PA_9
#define SPARE_PIN_1 PA10
#define IR_SELECT PA11
#define IR_RESET PA12
#define LEFT_EDGE PA15
#define RIGHT_EDGE PB3
#define LEFT_ENCODER_2 PB_4
#define RIGHT_ENCODER_1 PB5
#define MOTOR_RIGHT_F PB_6
#define MOTOR_RIGHT_B PB_7
#define MOTOR_LEFT_F PB_8
#define MOTOR_LEFT_B PB_9
#define I2C_SDA PB11
#define I2C_SCL PB10
#define LEFT_ARM PB_1
#define LEFT_CLAW PB_0
#define RIGHT_ARM PA_7
#define RIGHT_CLAW PA_6
#define IR_READ PA5
#define LINE_FOLLOW_LEFT PA4
#define LINE_FOLLOW_RIGHT PA3
#define SPARE_PIN_2 PA2
#define JOYSTICK_LR PA1
#define JOYSTICK_UD PA0
#define JOYSTICK_CLICK PC15
#define LEFT_ENCODER_1 PC14
#define RIGHT_ENCODER_2 PC_13

#define REFLECTANCE_THRESHOLD 200
#define EDGE_THRESHOLD 800
#define SONAR_PING_REQUIREMENT 3
#define MOTOR_SPEED 65

#define PID_MAX_INT MOTOR_SPEED / 3
#define LEFT_CLAW_OPEN -10
#define LEFT_CLAW_CLOSED 60
#define LEFT_CLAW_NEUTRAL 30
#define LEFT_ARM_UP 60
#define LEFT_ARM_DOWN -65
#define LEFT_ARM_VERTICAL 35
#define RIGHT_CLAW_OPEN -30
#define RIGHT_CLAW_CLOSED 55
#define RIGHT_CLAW_NEUTRAL 25
#define RIGHT_ARM_UP -70
#define RIGHT_ARM_DOWN 45
#define RIGHT_ARM_VERTICAL -45

#define LEDBUILTIN PB2

Claw leftClaw(LEFT_ARM, LEFT_CLAW, ULTRASONIC_TRIGGER, ULTARSONIC_LEFT, LEFT_CLAW_OPEN, LEFT_CLAW_CLOSED, LEFT_CLAW_NEUTRAL, LEFT_ARM_UP, LEFT_ARM_DOWN, LEFT_ARM_VERTICAL);
Claw rightClaw(RIGHT_ARM, RIGHT_CLAW, ULTRASONIC_TRIGGER, ULTRASONIC_RIGHT, RIGHT_CLAW_OPEN, RIGHT_CLAW_CLOSED, RIGHT_CLAW_NEUTRAL, RIGHT_ARM_UP, RIGHT_ARM_DOWN, RIGHT_ARM_VERTICAL);
Motor leftMotor(MOTOR_LEFT_F, MOTOR_LEFT_B, MOTOR_SPEED);
Motor rightMotor(MOTOR_RIGHT_F, MOTOR_RIGHT_B, MOTOR_SPEED);
Encoder leftEncoder(LEFT_ENCODER_1, LEFT_ENCODER_2);
Encoder rightEncoder(RIGHT_ENCODER_1, RIGHT_ENCODER_2);
IR IRSensors(IR_READ, IR_SELECT, IR_RESET);
PID irPID(20, 8, 1, PID_MAX_INT);
PID tapePID(25, 10, 0, PID_MAX_INT);

#if DEBUG
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BetterServo rightClawServo(RIGHT_CLAW);
BetterServo leftClawServo(LEFT_CLAW);
#endif

int abcdefgh = 0;
int sonarHits = 0;

enum States
{
    TapeFollow,
    ChickenWire,
    FindTape,
    IRFollow,
    Bridge,
    DropIdols,
};

static const char *States_str[] = {
    "TapeFollow",
    "ChickenWire",
    "FindTape",
    "IRFollow",
    "Bridge",
    "DropIdols"};

States state;

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

    if (leftValue > EDGE_THRESHOLD || rightValue > EDGE_THRESHOLD)
    {
        state = ChickenWire;
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
    Wire.setSDA(I2C_SDA);
    Wire.setSCL(I2C_SCL);
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
    state = TapeFollow;
}

void loop()
{
    int rdistanceTravelled = rightEncoder.getDistance();
    bool onTape = false;
    float angles[3] = {5.0, 10.0, 20.0};
#if DEBUG
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
    display_handler.println(abcdefgh);
    display_handler.printf("State: %s\n", States_str[state]);
    display_handler.printf("rdist: %d\n", rdistanceTravelled);
    display_handler.printf("Hits: %d\n", sonarHits);
#endif

    switch (state)
    {
    case TapeFollow:
        leftMotor.start();
        rightMotor.start();
        modulateMotors(tapePID.pid(tapeError()));

        if (rdistanceTravelled >= 160)
        {
            rightMotor.setDefaultSpeed(MOTOR_SPEED >> 1);
            leftMotor.setDefaultSpeed(MOTOR_SPEED >> 1);
            tapePID.setKP(13);
            tapePID.setKD(4);
            int distance = rightClaw.getDistance();
#if DEBUG
            display_handler.println(distance);
#endif
            if (distance > 10 && distance < 40)
            {
                sonarHits++;
            }
            if (sonarHits >= SONAR_PING_REQUIREMENT)
            {
                rightMotor.activeStop();
                leftMotor.activeStop();
                do
                {
                    leftMotor.setSpeed(20);
                    leftMotor.start();
                } while (rightClaw.getDistance() >= 15);
                leftMotor.stop();
                rightClaw.pickUp();
                rightMotor.setDefaultSpeed(MOTOR_SPEED);
                leftMotor.setDefaultSpeed(MOTOR_SPEED);
                tapePID.setKP(25);
                tapePID.setKD(8);
                rightMotor.start();
                leftMotor.start();
                sonarHits = 0;
                state = FindTape;
            }
        }
        break;

    case ChickenWire:
        display_handler.display();

        while (analogRead(LINE_FOLLOW_LEFT) > EDGE_THRESHOLD || analogRead(LINE_FOLLOW_RIGHT) > EDGE_THRESHOLD)
        {
            leftMotor.modulateSpeed(0);
            rightMotor.modulateSpeed(0);
        }
        state = FindTape;
        break;

    case FindTape:

        leftMotor.stop();
        rightMotor.stop();

        display_handler.display();
        delay(500);

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (!j)
                {
                    int startingCount = rightEncoder.getCount();
                    rightMotor.setSpeed(30);
                    leftMotor.setSpeed(-30);
                    rightMotor.start();
                    leftMotor.start();
                    do
                    {
                        int left = analogRead(LINE_FOLLOW_LEFT);
                        int right = analogRead(LINE_FOLLOW_RIGHT);

                        if ((left > REFLECTANCE_THRESHOLD && left < EDGE_THRESHOLD) || (right > REFLECTANCE_THRESHOLD && right < EDGE_THRESHOLD))
                        {
                            onTape = true;
                        }
                    } while (!onTape && (rightEncoder.getCount() - startingCount) < 50 * angles[i] /*/ANGLE_PER_COUNT*/);
                }
                else
                {
                    int startingCount = leftEncoder.getCount();
                    leftMotor.setSpeed(30);
                    rightMotor.setSpeed(-30);
                    rightMotor.start();
                    leftMotor.start();
                    do
                    {
                        int left = analogRead(LINE_FOLLOW_LEFT);
                        int right = analogRead(LINE_FOLLOW_RIGHT);

                        if ((left > REFLECTANCE_THRESHOLD && left < EDGE_THRESHOLD) || (right > REFLECTANCE_THRESHOLD && right < EDGE_THRESHOLD))
                        {
                            onTape = true;
                        }
                    } while (!onTape && (leftEncoder.getCount() - startingCount) < 50 * 2.0 * angles[i] /*/ANGLE_PER_COUNT*/);
                }
            }
        }
        rightMotor.stop();
        leftMotor.stop();
        rightMotor.setDefaultSpeed(MOTOR_SPEED);
        leftMotor.setDefaultSpeed(MOTOR_SPEED);

        state = TapeFollow;
        break;

    case IRFollow:
        display_handler.display();
        modulateMotors(irPID.pid(irError()));
        break;

    case Bridge:
        display_handler.display();
        break;

    case DropIdols:
        display_handler.display();
        break;

    default:
        break;
    }
#if DEBUG
    display_handler.display();
    abcdefgh++;
#endif
};

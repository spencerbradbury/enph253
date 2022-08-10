#define DEBUG 0

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Motor.h"
#include "Encoder.h"
#include "IR.h"
#include "Claw.h"
#include "PID.h"
#include "BetterServo.h"
#include "pins.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // This display does not have a reset pin accessible

#define REFLECTANCE_THRESHOLD 200
#define EDGE_THRESHOLD 800
#define MOTOR_SPEED 65

#define PID_MAX_INT MOTOR_SPEED / 3
#define LEFT_CLAW_OPEN -10
#define LEFT_CLAW_CLOSED 70
#define LEFT_CLAW_NEUTRAL 10
#define LEFT_ARM_UP 30
#define LEFT_ARM_DOWN -85
#define LEFT_ARM_VERTICAL 15
#define RIGHT_CLAW_OPEN -30
#define RIGHT_CLAW_CLOSED 55
#define RIGHT_CLAW_NEUTRAL 5
#define RIGHT_ARM_UP -70
#define RIGHT_ARM_DOWN 45
#define RIGHT_ARM_VERTICAL -45

#define LEDBUILTIN PB2

Claw leftClaw(LEFT_ARM, LEFT_CLAW, ULTRASONIC_TRIGGER_LEFT, ULTRASONIC_LEFT, HALL_SENSOR, LEFT_CLAW_OPEN, LEFT_CLAW_CLOSED, LEFT_CLAW_NEUTRAL, LEFT_ARM_UP, LEFT_ARM_DOWN, LEFT_ARM_VERTICAL);
Claw rightClaw(RIGHT_ARM, RIGHT_CLAW, ULTRASONIC_TRIGGER_RIGHT, ULTRASONIC_RIGHT, HALL_SENSOR, RIGHT_CLAW_OPEN, RIGHT_CLAW_CLOSED, RIGHT_CLAW_NEUTRAL, RIGHT_ARM_UP, RIGHT_ARM_DOWN, RIGHT_ARM_VERTICAL);
Motor leftMotor(MOTOR_LEFT_F, MOTOR_LEFT_B, MOTOR_SPEED);
Motor rightMotor(MOTOR_RIGHT_F, MOTOR_RIGHT_B, MOTOR_SPEED);
Encoder leftEncoder(LEFT_ENCODER_1, LEFT_ENCODER_2);
Encoder rightEncoder(RIGHT_ENCODER_1, RIGHT_ENCODER_2);
IR IRSensors(IR_READ, IR_SELECT, IR_RESET);
PID irPID(12, 4, 1, PID_MAX_INT);
PID tapePID(25, 10, 0, PID_MAX_INT);

#if DEBUG
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BetterServo rightClawServo(RIGHT_CLAW);
BetterServo leftClawServo(LEFT_CLAW);
BetterServo rightArmServo(RIGHT_ARM);
BetterServo leftArmServo(LEFT_ARM);
#endif

int abcdefgh = 0;
int lastPickupDistance = 0;
int sonarHits = 0;
int doubleCheckHits = 0;
int sonarResiliance = 5;
int doubleCheckResiliance = 2;
int idolCount = 0;
int irCatch = 0;
int irCatchThreshold = 5;
int distance = 0;
int startingCount;
int referenceCount;
int referenceDistance;

#if DEBUG
int hitsRequired[4] = {3, 2, 1, 1};
int doubleCheckHitsRequired[4] = {10, 10, 1, 1};
#else
int hitsRequired[4] = {20, 15, 15, 1};
int doubleCheckHitsRequired[4] = {10, 10, 10, 1};
#endif
float angles[3] = {10.0, 15.0, 20.0};

enum States
{
    TapeFollow,
    ChickenWire,
    FindTape,
    PassArch,
    IRFollow,
    Bridge,
    DropIdols,
};

static const char *States_str[] = {
    "TapeFollow",
    "ChickenWire",
    "FindTape",
    "PassArch",
    "IRFollow",
    "Bridge",
    "DropIdols"};

States state;

// Data.first is right, second is left
int irError()
{
    std::pair<int, int> data = IRSensors.read();

    int rawError = data.first - data.second;

#if DEBUG
    display_handler.printf("right: %d \n", data.first);
    display_handler.printf("left: %d \n", data.second);
    display_handler.printf("Error: %d \n", data.first - data.second);
#endif

    if (rawError > 300)
    {
        return (-5);
    }

    if (rawError > 100)
    {
        return (-3);
    }

    if (rawError > 10)
    {
        return (-1);
    }

    if (rawError < -300)
    {
        return (5);
    }

    if (rawError < -100)
    {
        return (3);
    }

    if (rawError < -10)
    {
        return (1);
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
        if (idolCount <= 1)
        {
            state = ChickenWire;
        }
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

// Positive Value Turns Left
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
    pinMode(ULTRASONIC_TRIGGER_LEFT, OUTPUT);
    pinMode(ULTRASONIC_TRIGGER_RIGHT, OUTPUT);
    pinMode(ULTRASONIC_RIGHT, INPUT);
    pinMode(ULTRASONIC_LEFT, INPUT);
    pinMode(HALL_SENSOR, INPUT_PULLUP);
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
#if DEBUG
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
    display_handler.println(abcdefgh);
    int duration;
    display_handler.printf("State: %s\n", States_str[state]);
    // display_handler.printf("rdist: %d\n", rdistanceTravelled);
    // display_handler.printf("Hits: %d\n", sonarHits);
    display_handler.display();
    abcdefgh++;
#endif

    switch (state)
    {
    case TapeFollow:
        leftMotor.start();
        rightMotor.start();
        modulateMotors(tapePID.pid(tapeError()));

        if (idolCount < 2)
        {
            if (rdistanceTravelled >= 160 && (rightEncoder.getDistance() - lastPickupDistance) > 70)
            {
                rightMotor.setDefaultSpeed(MOTOR_SPEED >> 1);
                leftMotor.setDefaultSpeed(MOTOR_SPEED >> 1);
                tapePID.setKP(13);
                tapePID.setKD(4);
                distance = rightClaw.getDistance();
#if DEBUG
                display_handler.println(distance);
#endif
                if (distance > 10 && distance < 32)
                {
                    sonarHits++;
                }
                else
                {
                    sonarResiliance--;
                    if (sonarResiliance <= 0)
                    {
                        sonarHits = 0;
                    }
                }
                if (sonarHits >= hitsRequired[idolCount])
                {
                    rightMotor.activeStop();
                    leftMotor.activeStop();

                    for (int i = 0; i < doubleCheckHitsRequired[idolCount]; i++)
                    {
                        int doubleCheckDistance = rightClaw.getDistance();
                        if (distance > 10 && distance < 25)
                        {
                            doubleCheckHits++;
                        }
                    }
                    if (doubleCheckHits >= (doubleCheckHitsRequired[idolCount] - doubleCheckResiliance))
                    {
                        do
                        {
                            leftMotor.setSpeed(20);
                            leftMotor.start();
                        } while (rightClaw.getDistance() >= 22);
                        leftMotor.activeStop();
                        lastPickupDistance = rightEncoder.getDistance();
                        rightClaw.pickUp();
                        idolCount++;
                    }
                    rightMotor.setDefaultSpeed(MOTOR_SPEED);
                    leftMotor.setDefaultSpeed(MOTOR_SPEED);
                    tapePID.setKP(25);
                    tapePID.setKD(8);
                    rightMotor.start();
                    leftMotor.start();
                    sonarHits = 0;
                    sonarResiliance = 5;
                    doubleCheckHits = 0;
                    state = FindTape;
                }
            }
        }
        break;

    case ChickenWire:
#if DEBUG
        display_handler.display();
#endif

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

#if DEBUG
        display_handler.display();
#endif
        delay(500);

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                if (!j)
                {
                    startingCount = rightEncoder.getCount();
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
                    startingCount = leftEncoder.getCount();
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

        if (idolCount == 2)
        {

            state = PassArch;
        }
        else
        {
            state = TapeFollow;
        }
        break;

    case PassArch:
        // #if DEBUG
        //         display_handler.display();
        // #endif
        rightClaw.armIn();
        leftClaw.armIn();
        leftMotor.setDefaultSpeed(MOTOR_SPEED >> 1);
        leftMotor.setSpeed(MOTOR_SPEED >> 1);
        rightMotor.setDefaultSpeed(MOTOR_SPEED >> 1);
        rightMotor.setSpeed(MOTOR_SPEED >> 1);
        referenceDistance = rightEncoder.getDistance();

        for (int i = 0; i < 3; i++)
        {
            do
            {
                leftMotor.start();
                rightMotor.start();
            } while ((rightEncoder.getDistance() - referenceDistance) < 10);

            leftMotor.activeStop();
            rightMotor.activeStop();

            referenceCount = rightEncoder.getCount();
            while ((rightEncoder.getCount() - referenceCount) < 69)
            {
                leftMotor.setSpeed(-30);
                rightMotor.setSpeed(30);
                leftMotor.start();
                rightMotor.start();
            }

            leftMotor.activeStop();
            rightMotor.activeStop();
        }

        leftMotor.setSpeed(MOTOR_SPEED >> 1);
        rightMotor.setSpeed(MOTOR_SPEED >> 1);

        //         if (irError() != 0)
        // {
        //     irCatch++;
        //     if (irCatch > irCatchThreshold)
        //     {
        //         leftMotor.modulateSpeed(0);
        //         rightMotor.modulateSpeed(0);
        //         delay(250);
        //         rightEncoder.resetCount();
        //         state = IRFollow;
        //     }
        // }

        state = IRFollow;

        break;

    case IRFollow:
#if DEBUG
        // display_handler.display();
#endif
        rightClaw.start();
        if (rightEncoder.getDistance() > 100)
        {
            rightMotor.setDefaultSpeed(MOTOR_SPEED >> 1);
            leftMotor.setDefaultSpeed(MOTOR_SPEED >> 1);
            irPID.setKP(8);
            irPID.setKD(2);
            distance = rightClaw.getDistance();
            if (distance > 10 && distance < 28)
            {
                sonarHits++;
            }
            else
            {
                sonarResiliance--;
                if (sonarResiliance <= 0)
                {
                    sonarHits = 0;
                }
            }
            if (sonarHits >= hitsRequired[idolCount])
            {
                rightMotor.activeStop();
                leftMotor.activeStop();

                for (int i = 0; i < doubleCheckHitsRequired[idolCount]; i++)
                {
                    int doubleCheckDistance = rightClaw.getDistance();
                    if (distance > 10 && distance < 25)
                    {
                        doubleCheckHits++;
                    }
                }
                if (doubleCheckHits >= (doubleCheckHitsRequired[idolCount] - doubleCheckResiliance))
                {
                    do
                    {
                        leftMotor.setSpeed(20);
                        leftMotor.start();
                    } while (rightClaw.getDistance() >= 16);
                    leftMotor.activeStop();
                    rightClaw.pickUp();
                    idolCount++;
                }
            }
            modulateMotors(irPID.pid(irError()));
            break;

        case Bridge:
#if DEBUG
            display_handler.display();
#endif
            break;

        case DropIdols:
#if DEBUG
            display_handler.display();
#endif
            break;

        default:
            break;
        }
    }
}
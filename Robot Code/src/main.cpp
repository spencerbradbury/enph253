/*-------------------------
* Author: Spencer Bradbury
* https://github.com/spencerbradbury/enph253
*
* Help from Tyler Wilson for servo code
*
*/

#define DEBUG 0
#define SIDE 1 // 1 is LEFT from audience view

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Motor.h"
#include "Encoder.h"
#include "IR.h"
#include "Claw.h"
#include "PID.h"
#include "BetterServo.h"
#include "pins.h"
#include "constants.h"
#include "main.h"
#include "functions.h"

Claw leftClaw(LEFT_ARM, LEFT_CLAW, ULTRASONIC_TRIGGER_LEFT, ULTRASONIC_LEFT, HALL_SENSOR, LEFT_CLAW_OPEN, LEFT_CLAW_CLOSED, LEFT_CLAW_NEUTRAL, LEFT_ARM_UP, LEFT_ARM_DOWN, LEFT_ARM_VERTICAL);
Claw rightClaw(RIGHT_ARM, RIGHT_CLAW, ULTRASONIC_TRIGGER_RIGHT, ULTRASONIC_RIGHT, HALL_SENSOR, RIGHT_CLAW_OPEN, RIGHT_CLAW_CLOSED, RIGHT_CLAW_NEUTRAL, RIGHT_ARM_UP, RIGHT_ARM_DOWN, RIGHT_ARM_VERTICAL);
Motor leftMotor(MOTOR_LEFT_F, MOTOR_LEFT_B, MOTOR_SPEED);
Motor rightMotor(MOTOR_RIGHT_F, MOTOR_RIGHT_B, MOTOR_SPEED);
Encoder leftEncoder(LEFT_ENCODER_1, LEFT_ENCODER_2);
Encoder rightEncoder(RIGHT_ENCODER_1, RIGHT_ENCODER_2);
IR IRSensors(IR_READ, IR_SELECT, IR_RESET);
PID irPID(13, 3, 1, PID_MAX_INT);
PID tapePID(25, 10, 0, PID_MAX_INT);

#if DEBUG
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BetterServo rightClawServo(RIGHT_CLAW);
BetterServo leftClawServo(LEFT_CLAW);
BetterServo rightArmServo(RIGHT_ARM);
BetterServo leftArmServo(LEFT_ARM);
#endif

bool seenBombYet = false;
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

// int hitsRequired[4] = {5, 5, 5, 1};
int hitsRequired[4] = {15, 15, 15, 1};
int doubleCheckHitsRequired[4] = {10, 10, 10, 1};
#endif

#if SIDE
int pickupAngles[4] = {0, 7, 0, 0};
#else
int pickupAngles[4] = {6, 5, 0, 0};
#endif

float angles[3] = {12.0, 15.0, 20.0};

extern States state;

static const char *States_str[] = {
    "TapeFollow",
    "ChickenWire",
    "FindTape",
    "BackDatAssUp",
    "FirstIRIdol",
    "SecondIrIdol",
    "Bridge",
    "DropIdols"};

States state;

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
    display_handler.printf("L Speed: %d\n", leftMotor.getDefaultSpeed());
    display_handler.printf("R Speed: %d\n", rightMotor.getDefaultSpeed());
    // display_handler.printf("rdist: %d\n", rdistanceTravelled);
    // display_handler.printf("Hits: %d\n", sonarHits);
    display_handler.display();
    abcdefgh++;
#endif
    switch (state)
    {
#pragma region TapeFollow
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
                if (distance > 10 && distance < 34)
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
                        delay(300);
                        turn(pickupAngles[idolCount]);
                        delay(300);
                        rightClaw.pickUp();
                        lastPickupDistance = rightEncoder.getDistance();
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
#pragma endregion

#pragma region ChickenWire
    case ChickenWire:
#if DEBUG
        display_handler.display();
#endif

        while (analogRead(LINE_FOLLOW_LEFT) > EDGE_THRESHOLD || analogRead(LINE_FOLLOW_RIGHT) > EDGE_THRESHOLD)
        {
            leftMotor.modulateSpeed(0);
            rightMotor.modulateSpeed(0);
        }
        driveSlowly(10);
        state = FindTape;
        break;
#pragma endregion

#pragma region FindTape
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

            state = BackDatAssUp;
        }
        else
        {
            state = TapeFollow;
        }
        break;
#pragma endregion

#pragma region BackDatAssUp
    case BackDatAssUp:
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

        delay(1500);
        for (int i = 0; i < 20; i++)
        {
            if (i < 5)
            {
                turn(-15);
            }
            else
            {
                if (turnToTape(-15))
                {
                    break;
                    i = 21;
                }
            }
            delay(400);
        }
        delay(500);
        turn(10);
        delay(1500);
        driveSlowly(-45);
        delay(1500);
        driveSlowly(17);
        delay(1500);
        turn(80);
        delay(500);
        driveSlowly(-60);
        leftMotor.setDefaultSpeed(MOTOR_SPEED);
        rightMotor.setDefaultSpeed(MOTOR_SPEED);
        leftMotor.setSpeed(MOTOR_SPEED);
        rightMotor.setSpeed(MOTOR_SPEED);

        for (int i = 0; i < 20; i++)
        {
            if (i < 5)
            {
                turn(-15);
            }
            else
            {
                if (turnToIR(-15))
                {
                    i = 21;
                }
            }
            delay(500);
        }
        delay(200);
        driveSlowly(15);
        delay(300);
        referenceDistance = rightEncoder.getDistance();
        state = FirstIRIdol;

        break;
#pragma endregion

#pragma region FirstIRIdol
    case FirstIRIdol:
#if DEBUG
        // display_handler.display();
#endif
        rightClaw.start();

        rightMotor.setDefaultSpeed(2 * MOTOR_SPEED / 3);
        leftMotor.setDefaultSpeed(2 * MOTOR_SPEED / 3);
        modulateMotors(irPID.pid(irError()));

        if (rightEncoder.getDistance() - referenceDistance >= 25)
        {
            while (true)
            {
                distance = rightClaw.getDistance();
#if DEBUG
                display_handler.println(distance);
#endif
                if (distance > 10 && distance < 34)
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
                        turn(pickupAngles[idolCount]);
                        rightClaw.pickUp();
                        idolCount++;
                    }
                    rightMotor.setDefaultSpeed(MOTOR_SPEED);
                    leftMotor.setDefaultSpeed(MOTOR_SPEED);
                    sonarHits = 0;
                    sonarResiliance = 5;
                    doubleCheckHits = 0;
                    state = Bridge;
                    break;
                }
            }
        }

        /* OLD CODE BEOFRE WE TRIED IR TRACKING
                leftMotor.setSpeed(-30);
                rightMotor.setSpeed(-30);

                do
                {
                    rightMotor.start();
                    leftMotor.start();
                    distance = leftClaw.getDistance();
                    if (distance > 5 && distance < 34)
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
                            int doubleCheckDistance = leftClaw.getDistance();
                            if (doubleCheckDistance > 5 && doubleCheckDistance < 25)
                            {
                                doubleCheckHits++;
                            }
                        }
                        if (doubleCheckHits >= (doubleCheckHitsRequired[idolCount] - doubleCheckResiliance))
                        {
                            int adjustmentDistance = leftClaw.getDistance();
                            if (adjustmentDistance > 20)
                            {
                                rightMotor.setSpeed(30);
                                while (leftClaw.getDistance() > 20)
                                {
                                    rightMotor.start();
                                }
                                rightMotor.stop();
                            }
                            else if (adjustmentDistance < 15)
                            {
                                turnWide(-40);
                                driveSlowly(15);
                                turn(20);
                                rightMotor.setSpeed(-30);
                                leftMotor.setSpeed(-30);
                                do
                                {
                                    distance = leftClaw.getDistance();
                                    leftMotor.start();
                                    rightMotor.start();
                                } while (distance < 5 || distance > 23);
                            }
                            leftClaw.pickUp();
                            idolCount++;
                            sonarHits = 0;
                            sonarResiliance = 5;
                            doubleCheckHits = 0;
                            state = SecondIRIdol;
                        }
                    }
                } while (state == FirstIRIdol);*/

        break;
#pragma endregion

#pragma region SecondIRIdol
    case SecondIRIdol:
        leftMotor.stop();
        rightMotor.stop();

        break;
#pragma endregion

#pragma region Bridge
    case Bridge:
#if DEBUG
        display_handler.display();
#endif
        leftMotor.stop();
        rightMotor.stop();
        leftClaw.armIn();
        rightClaw.armIn();
        break;
#pragma endregion

#pragma region Drop Idols
    case DropIdols:
#if DEBUG
        display_handler.display();
#endif
        break;
#pragma endregion

#pragma region Default
    default:
        break;
#pragma endregion
    }

#if DEBUG
    display_handler.display();
#endif
}